#ifndef __DATA_H__
#define __DATA_H__

#include <stdint.h>
#include <json.h>
#include <json_util.h>
typedef struct _Data {
	char* name;
	int64_t* values;
	uint64_t size;
	uint64_t count;
} Data;

Data* data_create(char* name, uint64_t size);
Data* data_json_create(json_object* jso);
void data_delete(Data* data);
void data_push_value(Data* data, int64_t value);
int64_t data_get_newest(Data* data);
int64_t data_get_avg(Data* data);
int64_t data_get_max(Data* data);
#endif /*__DATA_H__*/
