#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>

#include <net/if.h>
#include <netinet/if_ether.h>
#include <linux/if_packet.h>
#include <netinet/ip.h>

#define ENABLE_VLAN
#define FILL_8021Q

#define RAW_SOCK_MAX_FRM_SIZ 1536
#define RAW_SOCK_BUF_SIZE (64*1024*1024)
/* 25 ms */
#define RAW_SOCK_POLL_TIMEOUT 25

typedef struct fm_raw_sock_data_t_ {
    int fd;
    unsigned int buf_size;
    struct iovec *iovec;
    unsigned int iovmax;
    unsigned int iovhead;
    void *base;
} fm_raw_sock_data_t;

int fm_raw_pkt_sock_open(char *dev_name, fm_raw_sock_data_t *info)
{
    int raw_sock;
    struct ifreq ifr;
    struct sockaddr_ll sll;
    struct packet_mreq mr;
    struct tpacket_req req;
    int idx, i, k;

    printf("%s: starting.\n", __FUNCTION__);

    /* open raw packet socket */
    //raw_sock = socket(PF_PACKET, SOCK_RAW, 0);
    raw_sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if(raw_sock < 0) {
        printf("%s: raw socket: %s\n", __FUNCTION__, strerror(errno));
        return raw_sock;
    }

    /* find kernel ifindex of device */
    memset(&ifr, 0, sizeof(struct ifreq));
    strncpy(ifr.ifr_name, dev_name, sizeof(ifr.ifr_name) - 1);
    if(ioctl(raw_sock, SIOCGIFINDEX, &ifr) < 0) {
        printf("%s: ioctl(SIOCGIFINDEX): %s\n", 
                    __FUNCTION__, strerror(errno));
        goto err_out;
    }

    /* bind socket to device */
    memset(&sll, 0, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex =  ifr.ifr_ifindex;
    sll.sll_protocol = htons(ETH_P_ALL);
    if(bind(raw_sock, (struct sockaddr*)&sll, sizeof(sll))) {
        printf("%s: bind: %s\n",
                    __FUNCTION__, strerror(errno));
        goto err_out;
    }

    /* set device promiscuous */
    memset(&mr, 0, sizeof(mr));
    mr.mr_ifindex = ifr.ifr_ifindex;
    mr.mr_type = PACKET_MR_PROMISC;
    if(setsockopt(raw_sock, SOL_PACKET, PACKET_ADD_MEMBERSHIP,
                  (char*)&mr, sizeof(mr)) < 0) {
        printf("%s: setsockopt promisc: %s\n",
                    __FUNCTION__, strerror(errno));
        goto err_out;
    }

#ifdef ENABLE_VLAN
    int val, hdr_len;
    socklen_t len;

    /* Probe whether kernel supports TPACKET_V2 */
    val = TPACKET_V2;
    len = sizeof(val);
    if (getsockopt(raw_sock, SOL_PACKET, PACKET_HDRLEN, &val, &len) < 0) {
        if (errno == ENOPROTOOPT)
            goto err_out;
        printf("%s: Can't get TPACKET_V2 header len on socket %d: %s\n",
                   __FUNCTION__, raw_sock, strerror(errno)); 
        goto err_out;
    } 
    hdr_len = val;
    printf("%s, TPACKET_V2 header lenth is: %d\n", __FUNCTION__, hdr_len);

    /* Set raw packet socket version to V2 */
    val = TPACKET_V2;
    if (setsockopt(raw_sock, SOL_PACKET, PACKET_VERSION, 
                            &val, sizeof(val)) < 0) {
        printf("%s: Can't activate TPACKET_V2 on socket %d: %s\n",
                   __FUNCTION__, raw_sock, strerror(errno)); 
        goto err_out;
    }

    /* Reserve space for VLAN tag reconstruction */
    val = 4;
    if (setsockopt(raw_sock, SOL_PACKET, PACKET_RESERVE, 
                            &val, sizeof(val)) < 0) {
        printf("%s: Can't set up reserve on socket %d: %s\n",
                   __FUNCTION__, raw_sock, strerror(errno)); 
        goto err_out;
    }

    /* Enable auxiliary data to receive VLAN tag */
    val = 1;
    if (setsockopt(raw_sock, SOL_PACKET, PACKET_AUXDATA,
                    &val, sizeof(val)) == -1 && errno != ENOPROTOOPT) {
//    if (setsockopt(raw_sock, SOL_PACKET, PACKET_AUXDATA,
//                    &val, sizeof(val)) < 0) {
        printf("%s: setsockopt PACKET_AUXDATA: %s\n",
                    __FUNCTION__, strerror(errno));
        goto err_out;
    }
#endif

    info->fd = raw_sock;

    /* set up the packet ring */
    req.tp_block_size = getpagesize();
    req.tp_block_nr = info->buf_size/req.tp_block_size;
#ifdef ENABLE_VLAN
    req.tp_frame_size = TPACKET_ALIGN(TPACKET2_HDRLEN) +
#else
    req.tp_frame_size = TPACKET_ALIGN(TPACKET_HDRLEN) +
#endif
                        TPACKET_ALIGN(RAW_SOCK_MAX_FRM_SIZ+16);
    req.tp_frame_nr = req.tp_block_nr * (req.tp_block_size / req.tp_frame_size);
    if(setsockopt(raw_sock, SOL_PACKET, PACKET_RX_RING,
                  (void*)&req, sizeof(req))) {
        printf("%s: setsockopt(PACKET_RX_RING): %s\n",
               __FUNCTION__, strerror(errno));
        return -1;
    }

    /* map packet ring to user space */
    info->base = mmap(NULL, info->buf_size, (PROT_READ|PROT_WRITE),
                      (MAP_SHARED|MAP_LOCKED), raw_sock, 0);

    if(info->base == MAP_FAILED) {
        printf("%s: mmap: %s\n", __FUNCTION__, strerror(errno));
        return -1;
    }

    /* set up iovecs to frames in packet buffer */
    info->iovec = malloc(req.tp_frame_nr * sizeof(struct iovec));
    if(!info->iovec) {
        printf("%s: out of memory!\n", __FUNCTION__);
        return -1;
    }

    /* initialize all iovecs */
    for(idx=0, i=0; i<req.tp_block_nr ; i++) { /* for all blocks/pages */
        for(k=0; k<(req.tp_block_size / req.tp_frame_size); k++, idx++) {
            /* for each frame, whithin the block */
            info->iovec[idx].iov_base = (char*)info->base +
                            (req.tp_block_size * i) + (req.tp_frame_size * k);
            info->iovec[idx].iov_len = req.tp_frame_size;
        }
    }

    info->iovhead = 0;
    info->iovmax = req.tp_frame_nr - 1;

    return raw_sock;

err_out:
    if(raw_sock > 0)
        close(raw_sock);
    return -1;
}

/**
 *
 * Copied from pcap_mon.c
 *
 */

#define true  1
#define false 0

typedef unsigned long long uint64;
typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

#define packed __attribute__ ((__packed__))

/* default snap length (maximum bytes per packet to capture */
#define SNAP_LEN 1518

static inline int printable(u_char ch) {
    if(ch >= 33 && ch <= 126) {
        return true;
    } else {
        return false;
    }
}

void hex_dump(const u_char *pkt_start, const size_t len) 
{
#define BYTES_PER_LINE 16
    int addr = 0x0;
    int remainder = len;
    int i;
    u_char *pkt =  (u_char*) pkt_start;

    if(pkt == NULL || len <= 0)
        return;

    while(remainder > 0) {
        printf("%04x\t", addr);
        for(i=0; (i < BYTES_PER_LINE) && (i < remainder) ; i++) {
            printf("%02x ", pkt[i]);
            /* print an extra space in the middle */
            if(i == (BYTES_PER_LINE/2 -1)) {
                printf(" ");
            }
        }
        /* Fill spaces at the bottom line */
        for(;i < BYTES_PER_LINE; i++) {
            printf("   ");
            /* print an extra space in the middle */
            if(i == (BYTES_PER_LINE/2 -1)) {
                printf(" ");
            }
        }
        printf("\t");
        /* Print printable ASCII characters */
        for(i=0; (i < BYTES_PER_LINE) && (i < remainder) ; i++) {
            if(printable(pkt[i])) {
                printf("%c", pkt[i]);
            } else {
                printf(".");
            }
            /* print an extra space in the middle */
            if(i == (BYTES_PER_LINE/2 -1)) {
                printf(" ");
            }
        }
        printf("\n");
        remainder -= BYTES_PER_LINE;
        pkt += BYTES_PER_LINE;
        addr += BYTES_PER_LINE;
    }
}

void test_hex_dump()
{
    u_char buf1[64] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10, 0x11, 0x12, 'h', 'e', 'l', 'l', 'o'};

    hex_dump(buf1, 28);
}

