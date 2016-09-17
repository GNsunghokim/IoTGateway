#include <stdio.h>
#include <stdbool.h>

#include "iot.h"

bool lifx_turn_on(IoTDevice* iot_device) {
	printf("Lifix Reaction : Turn on\n");
	return true;
}

bool lifx_turn_off(IoTDevice* iot_device) {
	printf("Lifix Reaction : Turn off\n");
	return true;
}

bool lifx_turn_on_red(IoTDevice* iot_device) {
	printf("Lifix Reaction : Turn on Red\n");
	return true;
}

bool lifx_turn_on_blue(IoTDevice* iot_device) {
	printf("Lifix Reaction: Turn on Blue\n");
	return true;
}
