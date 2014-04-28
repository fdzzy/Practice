
TARGET = pcap_mon raw_pkt_sock

all: ${TARGET}

pcap_mon: pcap_mon.c
	gcc -Wall -Werror -g -o pcap_mon pcap_mon.c -L/usr/local/lib -lpcap

raw_pkt_sock: raw_pkt_sock.c
	gcc -Wall -Werror -g -o raw_pkt_sock raw_pkt_sock.c

clean:
	rm -f *.o ${TARGET}