#define MAC_BYTES   6

#define ETHER_TYPE_IPv4     0x0800 
#define ETHER_TYPE_ARP      0x0806
#define ETHER_TYPE_IPv6     0x86DD
#define ETHER_TYPE_VLAN     0x8100
#define ETHER_TYPE_QinQ     0x9100

#define IP_PROTO_ICMP   1
#define IP_PROTO_IGMP   2 
#define IP_PROTO_TCP    6
#define IP_PROTO_UDP    17

typedef struct packed ether_hdr_t {
    uint8   dst_MAC[MAC_BYTES];
    uint8   src_MAC[MAC_BYTES];
    uint16  ether_type;
} ether_hdr;

typedef struct packed vlan_hdr_t {
    uint16  vlan_tci;
    uint16  ether_type;
} vlan_hdr;

typedef struct packed icmp_hdr_t { 
    uint8   type;
    uint8   code;
    uint16  checksum;
    uint32  rest;
} icmp_hdr;

typedef struct packed ipv4_hdr_t {
    uint8   version_ihl;
    uint8   dscp_ecn;
    uint16  length;
    uint16  id;
    uint16  flags_frag;
    uint16  ttl_proto;
    uint16  crc;
    uint32  src_ip;
    uint32  dst_ip;
} ipv4_hdr;

