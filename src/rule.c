#include <stdbool.h>
#include <malloc.h>
#include <string.h>
#include <util/types.h>
#include <util/map.h>

#include <json.h>
#include <json_util.h>

#include "iot.h"
#include "rule.h"

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

	if(object->type == OBJECT_TYPE_INT64) {
		object->value = 0;
	} else if(object->type == OBJECT_TYPE_SENSOR) {
		object->sensor_obj.sensor = NULL;
		object->sensor_obj.func = NULL;
	} else {
		goto fail;
	}

	return object;

fail:
	if(object)
		free(object);

	return NULL;
}

static void remove_blank(char* str) {
	char* start = str;
	char* end = str + strlen(str);

	//cut head
	for(; start < end; start++) {
		if(*start != ' ') {
			break;
		}
	}

	//cut tail
	for(; end > start; --end) {
		if(*end != ' ') {
			break;
		}
	}

	strncpy(str, start, end - start);
	printf("Debug str%s\n", str);
}	

static void* parse_arg(char args[][64], char* line) {
	int num = sizeof(funcs) / sizeof(Func);

	for(int i = 0; i < num; i++) {
		char* func_ptr = strstr(funcs[i].name, line);
		if(!func_ptr) {
			strncpy(args[0], line, func_ptr - line);
			remove_blank(args[0]);
			strcpy(args[1], funcs[i].name);
			strcpy(args[2], func_ptr + strlen(funcs[i].name));
			remove_blank(args[2]);

			return args;
		}
	}

	return NULL;
}

bool rule_init() {
	rule_database = map_create(16, map_string_hash, map_string_equals, NULL);
	if(!rule_database)
		return false;

	return true;
}

static bool rule_func_check(char* func) {
	return true;
}

bool rule_delete(char* name) {
	return false;
}

typedef struct _RuleAction {
	char* action;
	IoTDevice* iot_device;
	bool (*func)(IoTDevice* iot_device);
} RuleAction;

static RuleAction* rule_action_create(char* action) {
	RuleAction* rule_action = malloc(sizeof(RuleAction));
	char* p;

	if(!rule_action) {
		printf("Can't allocate Rule Action\n");
		return NULL;
	}
	memset(rule_action, 0, sizeof(RuleAction));

	rule_action->action = malloc(strlen(action) + 1);
	if(!rule_action->action) {
		printf("Can't allocate action\n");
		goto fail;
	}
	strcpy(rule_action->action, action);

	/**
	   * rule action "iot_device_name.action"
	 **/
	p = strtok(action, ".");
	if(!p) {
		goto fail;
	}
	rule_action->iot_device = iot_get_iot_device(p);
	if(!rule_action->iot_device) {
		printf("Can't allocate iot device\n");
		goto fail;
	}

	p = strtok(action, ".");
	if(!p) {
		goto fail;
	}
	rule_action->func = iot_get_action(rule_action->iot_device, p);
	if(!rule_action->func) {
		printf("Can't allocate action\n");
		goto fail;
	}

	return rule_action;

fail:
	if(rule_action->action) {
		free(rule_action->action);
	}

	if(rule_action) {
		free(rule_action);
	}
	
	return NULL;
}

bool rule_create(char* name, char* func, char* action, char* description) {
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
	char args[3][64];
	if(!parse_arg(args, func)) {
		printf("Can't parse funcs\n");
		goto fail;
	}
	rule->left_object = create_object(args[0]);
	if(!rule->left_object) {
		printf("Can't create left object\n");
		goto fail;
	}
	rule->compare = get_func_ptr(args[1]);
	if(!rule->compare) {
		printf("Can't get compare function\n");
		goto fail;
	}
	rule->right_object =  create_object(args[2]);
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

	if(!map_put(rule_database, name, rule)) {
		printf("rule_create fail: can't add rule_database\n");
		goto fail;
	}

	return true;

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

	return false;
}

bool rule_json_create(json_object* jso) {
	char name[64];
	char func[128];
	char action[128];
	char description[128];

	json_object_object_foreach(jso, key1, child_object1) {
		if(!strcmp(key1, "name")) {
			strcpy(name, json_object_to_json_string(child_object1));
		} else if(!strcmp(key1, "func")) {
			strcpy(func, json_object_to_json_string(child_object1));
		} else if(!strcmp(key1, "action")) {
			strcpy(action, json_object_to_json_string(child_object1));
		} else if(!strcmp(key1, "description")) {
			strcpy(description, json_object_to_json_string(child_object1));
		} else {
			printf("???\n");
		}
	}

	if(!rule_create(name, func, action, description)) {
		return false;
	}

	printf("\t\t%s\t\t%s\n\t\t\t\t%s\t\t%s\n", name, func, action, description);

	return true;
}
