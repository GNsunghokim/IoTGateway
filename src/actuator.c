#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <util/map.h>
#include <util/types.h>

Map* actuators;

typedef struct _Actuaotr {

} Actuaotr;

bool actuator_init() {
	actuators = map_create(32, map_string_hash, map_string_equlas, NULL);
	if(!actuators)
		return false;


	return true;
}

bool actuator
