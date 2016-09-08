#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <util/map.h>
#include <util/types.h>
#include <net/packet.h>
#include <net/ether.h>
#include <net/arp.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/checksum.h>
#include <net/udp.h>

#include "iot.h"
#include "actuator.h"
#include "sensor.h"

#include <json.h>
#include <json_util.h>

static Map* iot_database;

bool iot_init() {
	iot_database = map_create(32, map_string_hash, map_string_equals, NULL);
	if(!iot_database)
		return false;


	return true;
}

IoTDevice* iot_create_device(char* name, char* description, uint64_t mac) {
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

	iotdevice->mac = mac;

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

	if(!map_put(iot_database, name, iotdevice)) {
		goto fail;
	}

	return iotdevice;
fail:
	if(iotdevice->name) {
		free(iotdevice->name);
		iotdevice->name = NULL;
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
	char mac[32];
	char description[128];


	json_object_object_foreach(jso, key1, child_object1) {
		if(!strcmp(key1, "name")) {
			strcpy(name, json_object_to_json_string(child_object1));
			printf("iot-device: %s\n", name);
		} else if(!strcmp(key1, "mac")) {
			strcpy(mac, json_object_to_json_string(child_object1));
			printf("\t%s: %s\n", key1, mac);
		} else if(!strcmp(key1, "description")) {
			strcpy(description, json_object_to_json_string(child_object1));
			printf("\t%s: %s\n", key1, description);
		} else {
			//printf("???\n");
		}
	}

	IoTDevice* iot_device = iot_create_device(name, description, 0); //TODO fix here mac address
	if(!iot_device) {
		printf("Can't Create iot_device\n");
		return false;
	}

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
			Actuator* actuator = map_get(iotdevice->sensors, module_name);
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

bool is_iot_packet(IP* ip) {
	return true;
}

bool iot_process(Packet* packet) {
	Ether* ether = (Ether*)(packet->buffer + packet->start);
	IP* ip = (IP*)ether->payload;
	if(is_alljoyn(ip)) {
		//alljoyn process
		return true;
	}
	if(is_iot_packet(ip)) {
		//noral protocol
		return true;
	} else {
		nic_free(packet);
		return true;
	}

	return false;
}
