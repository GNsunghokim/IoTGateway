#ifndef __ACTION_H__
#define __ACTION_H__

#include <stdbool.h>
#include <json.h>
#include <json_util.h>

typedef struct _Action {
	char* type;
	char* name;
	char* function;
	bool (*action)(void* data);
} Action;

Action* action_create(char* type, char* name, char* function);
Action* action_json_create(json_object* jso);
void action_delete(Action* action);

#endif /*__Action_H__*/
