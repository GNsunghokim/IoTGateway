#include <stdbool.h>
#include <malloc.h>
#include <string.h>
#include <util/types.h>
#include <util/map.h>

#include <json.h>
#include <json_util.h>
#include <errno.h>

#include "rule.h"
#include "sensor.h"
#include "actuator.h"
#include "util.h"

static Map* rule_database;

/* Function */
static int64_t is_left_large(int64_t left, int64_t right) {
	return left > right;
}

static int64_t is_right_large(int64_t left, int64_t right) {
	return left < right;
}

static int64_t is_equal(int64_t left, int64_t right) {
	return left == right;
}

static int64_t is_not_equal(int64_t left, int64_t right) {
	return left != right;
}

typedef struct _Func {
	char* name;
	int64_t (*func)(int64_t left, int64_t ring);
} Func;

Func funcs[] = {
	{
		.name = ">",
		.func = is_left_large
	},
	{
		.name = "<",
		.func = is_right_large
	},
	{
		.name = "==",
		.func = is_equal
	},
	{
		.name = "!=",
		.func = is_not_equal
	}
};

static void* get_func_ptr(char* func) {
	int num = sizeof(funcs) / sizeof(Func);

	for(int i = 0; i < num; i++) {
		if(!strcmp(funcs[i].name, func)) {
			return funcs[i].func;
		}
	}

	return NULL;
}

static Object* create_object(char* str) {
	Object* object = malloc(sizeof(Object));
	memset(object, 0, sizeof(Object));

	char * endptr;
	long val = strtol(str, &endptr, 10);
	if(str != endptr) {
		object->type = OBJECT_TYPE_INT64;
		object->value = val;
	} else {
		char buf[64];
		char* p;

		object->type = OBJECT_TYPE_SENSOR;
		strcpy(buf, str);
		p = strtok(buf, "->");
		if(!p) {
			printf("Can't parse\n");
			goto fail;
		}
		Sensor* sensor = sensor_database_get(p);
		if(!sensor) {
			printf("Can't get sensor: %s\n", p);
			goto fail;
		}
		object->sensor_obj.sensor = sensor;
		p = strtok(p + strlen(p) + 1, "->");
		if(!p) {
			printf("Can't parse\n");
			goto fail;
		}
		Data* data = sensor_get_data(sensor, p);
		if(!data) {
			printf("Can't get data: %s\n", p);
			goto fail;
		}
		object->sensor_obj.data = data;
		p = strtok(p + strlen(p) + 1, "->");
		if(!p) {
			printf("Can't parse\n");
			goto fail;
		}

		if(!strcmp(p, "newest")) {
			object->sensor_obj.func = data_get_newest;
			printf("newest\n");
		} else if(!strcmp(p, "avg")) {
			object->sensor_obj.func = data_get_avg;
			printf("avg\n");
		} else if(!strcmp(p, "max")) {
			object->sensor_obj.func = data_get_max;
			printf("max\n");
		} else {
			printf("Can't get Func: %s\n", p);
			goto fail;
		}	
	}

	return object;

fail:
	if(object)
		free(object);

	return NULL;
}

bool rule_database_init() {
	rule_database = map_create(16, map_string_hash, map_string_equals, NULL);
	if(!rule_database)
		return false;

	return true;
}

void rule_database_destroy() {
	map_destroy(rule_database);
}

bool rule_database_add(Rule* rule) {
	return map_put(rule_database, rule->name, rule);
}

Rule* rule_database_remove(char* name) {
	return map_remove(rule_database, name);
}

static bool rule_func_check(char* func) {
	return true;
}

static RuleAction* rule_action_create(char* action) {
	//TODO fix here
	RuleAction* rule_action = malloc(sizeof(RuleAction));
	char* p;
	char buf[64];

	if(!rule_action) {
		printf("Can't allocate Rule Action\n");
		return NULL;
	}
	memset(rule_action, 0, sizeof(RuleAction));

	/**
	   * rule action "iot_device_name.action"
	 **/
	strcpy(buf, action);
	p = strtok(buf, "->");
	if(!p) {
		printf("Can't parse\n");
		goto fail;
	}
	rule_action->actuator = actuator_database_get(p);
	if(!rule_action->actuator) {
		printf("Can't get actuator: %s\n", p);
		goto fail;
	}

	p = strtok(p + strlen(p) + 1, "->");
	if(!p) {
		printf("Can't parse\n");
		goto fail;
	}
	rule_action->action = actuator_get_action(rule_action->actuator, p);
	if(!rule_action->action) {
		printf("Can't get action: %s\n", p);
		goto fail;
	}

	return rule_action;

fail:
	if(rule_action) {
		free(rule_action);
	}
	
	return NULL;
}

