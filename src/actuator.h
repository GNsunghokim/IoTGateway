#ifndef __ACTUATOR_H__
#define __ACTUATOR_H__

#include <util/types.h>
#include <util/map.h>

#include <json.h>
#include <json_util.h>

#include <action.h>

typedef struct _Actuator {
	char* name;
	char* id;
	char* address;
	char* description;
	Map* actions;
} Actuator;

bool actuator_database_init(void);
void actuator_database_destroy(void);
bool actuator_database_add(Actuator* actuator);
Actuator* actuator_database_remove(char* name);
Actuator* actuator_database_get(char* name);

Actuator* actuator_json_create(json_object* jso);
bool actuator_delete(Actuator* actuator);
Actuator* actuator_create(char* name, char* address, char* description);

bool actuator_add_action(Actuator* actuator, Action* action);
Action* actuator_get_action(Actuator* actuator, char* action);
Action* actuator_remove_action(Actuator* actuator, char* name);

#endif /*__ACTUATOR_H__*/