typedef struct packed tcp_hdr_t {
    uint16  src_port;
    uint16  dst_port;
    uint32  seq;
    uint32  ack;
    uint16  flag;
    uint16  window;
    uint16  checksum;
    uint16  urgent;
} tcp_hdr;

typedef struct packed udp_hdr_t {
    uint16  src_port;
    uint16  dst_port;
    uint16  length;
    uint16  checksum;
} udp_hdr;

static inline void print_line() {
    printf("-----------------------------------------------\n");
}

void dissect_tcp(const u_char *packet) {
    tcp_hdr *hdr = (tcp_hdr*) packet;
    print_line();
    printf("TCP\n");
    printf("Dst Port: %d\nSrc Port: %d\n", ntohs(hdr->dst_port), ntohs(hdr->src_port));
}

void dissect_udp(const u_char *packet) {
    udp_hdr *hdr = (udp_hdr*) packet;
    print_line();
    printf("UDP\n");
    printf("Dst Port: %d\nSrc Port: %d\n", ntohs(hdr->dst_port), ntohs(hdr->src_port));
}

void dissect_icmp(const u_char *packet) {
    icmp_hdr *hdr = (icmp_hdr*) packet;
    print_line();
    printf("ICMP\nType: %d, Code %d ", hdr->type, hdr->code);
    if(hdr->type == 8 && hdr->code == 0) {
        printf("[Echo request (used to ping)]\n");
    } else if(hdr->type == 0 && hdr->code == 0) {
        printf("[Echo reply (used to ping)]\n");
    }
}

void dissect_igmp(const u_char *packet) {
    print_line();
    printf("IGMP packet\n");
}

