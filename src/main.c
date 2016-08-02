#include <stdio.h>
#include <thread.h>
#include <net/nic.h>
#include <net/packet.h>
#include <net/ether.h>
#include <net/arp.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/checksum.h>
#include <net/udp.h>

#include "dup.h"

void ginit(int argc, char** argv) {
}

void init(int argc, char** argv) {
	dup_init();
	iot_init();
	event_init();
}

inline void process(NIC* ni) {
	Packet* packet = nic_input(ni);
	if(!packet)
		return;
	
	printf("Hello\n");
// IoT Gateway not need echo
//	
//	if(arp_process(packet))
//		return;
//	
//	if(icmp_process(packet))
//		return;

	if(iot_process(packet))
		return;

	if(packet)
		nic_free(packet);
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
	
	thread_barrior();
	
	uint32_t i = 0;
	while(1) {
		uint32_t count = nic_count();
		if(count > 0) {
			i = (i + 1) % count;
			
			NIC* ni = nic_get(i);
			process(ni);
		}
		event_loop();
	}
	
	thread_barrior();
	
	destroy();
	
	thread_barrior();
	
	if(thread_id() == 0) {
		gdestroy(argc, argv);
	}
	
	return 0;
}
