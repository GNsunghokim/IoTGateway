#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <util/map.h>
#include <util/types.h>

#include <json.h>
#include <json_util.h>

#include "actuator.h"

Action* action_create(char* name, char* func) {
	if(name == NULL || func == NULL)
		return NULL;

	Action* action = (Action*)malloc(sizeof(Action));
	if(!action)
		return NULL; 

	memset(action, 0 ,sizeof(Action));
	action->name = (char*)malloc(strlen(name) + 1);
	if(!action->name)
		goto fail;

	strcpy(action->name, name);

	action->func = (char*)malloc(strlen(func) + 1);
	if(!action->func)
		goto fail;

	strcpy(action->func, func);

	return action;

fail:
	if(action->name) {
		free(action->name);
		action->name = NULL;
	}

	if(action) {
		free(action);
		action = NULL;
	}

	return NULL;
}

void action_delete(Action* action) {
	if(!action)
		return;

	if(action->name) {
		free(action->name);
		action->name = NULL;
	}

	if(action) {
		free(action);
		action = NULL;
	}
}

Actuator* actuator_create(char* name) {
	if(!name) {
		printf("actuator_create fail: name is null\n");
		return NULL;
	}

	Actuator* actuator = (Actuator*)malloc(sizeof(Actuator));
	if(!actuator)
		return NULL;

	memset(actuator, 0 ,sizeof(Actuator));
	actuator->name = (char*)malloc(strlen(name) + 1);
	if(!actuator->name)
		goto fail;

	actuator->actions = map_create(16, map_string_hash, map_string_equals, NULL);
	if(!actuator->actions)
		goto fail;

	strcpy(actuator->name, name);

	return actuator;

fail:
	if(actuator->actions) {
		map_destroy(actuator->actions);
		actuator->actions = NULL;
	}
	if(actuator->name) {
		free(actuator->name);
		actuator->name = NULL;
	}

	if(actuator) {
		free(actuator);
		actuator = NULL;
	}

	return NULL;
} 

Action* action_json_create(json_object* jso) {
	char name[64];
	char func[64];

	json_object_object_foreach(jso, key, child_object) {
		if(!strcmp(key, "name")) {
			strcpy(name, json_object_to_json_string(child_object));
		} else if(!strcmp(key, "func")) {
			strcpy(func, json_object_to_json_string(child_object));
		}
	}
	printf("\t\t\t\t%s\t\t%s\n", name, func);

	return action_create(name, func);
}

Actuator* actuator_json_create(json_object* jso) {
	Actuator* actuator = NULL;
	char name[64];
	json_object_object_foreach(jso, key1, child_object1) { 
		if(!strcmp(key1, "name")) {
			strcpy(name, json_object_to_json_string(child_object1));
		} else {
			//printf("???\n");
		}
	}
	actuator = actuator_create(name);
	if(!actuator) {
		return NULL;
	}

	printf("\t\t%s\n", name);

	json_object_object_foreach(jso, key2, child_object2) { 
		if(!strcmp(key2, "action")) {
			for(int j = 0; j < json_object_array_length(child_object2); j++) {
				json_object* action_object = json_object_array_get_idx(child_object2, j);
				Action* action = action_json_create(action_object);
				if(!action) {
					printf("Erronr\n"); //TODO Add garbage collection
					return NULL;
				}
				if(actuator) {
					actuator_add_action(actuator, action);
				} else {
					action_delete(action);
					printf("Error\n");
					return NULL;
				}
			}
		} else {
			//printf("???\n");
		}
	}

	return actuator;
}

bool actuator_delete(Actuator* actuator) {
	if(!actuator) {
		printf("actuator_delete fail: actuator is null\n");
		return false;
	}

	if(actuator->actions) {
		//add map iterator
		map_destroy(actuator->actions);
		actuator->actions = NULL;
	}
	if(actuator->name) {
		free(actuator->name);
		actuator->name = NULL;
	}

	if(actuator) {
		free(actuator);
		actuator = NULL;
	}

	return true;
}

bool actuator_add_action(Actuator* actuator, Action* action) {
	if(!actuator) {
		printf("actuator_add_sensor fail: actuator is null\n");
		return false;
	}
	if(!action) {
		printf("actuator_add_sensor fail: action is null\n");
		return false;
	}
	if(!map_put(actuator->actions, action->name, action)) {
		printf("actuator_add_sensor fail: Can't add action\n");
		return false;
	}

	return true;
}

Action* actuator_remove_action(Actuator* actuator, char* name) {
	if(!actuator) {
		printf("actuator_remove_sensor fail: actuator is null\n");
		return NULL;
	}

	if(!name) {
		printf("actuator_remove_sensor fail: name is null\n");
		return NULL;
	}

	Action* action = map_get(actuator->actions, name);
	if(!action) {
		printf("actuator_remove_sensor fail: Can't found action\n");
		return NULL;
	}

	return action;
}
