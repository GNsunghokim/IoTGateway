#include <util/map.h>
#include <util/types.h>
#include <malloc.h>
#include <string.h>

#include <json.h>
#include <json_util.h>

#include "sensor.h"
#include "util.h"

Sensor* sensor_create(char* name, uint64_t size) {
	Sensor* sensor = (Sensor*)malloc(sizeof(Sensor));
	if(!sensor)
		return NULL;

	memset(sensor, 0 ,sizeof(Sensor));
	sensor->name = (char*)malloc(strlen(name) + 1);
	if(!sensor->name)
		goto fail;

	strcpy(sensor->name, name);

	sensor->datas = (int64_t*)malloc(sizeof(int64_t) * size);
	if(!sensor->datas)
		goto fail;
	memset(sensor->datas, 0 ,sizeof(int64_t) * size);

	sensor->size = size;
	sensor->count = 0;

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

	remove_blank(name);
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

bool sensor_data_push(Sensor* sensor, int64_t data) {
	sensor->datas[sensor->count % sensor->size] = data;
	sensor->count++;

	return true;
}

int64_t get_newest(Sensor* sensor) {
	if(!sensor) {
		printf("Error\n");
		return 0;
	}
	
	if(sensor->count == 0) {
		return 0;
	} else {
		return sensor->datas[(sensor->count - 1) % sensor->size];
	}
}

int64_t get_avg(Sensor* sensor) {
	int64_t sum;

	if(!sensor) {
		printf("Error\n");
		return 0;
	}

	if(sensor->count < sensor->size) {
		for(int i = 0; i < sensor->count; i++) {
			sum += sensor->datas[i];
		}

		return sum / sensor->count;
	} else {
		for(int i = 0; i < sensor->size; i++) {
			sum += sensor->datas[i];
		}

		return sum / sensor->size;
	}

	return 0;
}

int64_t get_max(Sensor* sensor) {
	if(!sensor) {
		printf("Error\n");
		return 0;
	}

	int64_t max = 0;

	if(sensor->count < sensor->size) {
		for(int i = 0; i < sensor->count; i++) {
			if(max < sensor->datas[i])
				max = sensor->datas[i];
		}
	} else {
		for(int i = 0; i < sensor->size; i++) {
			if(max < sensor->datas[i])
				max = sensor->datas[i];
		}
	}

	return max;
}
