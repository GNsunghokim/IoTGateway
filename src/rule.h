#include <util/types.h>

#include <json.h>
#include <json_util.h>

/*
   IoT Gateway Prototype not support various type rule
#define CONDITION_TYPE_NONE	0X0
#define CONDITION_TYPE_NORMAL	0X1
#define CONDITION_TYPE_URGENT	0X2
*/

typedef struct _Rule {
	char* name;
	uint8_t type;
	char* func;
	char* description;
	bool activate;
} Rule;

bool rule_create(char* name, uint8_t type, char* func, char* description);
bool rule_json_create(json_object* jso);