Rule* rule_create(char* name, char* func, char* action, char* description) {
	if(!name)
		return false;

	if(!rule_func_check(func)) {
		printf("rule_create fail: rule is wrong\n");
		return false;
	}

	Rule* rule = malloc(sizeof(Rule));
	if(!rule) {
		printf("rule_create fail: can't allocate rule\n");
		return false;
	}
	memset(rule, 0, sizeof(Rule));
	rule->name = malloc(strlen(name) + 1);
	if(!rule->name) {
		printf("rule_create fail: can't allocate rule name\n");
		goto fail;
	}
	strcpy(rule->name, name);

	rule->func = malloc(strlen(func) + 1);
	if(!rule->func) {
		printf("rule_create fail: can't allocate rule func\n");
		goto fail;
	}
	strcpy(rule->func, func);

	char buf[64];
	char* p;
	strcpy(buf, func);
	p = strtok(buf, " ");
	rule->left_object = create_object(p);
	if(!rule->left_object) {
		printf("Can't create left object\n");
		goto fail;
	}

	p = strtok(p + strlen(p) + 1, " ");
	rule->compare = get_func_ptr(p);
	if(!rule->compare) {
		printf("Can't get compare function\n");
		goto fail;
	}

	p = strtok(p + strlen(p) + 1, " ");
	rule->right_object =  create_object(p);
	if(!rule->right_object) {
		printf("Can't create right object\n");
		goto fail;
	}

	rule->action = rule_action_create(action);
	if(!rule->action) {
		printf("rule action fail: can't allocate rule action\n");
		goto fail;
	}

	if(description) {
		rule->description = malloc(strlen(description + 1));
		if(!rule->description) {
			printf("rule_create fail: can't allocate rule description\n");
			goto fail;
		}
	}
	strcpy(rule->description, description);

	return rule;
fail:
	if(rule->name) {
		free(rule->name);
		rule->name = NULL;
	}

	if(rule->description) {
		free(rule->description);
		rule->description = NULL;
	}

	if(rule->func) {
		free(rule->func);
		rule->func = NULL;
	}

	if(rule->action) {
		free(rule->action);
		rule->action = NULL;
	}

	if(rule) {
		free(rule);
		rule = NULL;
	}

	return NULL;
}

Rule* rule_json_create(json_object* jso) {
	char name[64] = {0,};
	char func[128] = {0,};
	char action[128] = {0,};
	char description[128] = {0,};

	json_object_object_foreach(jso, key1, child_object1) {
		if(!strcmp(key1, "name")) {
			strcpy(name, json_object_to_json_string(child_object1));
		} else if(!strcmp(key1, "func")) {
			strcpy(func, json_object_to_json_string(child_object1));
		} else if(!strcmp(key1, "action")) {
			strcpy(action, json_object_to_json_string(child_object1));
		} else if(!strcmp(key1, "description")) {
			strcpy(description, json_object_to_json_string(child_object1));
		}
	}

	remove_blank(name);
	remove_blank(func);
	remove_blank(action);
	remove_blank(description);

	Rule* rule = rule_create(name, func, action, description);
	if(rule)
		printf("\t%s\t\t%s\n\t\t\t%s\t\t%s\n", name, func, action, description);
	else
		printf("Rule create Fail\n");

	return rule;
}

void rule_process() {
#ifdef TIMER_LOG
	uint64_t avg = 0;
	uint64_t size = map_size(rule_database);
#endif
	MapIterator iter;
	map_iterator_init(&iter, rule_database);
	while(map_iterator_has_next(&iter)) {
		MapEntry* entry = map_iterator_next(&iter);
		Rule* rule = entry->data;

#ifdef TIMER_LOG
#include <time.h>
		struct timespec pre;
		clock_gettime(CLOCK_REALTIME, &pre);
#endif
		int64_t l_value = rule->left_object->type == OBJECT_TYPE_INT64 ? rule->left_object->value : rule->left_object->sensor_obj.func(rule->left_object->sensor_obj.data);
		int64_t r_value = rule->right_object->type == OBJECT_TYPE_INT64 ? rule->right_object->value : rule->right_object->sensor_obj.func(rule->right_object->sensor_obj.data);
		bool result = rule->compare(l_value, r_value);

#ifdef TIMER_LOG
		struct timespec current;
		clock_gettime(CLOCK_REALTIME, &current);
		avg += (uint64_t)(current.tv_sec - pre.tv_sec) * 1000 * 1000 * 1000 + (uint64_t)(current.tv_nsec - pre.tv_nsec);
#endif
		if(result) {
			rule->action->action->action(rule->action->actuator);
			//printf("Name: %s\n\tFunc: %s\n\tDescription: %s\n\n", rule->name, rule->func, rule->description);
			//printf("%s\n", rule->action->action->name);
		}
	}
#ifdef TIMER_LOG
	extern void timer_end(uint64_t ns);
	timer_end(avg / size);
#endif
}
