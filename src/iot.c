#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <util/map.h>
#include <util/types.h>
#include <net/packet.h>
#include <net/ether.h>
#include <net/arp.h>
#include <net/ip.h>
#include <net/udp.h>
#include <net/tcp.h>
#include <net/icmp.h>
#include <net/checksum.h>
#include <net/udp.h>

#include "iot.h"
#include "actuator.h"
#include "sensor.h"
#include "mqtt.h"
#include "util.h"

#include <json.h>
#include <json_util.h>

static Map* iot_database;

bool iot_init() {
	iot_database = map_create(32, map_string_hash, map_string_equals, NULL);
	if(!iot_database)
		return false;


	return true;
}

IoTDevice* iot_create_device(char* name, char* description, char* id) {
	if(!name) {
		printf("iot_create_device fail: name is null\n");
		return NULL;
	}
	if(map_contains(iot_database, name)) {
		printf("iot_create_device fail: already existing name\n");
		return NULL;
	}

	IoTDevice* iotdevice = (IoTDevice*)malloc(sizeof(IoTDevice));
	if(!iotdevice) {
		printf("iot_create_device fail: malloc fail\n");
		return NULL;
	}

	memset(iotdevice, 0, sizeof(IoTDevice));

	iotdevice->name = (char*)malloc(strlen(name) + 1);
	if(!iotdevice->name)
		goto fail;

	strcpy(iotdevice->name, name);

	iotdevice->id = (char*)malloc(strlen(id) + 1);
	if(!iotdevice->id)
		goto fail;

	strcpy(iotdevice->id, id);

	if(description) {
		iotdevice->description = (char*)malloc(strlen(description) + 1);
		if(!iotdevice->name)
			goto fail;

		strcpy(iotdevice->description, description);
	}

	iotdevice->sensors = map_create(16, map_string_hash, map_string_equals, NULL);
	if(!iotdevice->sensors) {
		goto fail;
	}

	iotdevice->actuators = map_create(16, map_string_hash, map_string_equals, NULL);
	if(!iotdevice->actuators) {
		goto fail;
	}

	if(!map_put(iot_database, iotdevice->id, iotdevice)) {
		goto fail;
	}

	return iotdevice;
fail:
	if(iotdevice->name) {
		free(iotdevice->name);
		iotdevice->name = NULL;
	}
	if(iotdevice->id) {
		free(iotdevice->id);
		iotdevice->id = NULL;
	}
	if(iotdevice->description) {
		free(iotdevice);
		iotdevice->description = NULL;
	}
	if(iotdevice->sensors) {
		map_destroy(iotdevice->sensors);
		iotdevice->sensors = NULL;
	}
	if(iotdevice->actuators) {
		map_destroy(iotdevice->actuators);
		iotdevice->actuators = NULL;
	}
	if(iotdevice) {
		free(iotdevice);
		iotdevice = NULL;
	}
	printf("IoT Device Create Fail\n");

	return NULL;
}

bool iot_json_create(json_object *jso) {
	char name[64];
	char id[32];
	char description[128];

	json_object_object_foreach(jso, key1, child_object1) {
		if(!strcmp(key1, "name")) {
			strcpy(name, json_object_to_json_string(child_object1));
		} else if(!strcmp(key1, "id")) {
			strcpy(id, json_object_to_json_string(child_object1));
		} else if(!strcmp(key1, "description")) {
			strcpy(description, json_object_to_json_string(child_object1));
		} else {
			//printf("???\n");
		}
	}

	remove_blank(name);
	remove_blank(id);
	remove_blank(description);

	IoTDevice* iot_device = iot_create_device(name, description, id); //TODO fix here mac address
	if(!iot_device) {
		return false;
	}
	printf("\tName: %s\n", name);
	printf("\tID: %s\n", id);
	printf("\tDescription: %s\n", description);

	json_object_object_foreach(jso, key2, child_object2) {
		if(!strcmp(key2, "sensor")) {
			printf("\tSensor:\n");
			printf("\t\tName\t\tBufferSize\n");
			for(int j = 0; j < json_object_array_length(child_object2); j++) {
				json_object* sensor_object = json_object_array_get_idx(child_object2, j);
				Sensor* sensor = sensor_json_create(sensor_object);
				if(!sensor) {
					return false; //TODO garbage collect
				}
				if(!iot_add_module(iot_device, IOT_DEVICE_SENSOR, (void*)sensor)) {
					return false; //TODO garbage collect
				}
			}
		} else if(!strcmp(key2, "actuator")) {
			printf("\tActuator:\n");
			printf("\t\tName\t\tActionName\t\tActionFunc\n");
			for(int j = 0; j < json_object_array_length(child_object2); j++) {
				json_object* actuator_object = json_object_array_get_idx(child_object2, j);
				Actuator* actuator = actuator_json_create(actuator_object);
				if(!actuator) {
					return false;
				}
				if(!iot_add_module(iot_device, IOT_DEVICE_ACTUATOR, (void*)actuator)) {
					return false;
				}
			}
		}
	}

	return true;
}