void print_ipv4(const uint32* addr) {
    uint8 *pchar = (uint8 *) addr;
    int i;
    for(i=0; i<4 ; i++) {
        printf("%d", pchar[i]);
        if(i != 3) {
            printf(".");
        }
    }
}

static inline uint8 ip_proto(const ipv4_hdr *hdr) {
    return (ntohs(hdr->ttl_proto) & 0x00FF);
}

void dissect_ipv4(const u_char *packet) {
    ipv4_hdr *hdr = (ipv4_hdr*) packet;
    u_char *pdu = (u_char*)(packet + (hdr->version_ihl & 0x0F) * 4);
    uint8 proto = ip_proto(hdr);
    
    print_line();
    printf("Dst IPv4 addr: ");
    print_ipv4(&hdr->dst_ip);
    printf("\n");
    printf("Src IPv4 addr: ");
    print_ipv4(&hdr->src_ip);
    printf("\n");
    
    switch(proto) {
    case IP_PROTO_ICMP:
        dissect_icmp(pdu);
        break;
    case IP_PROTO_IGMP:
        dissect_igmp(pdu);
        break;
    case IP_PROTO_TCP:
        dissect_tcp(pdu);
        break;
    case IP_PROTO_UDP:
        dissect_udp(pdu);
        break;
    default:
        printf("IP protocol is: %d\n", proto);
        break;
    }
}

void dissect_ipv6(const u_char *packet) {
    print_line();
    printf("IPv6 packet\n");
}

void dissect_arp(const u_char *packet) {
    print_line();
    printf("ARP packet\n");
}

void print_mac(const uint8 mac[]) {
    int i;
    for(i=0 ; i<MAC_BYTES ; i++){
        printf("%02x", mac[i]);
        if(i != (MAC_BYTES-1)) {
            printf("-");
        }
    }
}

void dissect_vlan(const u_char *packet) {
    vlan_hdr *hdr = (vlan_hdr*) packet;
    u_char *pdu = (u_char*)(packet + 4);
    uint16 ether_type = ntohs(hdr->ether_type);
    
    print_line();
    printf("vlan: %d\n", ntohs(hdr->vlan_tci) & 0x0FFF);
    
    switch(ether_type){ 
    case ETHER_TYPE_IPv4:
        dissect_ipv4(pdu);
        break;
    case ETHER_TYPE_IPv6:
        dissect_ipv6(pdu);
        break;
    case ETHER_TYPE_ARP:
        dissect_arp(pdu);
        break;
    default:
        printf("Ether type is: %x\n", ether_type);
        break;
    }
}

void dissect_ether(const u_char *packet) {
    ether_hdr *hdr = (ether_hdr*) packet;
    u_char *pdu = (u_char*)(packet + 14);
    uint16 ether_type = ntohs(hdr->ether_type);

    print_line();
    printf("Dst MAC: ");
    print_mac(hdr->dst_MAC);
    printf("\n");
    printf("Src MAC: ");
    print_mac(hdr->src_MAC);
    printf("\n");

    switch(ether_type){ 
    case ETHER_TYPE_VLAN:
        dissect_vlan(pdu);
        break;
    case ETHER_TYPE_IPv4:
        dissect_ipv4(pdu);
        break;
    case ETHER_TYPE_IPv6:
        dissect_ipv6(pdu);
        break;
    case ETHER_TYPE_ARP:
        dissect_arp(pdu);
        break;
    default:
        printf("Ether type is: %x\n", ether_type);
        break;
    }
}

/*
 * The main function to deal with packets
 */

#if 0
void dissect_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
//void dissect_packet(const struct pcap_pkthdr *header, const u_char *packet) {
    static int count = 0;

    printf("###############################################\n");
    printf("Dissecting packet #%d\n", count++);
    printf("Time: %ld.%06ld\n", header->ts.tv_sec, header->ts.tv_usec);
    printf("Capture length: %d, Wire length: %d\n", header->caplen, header->len);
    print_line();
    hex_dump(packet, header->caplen);
    dissect_ether(packet);
    printf("\n");
}
#endif

