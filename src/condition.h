#include <util/types.h>

#define CONDITION_TYPE_NONE	0X0
#define CONDITION_TYPE_NORMAL	0X1
#define CONDITION_TYPE_URGENT	0X2

typedef struct _Condition {
	char* name;
	uint16_t type;
	bool (*func)(Sensor* sensor);
}
