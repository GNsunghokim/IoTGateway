#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <util/map.h>
#include <util/types.h>

static Map* iots;

bool iot_init() {
	iots = map_create(32, map_string_hash, map_string_equals, NULL);
	if(!iots)
		return false;


	return true;
}

static Sensor* sensor_create() {
}

static Sensor* sensor_create() {
	char* name = NULL;

	Sensor* sensor = (Sensor*)malloc(sizeof(Sensor));
	if(!sensor)
		return NULL;

	memset(sensor, 0 ,sizeof(Sensor));
	sensor->name = (char*)malloc(strlen(name) + 1);
	if(!sensor->name)
		goto fail;

	strcpy(sensor->name, name);
	return sensor;

fail:
	if(sensor->name) {
		free(sensor->name);
		sensor->name = NULL;
	}

	if(sensor) {
		free(sensor);
		sensor = NULL;
	}

	return NULL;
} 

static Action* action_create() {
	char* name = NULL;

	Action* action = (Action*)malloc(sizeof(Action));
	if(!action)
		return NULL;


	memset(action, 0 ,sizeof(Action));
	action->name = (char*)malloc(strlen(name) + 1);
	if(!action->name)
		goto fail;

	action->actions = map_create(16, map_string_hash, map_string_equals, NULL);
	if(!action->actions)
		goto fail;

	strcpy(action->name, name);
	return action;

fail:
	if(action->actions) {
		map_destroy(action->actions);
		action->actions = NULL;
	}
	if(action->name) {
		free(action->name);
		action->name = NULL;
	}

	if(action) {
		free(action);
		action = NULL;
	}

	return NULL;
}

static Actuator* actuator_create() {
	char* name = NULL;

	Actuator* actuator = (Actuator*)malloc(sizeof(Actuator));
	if(!actuator)
		return NULL;


	memset(actuator, 0 ,sizeof(Actuator));
	actuator->name = (char*)malloc(strlen(name) + 1);
	if(!actuator->name)
		goto fail;

	actuator->actions = map_create(16, map_string_hash, map_string_equals, NULL);
	if(!actuator->actions)
		goto fail;

	strcpy(actuator->name, name);
	return actuator;

fail:
	if(actuator->actions) {
		map_destroy(actuator->actions);
		actuator->actions = NULL;
	}
	if(actuator->name) {
		free(actuator->name);
		actuator->name = NULL;
	}

	if(actuator) {
		free(actuator);
		actuator = NULL;
	}

	return NULL;
} 

bool iot_create_device() {
	char* name = NULL, description = NULL;
	uint32_t ip = 0;

	IoTDevice* iotdevice = (IoTDevice*)malloc(sizeof(IoTDevice));
	if(!iotdevice)
		return false;

	memset(iotdevice, 0, sizeof(IoTDevice));

	iotdevice->name = (char*)malloc(strlen(name) + 1);
	if(!iotdevice->name) {
		goto fail;
	}

	iotdevice->description = (char*)malloc(strlen(description) + 1);
	if(!iotdevice->name) {
		goto fail;
	}
	iotdevice->sensors = map_create(16, map_string_hash, map_string_equals, NULL);
	if(!iotdevice->sensors) {
		goto fail;
	}

	iotdevice->actuators = map_create(16, map_string_hash, map_string_equals, NULL);
	if(!iotdevice->actuators) {
		goto fail;
	}

	strcpy(iotdevice->name, name);
	strcpy(iotdevice->description, description);
	iotdevice->ip = ip;

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
