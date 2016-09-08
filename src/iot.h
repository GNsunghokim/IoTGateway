#include <net/packet.h>
#include <util/types.h>
#include <util/map.h>

#include <json.h>
#include <json_util.h>

typedef struct _IoTDevice {
	char* name;
	char* description;
	uint64_t mac;

	Map* sensors;
	Map* actuators;
} IoTDevice;

bool iot_init();
IoTDevice* iot_create_device(char* name, char* description, uint64_t mac);
bool iot_json_create(json_object *jso);
bool iot_delete_device(char* name);

#define		IOT_DEVICE_NONE		0x00
#define		IOT_DEVICE_SENSOR	0x01
#define		IOT_DEVICE_ACTUATOR	0x02

bool iot_add_module(IoTDevice* iot_device, uint8_t type, void* module);
void* iot_remove_module(char* name, uint8_t type, char* module_name);
bool iot_process(Packet* packet);
