#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <util/map.h>
#include <util/types.h>

static Map* iot_database;

bool iot_init() {
	iot_database = map_create(32, map_string_hash, map_string_equals, NULL);
	if(!iots)
		return false;


	return true;
}

bool iot_create_device(char* name, char* description, uint32_t ip) {
	if(!name) {
		printf("iot_create_device fail: name is null\n");
		return false;
	}
	if(map_contains(iot_database, name)) {
		printf("iot_create_device fail: already existing name\n");
		return false;
	}

	IoTDevice* iotdevice = (IoTDevice*)malloc(sizeof(IoTDevice));
	if(!iotdevice) {
		printf("iot_create_device fail: malloc fail\n");
		return false;
	}

	memset(iotdevice, 0, sizeof(IoTDevice));

	iotdevice->name = (char*)malloc(strlen(name) + 1);
	if(!iotdevice->name)
		goto fail;

	strcpy(iotdevice->name, name);

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

	iotdevice->ip = ip;

	if(!map_put(iot_database, name, iotdevice)) {
		goto fail;
	}

	return true;
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

	return false;
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

bool iot_add_module(char* name, uint8_t type, void* module) {
	if(!name) {
		return false;
	}
	if(!module) {
		return false;
	}

	IoTDevice* iotdevice = map_get(iot_database, name);
	if(!iotdevice) {
		return false;
	}

	switch(type) {
		case IOT_DEVICE_SENSOR:
			;
			Sensor* sensor = (Sensor*)module;
			if(!map_put(iotdevice->sensors, sensor->name, sensor)) {
				return false;
			}
			break;
		case IOT_DEVICE_ACTUATOR:
			;
			Actuator* actuator = (Actuator*)module;
			if(!map_put(iotdevice->actuators, actuator->name, actuator)) {
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

bool iot_process(Packet* packet) {
	Ether* ether = (Ether*)((*packet)->buffer + (*packet)->start);
	IP* ip = (IP*)ether->payload;
	if(is_alljoyn(ip)) {
		//alljoyn process

		return true;
	}
	if(is_iot_packet(ip)) {
		//noral protocol

		return true;
	} else {
		ni_free(packet);
		return true;
	}

	return false;
}
