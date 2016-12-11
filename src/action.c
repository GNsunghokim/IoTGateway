#include <malloc.h>
#include <string.h>

#include <action.h>
#include <util.h>
#include <lifx.h>
#include <print.h>

static void* get_action(char* type, char* function) {
	if(!strcmp(type, "lifx")) {
		return lifx_get_action(function);
	} else if(!strcmp(type, "print")) {
		return print_get_action(function);
	}

	return NULL;
}

Action* action_create(char* type, char* name, char* function) {
	if(!strlen(type) || !strlen(name) || !strlen(function))
		return NULL;

	Action* action = (Action*)malloc(sizeof(Action));
	if(!action)
		return NULL; 

	memset(action, 0 ,sizeof(Action));

	action->type = (char*)malloc(strlen(type) + 1);
	if(!action->type)
		goto fail;
	strcpy(action->type, type);

	action->name = (char*)malloc(strlen(name) + 1);
	if(!action->name)
		goto fail;
	strcpy(action->name, name);

	action->function = (char*)malloc(strlen(function) + 1);
	if(!action->function)
		goto fail;
	strcpy(action->function, function);

	action->action = get_action(type, function);
	if(!action->action)
		goto fail;

	return action;

fail:
	if(action->type) {
		free(action->type);
		action->type = NULL;
	}

	if(action->name) {
		free(action->name);
		action->name = NULL;
	}

	if(action->function) {
		free(action->function);
		action->function = NULL;
	}

	free(action);

	return NULL;
}

Action* action_json_create(json_object* jso) {
	char type[64] = {0, };
	char name[64] = {0, };
	char function[64] = {0, };

	json_object_object_foreach(jso, key, child_object) {
		if(!strcmp(key, "type")) {
			strcpy(type, json_object_to_json_string(child_object));
			remove_blank(type);
		} else if(!strcmp(key, "name")) {
			strcpy(name, json_object_to_json_string(child_object));
			remove_blank(name);
		} else if(!strcmp(key, "function")) {
			strcpy(function, json_object_to_json_string(child_object));
			remove_blank(function);
		}
	}

	Action* action = action_create(type, name, function);
	if(action)
		printf("\t\t\t\t%s\t\t%s\t\t%s\n", type, name, function);

	return action;
}

void action_delete(Action* action) {
	free(action->type);
	free(action->name);
	free(action->function);
	free(action);
}
