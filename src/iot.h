#ifndef __IOT_H__
#define __IOT_H__

#include <stdint.h>
#include <net/packet.h>
#include <util/types.h>
#include <util/map.h>

#include <json.h>
#include <json_util.h>

typedef struct _IoTDevice {
	char* name;
	char* id;
	uint32_t address;
	char* description;

	Map* sensors;
	Map* actuators;
} IoTDevice;

bool iot_init();
// IoTDevice* iot_create_device(char* name, char* description, char* id, char* address);
// bool iot_json_create(json_object *jso);
// IoTDevice* iot_get_iot_device(char* name);
// bool iot_delete_device(char* name);

bool iot_process(Packet* packet);
#endif
