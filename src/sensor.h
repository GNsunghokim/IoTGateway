#ifndef __SENSOR_H__
#define __SENSOR_H__

#include <stdint.h>

#include <json.h>
#include <json_util.h>

#include <util/types.h>

#include <data.h>

typedef struct _Sensor {
	char* name;
	char* description;
	char* type;
	Map* data_map;
} Sensor;

bool sensor_database_init();
void sensor_database_destroy();
bool sensor_database_add(Sensor* sensor);
Sensor* sensor_database_remove(char* name);
Sensor* sensor_database_get(char* name);

Sensor* sensor_create(char* name, char* type, char* description);
Sensor* sensor_json_create(json_object* jso);
bool sensor_delete(Sensor* sensor);

bool sensor_add_data(Sensor* sensor, Data* data);
Data* sensor_remove_data(Sensor* sensor, char* data_name);
Data* sensor_get_data(Sensor* sensor, char* data_name);
#endif
