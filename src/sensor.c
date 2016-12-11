#include <util/map.h>
#include <util/types.h>
#include <malloc.h>
#include <string.h>

#include <json.h>
#include <json_util.h>

#include "sensor.h"
#include "util.h"

static Map* sensor_database;
bool sensor_database_init() {
	sensor_database = map_create(16, map_string_hash, map_string_equals, NULL);
	if(!sensor_database)
		return false;

	return true;
}

void sensor_database_destroy() {
	map_destroy(sensor_database);
}

bool sensor_database_add(Sensor* sensor) {
	return map_put(sensor_database, sensor->name, sensor);
}

Sensor* sensor_database_remove(char* name) {
	return map_remove(sensor_database, name);
}

Sensor* sensor_database_get(char* name) {
	return map_get(sensor_database, name);
}

Sensor* sensor_create(char* name, char* type, char* description) {
	if(!strlen(name) || !strlen(type))
		return NULL;

	Sensor* sensor = (Sensor*)malloc(sizeof(Sensor));
	if(!sensor)
		return NULL;

	memset(sensor, 0 ,sizeof(Sensor));
	sensor->name = (char*)malloc(strlen(name) + 1);
	if(!sensor->name)
		goto fail;
	strcpy(sensor->name, name);

	sensor->type = (char*)malloc(strlen(type) + 1);
	if(!sensor->type)
		goto fail;
	strcpy(sensor->type, type);

	sensor->description = (char*)malloc(strlen(description) + 1);
	if(!sensor->description)
		goto fail;
	strcpy(sensor->description, description);

	sensor->data_map = map_create(16, map_string_hash, map_string_equals, NULL);
	if(!sensor->data_map)
		goto fail;

	return sensor;

fail:
	if(sensor->data_map)
		map_destroy(sensor->data_map);

	if(sensor->name) {
		free(sensor->name);
	}

	if(sensor->description) {
		free(sensor->description);
	}

	if(sensor->type) {
		free(sensor->type);
	}

	free(sensor);

	return NULL;
} 

Sensor* sensor_json_create(json_object* jso) {
	char name[64] = {0, };
	char type[64] = {0, };
	char description[128] = {0, };

	//TODO add new value
	json_object_object_foreach(jso, key1, child_object) { 
		if(!strcmp(key1, "name")) {
			strcpy(name, json_object_to_json_string(child_object));
			remove_blank(name);
		} else if(!strcmp(key1, "type")) {
			strcpy(type, json_object_to_json_string(child_object));
			remove_blank(type);
		} else if(!strcmp(key1, "description")) {
			strcpy(description, json_object_to_json_string(child_object));
		}
	}

	Sensor* sensor = sensor_create(name, type, description);

	if(sensor) {
		json_object_object_foreach(jso, key2, child_object2) { 
			if(!strcmp(key2, "datas")) {
				for(int j = 0; j < json_object_array_length(child_object2); j++) {
					json_object* action_object = json_object_array_get_idx(child_object2, j);
					Data* data = data_json_create(action_object);
					if(!data) {
						printf("Data create fail\n");
						goto fail;
					}
					if(!sensor_add_data(sensor, data)) {
						printf("Data add fail\n");
						data_delete(data);
						goto fail;
					}
				}
			}
		}
		printf("\t\t%s\n", name);
	} else {
		printf("Sensor create fail\n");
	}

	return sensor;
fail:
	sensor_delete(sensor);
	return NULL;
}

bool sensor_delete(Sensor* sensor) {
	//TODO Map flush
	MapIterator iter;
	map_iterator_init(&iter, sensor->data_map);
	while(map_iterator_has_next(&iter)) {
		MapEntry* entry = map_iterator_remove(&iter);
		Data* data = entry->data;
		data_delete(data);
	}
	map_destroy(sensor->data_map);
	free(sensor->name);
	free(sensor);

	return true;
}

bool sensor_add_data(Sensor* sensor, Data* data) {
	return map_put(sensor->data_map, data->name, data);
}

Data* sensor_remove_data(Sensor* sensor, char* data_name) {
	return map_remove(sensor->data_map, data_name);
}

Data* sensor_get_data(Sensor* sensor, char* data_name) {
	return map_get(sensor->data_map, data_name);
}	