bool iot_delete_device(char* name) {
	if(!name) {
		printf("iot_delete_device fail: name is null\n");
		return false;
	}
	IoTDevice* iotdevice = map_remove(iot_database, name);
	if(!iotdevice) {
		printf("iot_delete_device fail: Can't found IoT Device");
		return false;
	}


	if(iotdevice->name) {
		free(iotdevice->name);
		iotdevice->name = NULL;
	}
	if(iotdevice->description) {
		free(iotdevice->description);
		iotdevice->description = NULL;
	}
	if(iotdevice->sensors) {
		//add map iterator and free all sensor
		map_destroy(iotdevice->sensors);
		iotdevice->sensors = NULL;
	}
	if(iotdevice->actuators) {
		//add map iterator and free all actuaotr
		map_destroy(iotdevice->actuators);
		iotdevice->actuators = NULL;
	}
	if(iotdevice) {
		free(iotdevice);
		iotdevice = NULL;
	}
	printf("iot_delete_device success");
	return true;
}

bool iot_add_module(IoTDevice* iot_device, uint8_t type, void* module) {
	if(!iot_device) {
		return false;
	}
	if(!module) {
		return false;
	}

	switch(type) {
		case IOT_DEVICE_SENSOR:
			;
			Sensor* sensor = (Sensor*)module;
			if(!map_put(iot_device->sensors, sensor->name, sensor)) {
				return false;
			}
			break;
		case IOT_DEVICE_ACTUATOR:
			;
			Actuator* actuator = (Actuator*)module;
			if(!map_put(iot_device->actuators, actuator->name, actuator)) {
				return false;
			}
			break;
		default:
			return false;
	}

	return true;
}

void* iot_remove_module(char* name, uint8_t type, char* module_name) {
	if(!name) {
		return NULL;
	}
	if(!module_name) {
		return NULL;
	}

	IoTDevice* iotdevice = map_get(iot_database, name);
	if(!iotdevice) {
		return NULL;
	}

	switch(type) {
		case IOT_DEVICE_SENSOR:
			;
			Sensor* sensor = map_get(iotdevice->sensors, module_name); 
			if(!sensor) {
				return NULL;
			}
			return sensor;
		case IOT_DEVICE_ACTUATOR:
			;
			Actuator* actuator = map_get(iotdevice->actuators, module_name);
			if(!actuator) {
				return NULL;
			}
			return actuator;
		default:
			return NULL;
	}
}

bool is_alljoyn(IP* ip) {
	return false;
}

typedef struct _MQTT_VHeader {
	uint8_t		id;
	uint8_t		topic_length;
	uint8_t         topic[0];
} __attribute__ ((packed)) MQTT_VHeader;

static int counter = 0;
bool is_iot_packet(IP* ip) {
	if(ip->protocol == IP_PROTOCOL_UDP) {
		return false;
	} else if(ip->protocol == IP_PROTOCOL_TCP) {
		TCP* tcp = (TCP*)ip->body;
		if(endian16(tcp->source) == 1883) { //check ip and port iot device ip and port 1883 == mqtt broker port
			MQTT* mqtt = (MQTT*)((uint8_t*)tcp + (tcp->offset * 4));
			MQTT_VHeader* vheader= (MQTT_VHeader*)mqtt->body;
			char buf[256] = {0,};
			strncpy(buf, (char*)vheader->topic, vheader->topic_length);
			memcpy(buf, (char*)vheader->topic + vheader->topic_length, mqtt->length - sizeof(MQTT_VHeader) + vheader->topic_length);
			json_object* jso = json_tokener_parse(buf);
			if(jso) {
				//printf("\tData: %s\n", json_object_to_json_string(jso));
				IoTDevice* iot_device = NULL;
				char name[64] = {0, };
				json_object_object_foreach(jso, key, child_object) {
					if(!strcmp(key, "id_of_sensor")) {
						strcpy(name, json_object_to_json_string(child_object));
						remove_blank(name);
						iot_device = iot_get_iot_device(name);
						break;
					}
				}
				if(!iot_device)
					return false;
				//get IoTDevice;
				printf("IoT Device: %s:%d\n", iot_device->name, counter++);
				json_object_object_foreach(jso, key1, child_object1) {
					char name[64] = {0, };

					strcpy(name, key1);
					remove_blank(name);
					Sensor* sensor = iot_get_module(iot_device, IOT_DEVICE_SENSOR, name);

					if(sensor) {
						char value[64] = {0, };
						strcpy(value, json_object_to_json_string(child_object1));
						remove_blank(value);
						char* ptr;
						int64_t val = strtol(value, &ptr, 10);
						if(val) {
							printf("\t%s: %ld\n", name, val);
							sensor_data_push(sensor, val);
						}
					}
				}
				json_object_put(jso); //free
				//rule check process
			}
		}

		return false;
	}
	return false;
}

bool iot_process(Packet* packet) {
	Ether* ether = (Ether*)(packet->buffer + packet->start);
	if(endian16(ether->type) == ETHER_TYPE_IPv4) {
		IP* ip = (IP*)ether->payload;
		if(is_alljoyn(ip)) {
			nic_free(packet);
			return true;
		}
		if(is_iot_packet(ip)) {
			nic_free(packet);
			return true;
		}
	}

	nic_free(packet);
	return true;
}

IoTDevice* iot_get_iot_device(char* id) {
	return map_get(iot_database, id);
}

void* iot_get_module(IoTDevice* iot_device, uint8_t type, char* name) {
	switch(type) {
		case IOT_DEVICE_SENSOR:
			return map_get(iot_device->sensors, name);
		case IOT_DEVICE_ACTUATOR:
			return map_get(iot_device->actuators, name);
		default:
			return NULL;
	}
}
