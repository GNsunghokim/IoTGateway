#include <stdbool.h>
#include <malloc.h>
#include <string.h>
#include <util/types.h>
#include <util/map.h>

#include "rule.h"

static Map* rule_database;

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

bool rule_create(char* name, uint8_t type, char* func, char* description) {
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

	if(rule) {
		free(rule);
		rule = NULL;
	}

	return false;
}
