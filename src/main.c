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

#include "dup.h"
#include "iot.h"
#include "rule.h"

#define IOT_CONFIG_FILE	"iot_config.json"
#define IOT_DUP_FILE	"iot_dup.json"
#define IP_ADDR		0xc0a80a01

void ginit(int argc, char** argv) {
}

void init(int argc, char** argv) {
	dup_init();
	iot_init();

	//TODO event fix
	//event_init();
	rule_init();

	json_object *jso = json_object_from_file(IOT_CONFIG_FILE);
	if(jso) { 
		printf("%s is opened\n", IOT_CONFIG_FILE);
		json_object_object_foreach(jso, key, child_object) {

			if(!strcmp(key, "iot-device")) {
				printf("IoT Device:\n");
				for(int i =0; i < json_object_array_length(child_object); i++) {
					json_object* iot_object = json_object_array_get_idx(child_object, i);
					iot_json_create(iot_object);
					printf("\n");
				}
			} else if(!strcmp(key, "rule")) {
				printf("Rule:\n");
				printf("\tName\t\tFunc\n\t\t\tAction\t\t\t\tDescription\n");
				for(int i =0; i < json_object_array_length(child_object); i++) {
					json_object* rule_object = json_object_array_get_idx(child_object, i);
					rule_json_create(rule_object);
					printf("\n");
				}
			} else {
				printf("???\n");
			}
		}
		json_object_put(jso);
	}

	jso = json_object_from_file(IOT_DUP_FILE);
	if(jso) { 
		printf("%s is opened\n", IOT_DUP_FILE);
		json_object_object_foreach(jso, key, child_object) {

			if(!strcmp(key, "duplicator")) {
				printf("IoT Data Duplicator:\n");
				for(int i =0; i < json_object_array_length(child_object); i++) {
					json_object* iot_object = json_object_array_get_idx(child_object, i);
					dup_json_create(iot_object);
					printf("\n");
				}
			} else {
				printf("???\n");
			}
		}
		json_object_put(jso);
	}
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