void fm_raw_pkt_sock_read (fm_raw_sock_data_t *info, short int revents)
{
    static int count = 0;
    struct iovec *ring = info->iovec;
    u_char *pkt;
#ifdef ENABLE_VLAN
    struct tpacket2_hdr *h;
#else
    struct tpacket_hdr *h;
#endif

    h = ring[info->iovhead].iov_base;
    while (h->tp_status) {
        pkt = (u_char*)h + h->tp_mac;
        /* dissecting packets */
        printf("###############################################\n");
        printf("Dissecting packet #%d\n", count++);
#ifdef ENABLE_VLAN
        printf("Time: %d.%09d\n", h->tp_sec, h->tp_nsec);
#else
        printf("Time: %d.%06d\n", h->tp_sec, h->tp_usec);
#endif
        printf("Capture length: %d, Wire length: %d\n", h->tp_snaplen, h->tp_len);
#ifdef ENABLE_VLAN
#ifdef FILL_8021Q
        if(h->tp_vlan_tci & 0x0FFF) {
            pkt -= 4; 
            memmove(pkt, pkt + 4, 2 * 6);
            *(uint16_t*)(pkt+12) = htons(0x8100);
            *(uint16_t*)(pkt+14) = htons(h->tp_vlan_tci);
            h->tp_snaplen += 4;
            h->tp_len += 4;
        }
#else
        printf("VLAN is %d\n", h->tp_vlan_tci & 0x0FFF);
#endif  // endif FILL_8021Q
#endif  // endif ENABLE_VLAN
        print_line();
        hex_dump(pkt, h->tp_snaplen);
        dissect_ether(pkt);
        printf("\n");
        /* end dissecting packets */

        /* release packet back */
        h->tp_status = TP_STATUS_KERNEL;
        info->iovhead = (info->iovhead == info->iovmax) ? 0 : info->iovhead+1;
        h = ring[info->iovhead].iov_base;
    }
}

int main(int argc, char *argv[])
{
    char            *dev = NULL;                /* capture device name */
    //int             num_packets = -1;           /* number of packets to capture */
    fm_raw_sock_data_t sock_info;
    struct pollfd fds;
    int nfds, pres;

    /* Accept when user indicates a network device */
    if(argc >= 2 && argv[1] != NULL) {
        dev = argv[1];
    /* FIXME: need to validate the existence of the user provided device  */
    /*} else {
        dev = pcap_lookupdev(errbuf);
        if(dev == NULL) {
            fprintf(stderr, "Couldn't find default device: %s\n", errbuf);
            exit(-1);
        }
    */
    }

    printf("Sniffing on device: %s\n", dev);

    memset(&sock_info, 0, sizeof(sock_info));
    sock_info.buf_size = RAW_SOCK_BUF_SIZE;
    fm_raw_pkt_sock_open(dev, &sock_info);
    fds.fd = sock_info.fd;
    fds.revents = 0;
    fds.events = POLLIN | POLLERR;
    nfds = 1;

    printf("sockets open, ready to rx.\n");
    
    while(1) {
        pres = poll(&fds, nfds, RAW_SOCK_POLL_TIMEOUT);
        if(pres < 0) {
        } else if(pres > 0) {
            /* data should be ready */
            if(fds.revents != 0) {
                fm_raw_pkt_sock_read(&sock_info, fds.revents);
            }
        }
    }

#if 0
    /* open capture device */
    handle = pcap_open_live(dev, SNAP_LEN, true, 1000, errbuf);
    if(handle == NULL) {
        fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
        exit(-1);
    }

    /* make sure we are capturing on an Ethernet device */
    if(pcap_datalink(handle) != DLT_EN10MB) {
        fprintf(stderr, "%s is not an Ethernet device\n", dev);
        exit(-1);
    }

    /* now we loop through the packets */
    pcap_loop(handle, num_packets, dissect_packet, NULL);

    pcap_close(handle);
#endif
    
    printf("\nCapture complete.\n");

//    test_hex_dump();
    return 0;
}
