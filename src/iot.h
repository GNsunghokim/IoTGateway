#include <util/types.h>
#include <util/map.h>

typedef struct _Action {
	char* name;
	bool (*func)(void* data);
} Action;

typedef struct _Actuator {
	char* name;
	Map* actions;
} Actuator;

typedef struct _Sensor {
	char* name;
	uint64_t* datas;
	uint32_t size;
	uint32_t index;
} Sensor;

typedef struct _IoTDevice {
	char* name;
	char* description;
	uint32_t ip;

	Map* sensors;
	Map* actuators;
} IoTDevice;

bool iot_init();
bool iot_create_device();
