/*
 * Use libpcap to get data link frames
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pcap.h>
#include <netinet/in.h>

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

void print_version()
{
    const char *str = pcap_lib_version();
    printf("%s\n", str);
}

int main(int argc, char *argv[])
{
    char            *dev = NULL;                /* capture device name */
    char            errbuf[PCAP_ERRBUF_SIZE];   /* error buffer */
    pcap_t          *handle = NULL;             /* packet capture handle */
    int             num_packets = -1;           /* number of packets to capture */

    /* Accept when user indicates a network device */
    if(argc >= 2 && argv[1] != NULL) {
        dev = argv[1];
    /* FIXME: need to validate the existence of the user provided device  */
        if(strncmp(dev, "-v", 2) == 0) {
            print_version();
            exit(0);
        }
    } else {
        dev = pcap_lookupdev(errbuf);
        if(dev == NULL) {
            fprintf(stderr, "Couldn't find default device: %s\n", errbuf);
            exit(-1);
        }
    }

    printf("Sniffing on device: %s\n", dev);

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
    
    printf("\nCapture complete.\n");

//    test_hex_dump();
    return 0;
}
