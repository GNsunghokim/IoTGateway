#ifndef __SENSOR_H__
#define __SENSOR_H__

#include <util/types.h>

#include <json.h>
#include <json_util.h>

typedef struct _Sensor {
	char* name;
	uint64_t* datas;
	uint64_t size;
	uint64_t count;
} Sensor;

Sensor* sensor_create(char* name, uint64_t size);
bool sensor_delete(Sensor* sensor);
Sensor* sensor_json_create(json_object* jso);
bool sensor_data_push(Sensor* sensor, uint64_t data);

int64_t get_newest(Sensor* sensor);
int64_t get_avg(Sensor* sensor);
int64_t get_max(Sensor* sensor);

#endif
