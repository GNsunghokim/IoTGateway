#include <util/types.h>
#include <util/map.h>

typedef struct _IoTDevice {
	char* name;
	char* description;
	uint32_t ip;

	Map* sensors;
	Map* actuators;
} IoTDevice;

bool iot_init();
bool iot_create_device(char* name, char* description, uint32_t ip);
bool iot_delete_device(char* name);

#define		IOT_DEVICE_NONE		0x00
#define		IOT_DEVICE_SENSOR	0x01
#define		IOT_DEVICE_ACTUATOR	0x02

bool iot_add_module(char* name, uint8_t type, void* module);
void* iot_remove_module(char* name, uint8_t type, char* module_name);
bool iot_process(Packet* packet);
