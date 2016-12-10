#ifndef __IOT_H__
#define __IOT_H__

#include <stdint.h>
#include <net/packet.h>
#include <util/types.h>
#include <util/map.h>

#include <json.h>
#include <json_util.h>

#include "actuator.h"
#include "sensor.h"

typedef struct _IoTDevice {
	char* name;
	char* id;
	uint32_t address;
	char* description;

	Map* sensors;
	Map* actuators;
} IoTDevice;

bool iot_init();
IoTDevice* iot_create_device(char* name, char* description, char* id, char* address);
bool iot_json_create(json_object *jso);
IoTDevice* iot_get_iot_device(char* name);
void* iot_get_action(IoTDevice* iot_device, char* action);
bool iot_delete_device(char* name);

#define		IOT_DEVICE_NONE		0x00
#define		IOT_DEVICE_SENSOR	0x01
#define		IOT_DEVICE_ACTUATOR	0x02

bool iot_add_module(IoTDevice* iot_device, uint8_t type, void* module);
void* iot_remove_module(char* name, uint8_t type, char* module_name);
void* iot_get_module(IoTDevice* iot_device, uint8_t type, char* name);
bool iot_process(Packet* packet);
#endif
