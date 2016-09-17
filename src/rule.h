#include <util/types.h>

#include <json.h>
#include <json_util.h>

#include "sensor.h"
/*
   IoT Gateway Prototype not support various type rule
#define CONDITION_TYPE_NONE	0X0
#define CONDITION_TYPE_NORMAL	0X1
#define CONDITION_TYPE_URGENT	0X2
*/

#define OBJECT_TYPE_INT64	1
#define OBJECT_TYPE_SENSOR	2

typedef struct _SensorObject {
	Sensor* sensor;
	int64_t (*func)(Sensor* sensor);
} SensorObject;

typedef struct _Object {
	uint8_t type;
	union {
		int64_t value;
		SensorObject sensor_obj;
	};
} Object;

typedef struct _Rule {
	char* name;
	char* func;
	void* action;
	char* description;

	Object* left_object;
	Object* right_object;
	bool (*compare)(int64_t l_value, int64_t r_value);
} Rule;

bool rule_create(char* name, char* func, char* action, char* description);
bool rule_json_create(json_object* jso);
