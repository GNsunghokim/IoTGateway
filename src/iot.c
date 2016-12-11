#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <util/map.h>
#include <util/types.h>
#include <arpa/inet.h>
#undef IP_TTL //TODO fixhere
#include <net/packet.h>
#include <net/ether.h>
#include <net/arp.h>
#include <net/ip.h>
#include <net/udp.h>
#include <net/tcp.h>
#include <net/icmp.h>
#include <net/checksum.h>
#include <net/udp.h>

#include "util.h"
#include "iot.h"
#include "actuator.h"
#include "sensor.h"
#include "dup.h"
#include "rule.h"
#include "mqtt.h"

#define IOT_CONFIG_FILE	"iot_config.json"
#define IOT_DUP_FILE	"iot_dup.json"

bool iot_init() {
	sensor_database_init();
	actuator_database_init();
	rule_database_init();
	dup_init();

	json_object *jso = json_object_from_file(IOT_CONFIG_FILE);
	if(jso) { 
		printf("\"%s\" is opened\n", IOT_CONFIG_FILE);
		json_object_object_foreach(jso, key, child_object) {
			if(!strcmp(key, "sensors")) {
				printf("IoT Sensors:\n");
				for(int i =0; i < json_object_array_length(child_object); i++) {
					json_object* iot_object = json_object_array_get_idx(child_object, i);
					Sensor* sensor = sensor_json_create(iot_object);
					if(sensor) {
						sensor_database_add(sensor);
					}
					printf("\n");
				}
			} else if(!strcmp(key, "actuators")) {
				printf("IoT Actuators:\n");
				for(int i =0; i < json_object_array_length(child_object); i++) {
					json_object* iot_object = json_object_array_get_idx(child_object, i);
					Actuator* actuator = actuator_json_create(iot_object);
					if(actuator) {
						actuator_database_add(actuator);
					}
					printf("\n");
				}
			} else if(!strcmp(key, "rule")) {
				printf("Rule:\n");
				printf("\tName\t\tFunc\n\t\t\tAction\t\t\t\tDescription\n");
				for(int i =0; i < json_object_array_length(child_object); i++) {
					json_object* rule_object = json_object_array_get_idx(child_object, i);
					Rule* rule = rule_json_create(rule_object);
					if(rule) {
						rule_database_add(rule);
					}
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
		printf("\"%s\" is opened\n", IOT_DUP_FILE);
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

	return true;
}

// IoTDevice* iot_create_device(char* name, char* description, char* id, char* address) {
// 	if(!name) {
// 		printf("iot_create_device fail: name is null\n");
// 		return NULL;
// 	}
// 	if(map_contains(iot_database, name)) {
// 		printf("iot_create_device fail: already existing name\n");
// 		return NULL;
// 	}
// 
// 	IoTDevice* iotdevice = (IoTDevice*)malloc(sizeof(IoTDevice));
// 	if(!iotdevice) {
// 		printf("iot_create_device fail: malloc fail\n");
// 		return NULL;
// 	}
// 
// 	memset(iotdevice, 0, sizeof(IoTDevice));
// 
// 	iotdevice->name = (char*)malloc(strlen(name) + 1);
// 	if(!iotdevice->name)
// 		goto fail;
// 
// 	strcpy(iotdevice->name, name);
// 
// 	iotdevice->id = (char*)malloc(strlen(id) + 1);
// 	if(!iotdevice->id)
// 		goto fail;
// 
// 	strcpy(iotdevice->id, id);
// 
// 	iotdevice->address = inet_addr(address);
// 
// 	if(description) {
// 		iotdevice->description = (char*)malloc(strlen(description) + 1);
// 		if(!iotdevice->name)
// 			goto fail;
// 
// 		strcpy(iotdevice->description, description);
// 	}
// 
// 	iotdevice->sensors = map_create(16, map_string_hash, map_string_equals, NULL);
// 	if(!iotdevice->sensors) {
// 		goto fail;
// 	}
// 
// 	iotdevice->actuators = map_create(16, map_string_hash, map_string_equals, NULL);
// 	if(!iotdevice->actuators) {
// 		goto fail;
// 	}
// 
// 	if(!map_put(iot_database, iotdevice->id, iotdevice)) {
// 		goto fail;
// 	}
// 
// 	return iotdevice;
// fail:
// 	if(iotdevice->name) {
// 		free(iotdevice->name);
// 		iotdevice->name = NULL;
// 	}
// 	if(iotdevice->id) {
// 		free(iotdevice->id);
// 		iotdevice->id = NULL;
// 	}
// 	if(iotdevice->description) {
// 		free(iotdevice);
// 		iotdevice->description = NULL;
// 	}
// 	if(iotdevice->sensors) {
// 		map_destroy(iotdevice->sensors);
// 		iotdevice->sensors = NULL;
// 	}
// 	if(iotdevice->actuators) {
// 		map_destroy(iotdevice->actuators);
// 		iotdevice->actuators = NULL;
// 	}
// 	if(iotdevice) {
// 		free(iotdevice);
// 		iotdevice = NULL;
// 	}
// 	printf("IoT Device Create Fail\n");
// 
// 	return NULL;
// }
// 
// bool iot_json_create(json_object *jso) {
// 	char name[64];
// 	char id[32];
// 	char address[32];
// 	char description[128];
// 
// 	json_object_object_foreach(jso, key1, child_object1) {
// 		if(!strcmp(key1, "name")) {
// 			strcpy(name, json_object_to_json_string(child_object1));
// 		} else if(!strcmp(key1, "id")) {
// 			strcpy(id, json_object_to_json_string(child_object1));
// 		} else if(!strcmp(key1, "address")) {
// 			strncpy(address, json_object_to_json_string(child_object1), 32);
// 		} else if(!strcmp(key1, "description")) {
// 			strcpy(description, json_object_to_json_string(child_object1));
// 		} else {
// 			//printf("???\n");
// 		}
// 	}
// 
// 	remove_blank(name);
// 	remove_blank(id);
// 	remove_blank(address);
// 	remove_blank(description);
// 
// 	IoTDevice* iot_device = iot_create_device(name, description, id, address);
// 	if(!iot_device) {
// 		return false;
// 	}
// 	printf("\tName: %s\n", name);
// 	printf("\tID: %s\n", id);
// 	printf("\tAddress: %s\n", address);
// 	printf("\tDescription: %s\n", description);
// 
// 	json_object_object_foreach(jso, key2, child_object2) {
// 		if(!strcmp(key2, "sensor")) {
// 			printf("\tSensor:\n");
// 			printf("\t\tName\t\tBufferSize\n");
// 			for(int j = 0; j < json_object_array_length(child_object2); j++) {
// 				json_object* sensor_object = json_object_array_get_idx(child_object2, j);
// 				Sensor* sensor = sensor_json_create(sensor_object);
// 				if(!sensor) {
// 					return false; //TODO garbage collect
// 				}
// 				if(!iot_add_module(iot_device, IOT_DEVICE_SENSOR, (void*)sensor)) {
// 					return false; //TODO garbage collect
// 				}
// 			}
// 		} else if(!strcmp(key2, "actuator")) {
// 			printf("\tActuator:\n");
// 			printf("\t\tName\t\tActionName\t\tActionFunc\n");
// 			for(int j = 0; j < json_object_array_length(child_object2); j++) {
// 				json_object* actuator_object = json_object_array_get_idx(child_object2, j);
// 				Actuator* actuator = actuator_json_create(actuator_object);
// 				if(!actuator) {
// 					return false;
// 				}
// 				if(!iot_add_module(iot_device, IOT_DEVICE_ACTUATOR, (void*)actuator)) {
// 					return false;
// 				}
// 			}
// 		}
// 	}
// 
// 	return true;
// }
// 
// bool iot_delete_device(char* name) {
// 	if(!name) {
// 		printf("iot_delete_device fail: name is null\n");
// 		return false;
// 	}
// 	IoTDevice* iotdevice = map_remove(iot_database, name);
// 	if(!iotdevice) {
// 		printf("iot_delete_device fail: Can't found IoT Device");
// 		return false;
// 	}
// 
// 
// 	if(iotdevice->name) {
// 		free(iotdevice->name);
// 		iotdevice->name = NULL;
// 	}
// 	if(iotdevice->description) {
// 		free(iotdevice->description);
// 		iotdevice->description = NULL;
// 	}
// 	if(iotdevice->sensors) {
// 		//add map iterator and free all sensor
// 		map_destroy(iotdevice->sensors);
// 		iotdevice->sensors = NULL;
// 	}
// 	if(iotdevice->actuators) {
// 		//add map iterator and free all actuaotr
// 		map_destroy(iotdevice->actuators);
// 		iotdevice->actuators = NULL;
// 	}
// 	if(iotdevice) {
// 		free(iotdevice);
// 		iotdevice = NULL;
// 	}
// 	printf("iot_delete_device success");
// 	return true;
// }

static bool is_iot_packet(IP* ip) {
	if(ip->protocol == IP_PROTOCOL_UDP) {
		return false;
	} else if(ip->protocol == IP_PROTOCOL_TCP) {
		TCP* tcp = (TCP*)ip->body;
		if(endian16(tcp->destination) == 1883) { //check ip and port iot device ip and port 1883 == mqtt broker port
			uint16_t body_len = endian16(ip->length) - (ip->ihl * 4) - (tcp->offset * 4);
			if(body_len < sizeof(MQTT)) {
				return false;
			}
			MQTT* mqtt = (MQTT*)((uint8_t*)tcp + (tcp->offset * 4));

			return mqtt_process(mqtt);
		}
	}
	return false;
}

bool iot_process(Packet* packet) {
	Ether* ether = (Ether*)(packet->buffer + packet->start);
	if(endian16(ether->type) == ETHER_TYPE_IPv4) {
		IP* ip = (IP*)ether->payload;
		if(is_iot_packet(ip)) {
			rule_process();
			dup_process(packet->buffer, packet->end - packet->start);
			ni_free(packet);
			return true;
		}
	}

	ni_free(packet);
	return true;
}
