typedef struct _Sensor {
	char* name;
	uint64_t* datas;
	uint32_t size;
	uint32_t index;

	List* conditions;
} Sensor;

bool sensor_create();
