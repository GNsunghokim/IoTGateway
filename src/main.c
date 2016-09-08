#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include <thread.h>
#include <util/event.h>
#include <net/nic.h>
#include <net/packet.h>
#include <net/ether.h>
#include <net/arp.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/checksum.h>
#include <net/udp.h>

#include "dup.h"
#include "iot.h"

#define CONFIG_FILE	"iot_config.json"

void ginit(int argc, char** argv) {
}

void init(int argc, char** argv) {
	dup_init();
	iot_init();
	event_init();

	json_object *jso = json_object_from_file("./example/iot_config.json");
	if (jso) {
		//json_object_put(jso);
		json_object_object_foreach(jso, key, child_object) {

			if(!strcmp(key, "iot-device")) {
				for(int i =0; i < json_object_array_length(child_object); i++) {
					json_object* iot_object = json_object_array_get_idx(child_object, i);
					iot_json_create(iot_object);
					printf("\n");
				}
			} else if(!strcmp(key, "rule")) {
				for(int i =0; i < json_object_array_length(child_object); i++) {
					json_object* rule_object = json_object_array_get_idx(child_object, i);
					rule_json_create(rule_object);
					printf("\n");
				}
			}
		}
	}
}

void process(NIC* ni) {
	Packet* packet = nic_input(ni);
	if(!packet)
		return;
	
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
