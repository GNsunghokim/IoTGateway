#include <util/map.h>
#include <util/types.h>

static Sensor* sensor_create() {
	char* name = NULL;
	uint16_t size = 0;

	Sensor* sensor = (Sensor*)malloc(sizeof(Sensor));
	if(!sensor)
		return NULL;

	memset(sensor, 0 ,sizeof(Sensor));
	sensor->name = (char*)malloc(strlen(name) + 1);
	if(!sensor->name)
		goto fail;

	strcpy(sensor->name, name);

	sensor->datas = (uint64_t*)malloc(sizeof(uint64_t) * size);
	if(!sensor->datas)
		goto fail;

	sensor->size = size;
	memset(sensor->datas, 0 ,sizeof(uint64_t) * size);

	return sensor;

fail:
	if(sensor->datas) {
		free(sensor->datas);
		sensor->datas = NULL;
	}

	if(sensor->name) {
		free(sensor->name);
		sensor->name = NULL;
	}

	if(sensor) {
		free(sensor);
		sensor = NULL;
	}

	return NULL;
} 

static void sensor_delete(Sensor* sensor) {
	if(sensor->datas) {
		free(sensor->datas);
		sensor->datas = NULL;
	}

	if(sensor->name) {
		free(sensor->name);
		sensor->name = NULL;
	}

	if(sensor) {
		free(sensor);
		sensor = NULL;
	}

	return NULL;
}
