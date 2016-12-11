#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <string.h>
#include <util/map.h>
#include <util/types.h>

#include <actuator.h>
#include <util.h>

static Map* actuator_database;
bool actuator_database_init(void) {
	actuator_database = map_create(16, map_string_hash, map_string_equals, NULL);
	if(!actuator_database)
		return false;

	return true;
}

void actuator_database_destroy(void) {
	map_destroy(actuator_database);
}

bool actuator_database_add(Actuator* actuator) {
	return map_put(actuator_database, actuator->name, actuator);
}

Actuator* actuator_database_remove(char* name) {
	return map_remove(actuator_database, name);
}

Actuator* actuator_database_get(char* name) {
	return map_get(actuator_database, name);
}

Actuator* actuator_create(char* name, char* address, char* description) {
	if(!strlen(name) || !strlen(address)) {
		return NULL;
	}

	Actuator* actuator = (Actuator*)malloc(sizeof(Actuator));
	if(!actuator)
		return NULL;
	memset(actuator, 0, sizeof(Actuator));

	actuator->name = (char*)malloc(strlen(name) + 1);
	if(!actuator->name)
		goto fail;
	strcpy(actuator->name, name);

	actuator->address = (char*)malloc(strlen(address) + 1);
	if(!actuator->address)
		goto fail;
	strcpy(actuator->address, address);

	actuator->description = (char*)malloc(strlen(description) + 1);
	if(!actuator->description)
		goto fail;
	strcpy(actuator->description, description);

	actuator->actions = map_create(16, map_string_hash, map_string_equals, NULL);
	if(!actuator->actions)
		goto fail;

	return actuator;

fail:
	if(actuator->actions) {
		map_destroy(actuator->actions);
		actuator->actions = NULL;
	}

	if(actuator->description) {
		free(actuator->description);
		actuator->description = NULL;
	}

	if(actuator->address) {
		free(actuator->address);
		actuator->address = NULL;
	}

	if(actuator->id) {
		free(actuator->id);
		actuator->id = NULL;
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

Actuator* actuator_json_create(json_object* jso) {
	char name[64] = {0, };
	char address[64] = {0, };
	char description[64] = {0, };

	json_object_object_foreach(jso, key1, child_object1) { 
		if(!strcmp(key1, "name")) {
			strcpy(name, json_object_to_json_string(child_object1));
			remove_blank(name);
		} else if(!strcmp(key1, "address")) {
			strcpy(address, json_object_to_json_string(child_object1));
			remove_blank(address);
		} else if(!strcmp(key1, "description")) {
			strcpy(description, json_object_to_json_string(child_object1));
			remove_blank(description);
		}
	}

	Actuator* actuator = actuator_create(name, address, description);
	if(actuator) {
		json_object_object_foreach(jso, key2, child_object2) { 
			if(!strcmp(key2, "actions")) {
				for(int j = 0; j < json_object_array_length(child_object2); j++) {
					json_object* action_object = json_object_array_get_idx(child_object2, j);
					Action* action = action_json_create(action_object);
					if(!action) {
						printf("Action create fail\n");
						goto fail;
					}
					if(!actuator_add_action(actuator, action)) {
						printf("Action add fail\n");
						goto fail;
					}
				}
			}
		}
	} else {
		printf("Actuator create fail\n");
	}

	return actuator;

fail:
	actuator_delete(actuator);

	return NULL;
}

bool actuator_delete(Actuator* actuator) {
	MapIterator iter;
	map_iterator_init(&iter, actuator->actions);
	while(map_iterator_has_next(&iter)) {
		MapEntry* entry = map_iterator_remove(&iter);
		action_delete(entry->data);
	}
	map_destroy(actuator->actions);
	actuator->actions = NULL; 
	free(actuator->description);
	free(actuator->address);
	free(actuator->id);
	free(actuator->name);
	free(actuator);

	return true;
}

bool actuator_add_action(Actuator* actuator, Action* action) {
	return map_put(actuator->actions, action->name, action);
}

Action* actuator_get_action(Actuator* actuator, char* action) {
	return map_get(actuator->actions, action);
}

Action* actuator_remove_action(Actuator* actuator, char* name) {
	return map_remove(actuator->actions, name);
}

