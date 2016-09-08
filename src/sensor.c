#include <util/map.h>
#include <util/types.h>
#include <malloc.h>
#include <string.h>

#include <json.h>
#include <json_util.h>

#include "sensor.h"

Sensor* sensor_create(char* name, uint64_t size) {
	Sensor* sensor = (Sensor*)malloc(sizeof(Sensor));
	if(!sensor)
		return NULL;

	memset(sensor, 0 ,sizeof(Sensor));
	sensor->name = (char*)malloc(strlen(name) + 1);
	if(!sensor->name)
		goto fail;

	strcpy(sensor->name, name);

	sensor->datas = (uint64_t*)malloc(sizeof(uint64_t) * size);
	if(!sensor->datas)
		goto fail;

	sensor->size = size;
	memset(sensor->datas, 0 ,sizeof(uint64_t) * size);

	return sensor;

fail:
	if(sensor->datas) {
		free(sensor->datas);
		sensor->datas = NULL;
	}

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

Sensor* sensor_json_create(json_object* jso) {
	char name[64];
	uint64_t size = 10;

	json_object_object_foreach(jso, key, child_object) { 
		if(!strcmp(key, "name")) {
			strcpy(name, json_object_to_json_string(child_object));
		} else if(!strcmp(key, "buffer-size")) {
			size = json_object_get_int64(child_object);
		} else {
			printf("???\n");
		}
	}

	printf("\t\t%s\t%ld\n", name, size);
	return sensor_create(name, size);
}

bool sensor_delete(Sensor* sensor) {
	if(!sensor) {
		printf("sensor is null\n");
		return false;
	}	
	if(sensor->datas) {
		free(sensor->datas);
		sensor->datas = NULL;
	}

	if(sensor->name) {
		free(sensor->name);
		sensor->name = NULL;
	}

	if(sensor) {
		free(sensor);
		sensor = NULL;
	}

	return true;
}
