#ifndef __RULE_H__
#define __RULE_H__

#include <util/types.h>

#include <json.h>
#include <json_util.h>

#include "sensor.h"
#include "actuator.h"
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
	Data* data;
	int64_t (*func)(Data* data);
} SensorObject;

typedef struct _Object {
	uint8_t type;
	union {
		int64_t value;
		SensorObject sensor_obj;
	};
} Object;

typedef struct _RuleAction {
	Actuator* actuator;
	Action* action;
} RuleAction;

typedef struct _Rule {
	char* name;
	char* func;
	char* description;

	Object* left_object;
	Object* right_object;
	RuleAction* action;
	bool (*compare)(int64_t l_value, int64_t r_value);
} Rule;

bool rule_database_init();
void rule_database_destroy();
bool rule_database_add(Rule* rule);
Rule* rule_database_remove(char* name);

Rule* rule_create(char* name, char* func, char* action, char* description);
Rule* rule_json_create(json_object* jso);
void rule_process();

#endif
