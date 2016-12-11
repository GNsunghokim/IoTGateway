#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <json.h>
#include <json_util.h>

#include <net/tcp.h>

#include <mqtt.h>
#include <util.h>
#include <sensor.h>

bool mqtt_process(MQTT* mqtt) {
	if(mqtt->length < sizeof(MQTT_VHeader)) {
		return false;
	}

	MQTT_VHeader* vheader= (MQTT_VHeader*)mqtt->body;
	ssize_t len = mqtt->length - (sizeof(MQTT_VHeader) + vheader->topic_length);
	char buf[256] = {0,};
	if(len < 0 || len > sizeof(buf)) {
		return false;
	}
	memcpy(buf, (char*)vheader->topic + vheader->topic_length, len);
	json_object* jso = json_tokener_parse(buf);

	if(jso) {
		Sensor* sensor = NULL;
		char name[64] = {0, };
		json_object_object_foreach(jso, key, child_object) {
			if(!strcmp(key, "name_of_sensor")) {
				strcpy(name, json_object_to_json_string(child_object));
				printf("name %s\n", name);
				remove_blank(name);
				sensor = sensor_database_get(name);
			}
#ifdef TIMER_LOG
			if(!strcmp(key, "id_of_sensor")) {
				char id[64] = {0, };
				strcpy(id, json_object_to_json_string(child_object));
				remove_blank(id);
				extern void timer_check(char* id);
				timer_check(id);
			}
#endif
		}

		if(sensor) {
			json_object_object_foreach(jso, key1, child_object1) {
				char name[64] = {0, };

				strcpy(name, key1);
				remove_blank(name);

				Data* data = sensor_get_data(sensor, name);
				if(data) {
					char value[64] = {0, };
					strcpy(value, json_object_to_json_string(child_object1));
					remove_blank(value);
					char* ptr;
					int64_t val = strtol(value, &ptr, 10);
#ifdef DEBUG_MSG
					printf("\t%s: %ld\n", name, val);
#endif
					data_push_value(data, val);
				}
			}
		}
#ifdef DEBUG_MSG
		printf("\n");
#endif
		json_object_put(jso); //free
	}

	return true;
}
