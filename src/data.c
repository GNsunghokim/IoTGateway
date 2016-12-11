#include <malloc.h>
#include <string.h>

#include <data.h>
#include <util.h>

Data* data_create(char* name, uint64_t size) {
	if(!strlen(name))
		return NULL;

	Data* data = (Data*)malloc(sizeof(Data));
	if(!data)
		return NULL;

	memset(data, 0, sizeof(Data));

	data->name = (char*)malloc(strlen(name) + 1);
	if(!data->name)
		goto fail;
	strcpy(data->name, name);

	data->values = (int64_t*)malloc(sizeof(int64_t) * size);
	if(!data->values)
		goto fail;
	memset(data->values, 0 , sizeof(int64_t) * size);

	data->size = size;
	data->count = 0;

	return data;
fail:
	if(data->values)
		free(data->values);

	if(data->name)
		free(data->name);

	free(data);
	return NULL;
}

Data* data_json_create(json_object* jso) {
	char name[64] = {0, };
	uint64_t size = 10;

	//TODO add new value
	json_object_object_foreach(jso, key, child_object) { 
		if(!strcmp(key, "name")) {
			strcpy(name, json_object_to_json_string(child_object));
			remove_blank(name);
		} else if(!strcmp(key, "size")) {
			size = json_object_get_int64(child_object);
		}
	}

	return data_create(name, size);
}

void data_delete(Data* data) {
	free(data->values);
	free(data->name);
	free(data);
}

void data_push_value(Data* data, int64_t value) {
	data->values[data->count % data->size] = value;
	data->count++;
}

int64_t data_get_newest(Data* data) {
	if(data->count == 0) {
		return 0;
	} else {
		return data->values[(data->count - 1) % data->size];
	}
}

int64_t data_get_avg(Data* data) {
	int64_t sum = 0;
	if(data->count < data->size) {
		for(int i = 0; i < data->count; i++) {
			sum += data->values[i];
		}

		return sum / data->count;
	} else {
		for(int i = 0; i < data->size; i++) {
			sum += data->values[i];
		}

		return sum / data->size;
	}
}

int64_t data_get_max(Data* data) {
	int64_t max = 0;
	if(data->count < data->size) {
		for(int i = 0; i < data->count; i++) {
			if(max < data->values[i])
				max = data->values[i];
		}
	} else {
		for(int i = 0; i < data->size; i++) {
			if(max < data->values[i])
				max = data->values[i];
		}
	}

	return max;
}
