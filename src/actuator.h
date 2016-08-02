#include <util/types.h>
#include <util/map.h>

typedef struct _Action {
	char* name;
	bool (*func)(void* data);
} Action;

typedef struct _Actuator {
	char* name;
	Map* actions;
} Actuator;

Action* action_create(char* name, bool (*func)(void*));
void action_delete(Action* action);
Actuator* actuator_create(char* name);
bool actuator_delete(Actuator* actuator);
bool actuator_add_sensor(Actuator* actuator, Action* action);
Action* actuator_remove_sensor(Actuator* actuator, char* name);
