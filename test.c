#include <stdio.h>

#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <json.h>
#include <json_util.h>

#include "src/iot.h"
#include "src/rule.h"

int main() {
	printf("Test Function\n");
	//dup_init();
	iot_init();
	//rule_init();

	json_object *jso = json_object_from_file("./example/iot_config.json");
	if (jso) {
		//json_object_put(jso);
		json_object_object_foreach(jso, key, child_object) {

			if(!strcmp(key, "iot-device")) {
				for(int i =0; i < json_object_array_length(child_object); i++) {
					json_object* iot_object = json_object_array_get_idx(child_object, i);
					iot_json_create(iot_object);
					printf("\n");
				}
			} else if(!strcmp(key, "rule")) {
				for(int i =0; i < json_object_array_length(child_object); i++) {
					json_object* rule_object = json_object_array_get_idx(child_object, i);
					rule_json_create(rule_object);
					printf("\n");
				}
			}
		}
	}

	return 0;
}
