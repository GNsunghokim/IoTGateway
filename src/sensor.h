#include <util/types.h>

#include <json.h>
#include <json_util.h>

typedef struct _Sensor {
	char* name;
	uint64_t* datas;
	uint64_t size;
	uint32_t index;
} Sensor;

Sensor* sensor_create(char* name, uint64_t size);
bool sensor_delete(Sensor* sensor);
Sensor* sensor_json_create(json_object* jso);
