#ifndef __ACTUATOR_H__
#define __ACTUATOR_H__

#include <util/types.h>
#include <util/map.h>

#include <json.h>
#include <json_util.h>

typedef struct _Action {
	char* name;
	char* func;
} Action;

typedef struct _Actuator {
	char* name;
	Map* actions;
} Actuator;

Action* action_create(char* name, char* func);
Actuator* actuator_json_create(json_object* jso);
void action_delete(Action* action);
Actuator* actuator_create(char* name);
bool actuator_delete(Actuator* actuator);
bool actuator_add_action(Actuator* actuator, Action* action);
Action* actuator_remove_action(Actuator* actuator, char* name);
#endif
