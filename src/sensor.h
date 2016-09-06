typedef struct _Sensor {
	char* name;
	uint64_t* datas;
	uint32_t size;
	uint32_t index;
} Sensor;

Sensor* sensor_create(char* name, uint16_t size);
bool sensor_delete(Sensor* sensor);
