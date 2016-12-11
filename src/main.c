#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <thread.h>
//#include <util/event.h>
#include <net/ni.h>
#include <net/packet.h>
#include <net/ether.h>
#include <net/arp.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/checksum.h>
#include <net/udp.h>

#include "iot.h"

#define IP_ADDR		0xc0a80a01

void ginit(int argc, char** argv) {
}

void init(int argc, char** argv) {
	//TODO event fix
	//event_init();
	iot_init();
}

bool _arp_process(Packet* packet) {
	Ether* ether = (Ether*)(packet->buffer + packet->start);
	if(endian16(ether->type) == ETHER_TYPE_ARP) {
		// ARP response
		ARP* arp = (ARP*)ether->payload;
		if(endian16(arp->operation) == 1 && endian32(arp->tpa) == IP_ADDR) {
			ether->dmac = ether->smac;
			ether->smac = endian48(packet->ni->mac);
			arp->operation = endian16(2);
			arp->tha = arp->sha;
			arp->tpa = arp->spa;
			arp->sha = ether->smac;
			arp->spa = endian32(IP_ADDR);
			
			ni_output(packet->ni, packet);
			return true;
		}
	}

	return false;
}

void process(NetworkInterface* ni) {
	Packet* packet = ni_input(ni);
	if(!packet)
		return;
	
 	if(_arp_process(packet)) {
 		return;
	}

	if(iot_process(packet))
		return;

	ni_free(packet);
}

void destroy() {
}

void gdestroy() {
}

int main(int argc, char** argv) {
	printf("PacketNgin IoT Gateway Start\n");
	printf("Thread %d booting\n", thread_id());

	if(thread_id() == 0) {
		ginit(argc, argv);
	}
	
	thread_barrior();
	
	init(argc, argv);
	printf("Thread %d Initilized\n", thread_id());
	thread_barrior();
	
	uint32_t i = 0;
	while(1) {
		uint32_t count = ni_count();
		if(count > 0) {
			i = (i + 1) % count;
			
			NetworkInterface* ni = ni_get(i);
			process(ni);
		}
	}
	
	thread_barrior();
	
	destroy();
	
	thread_barrior();
	
	if(thread_id() == 0) {
		gdestroy(argc, argv);
	}
	
	return 0;
}
