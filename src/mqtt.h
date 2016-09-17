#ifndef __MQTT_H__
#define __MQTT_H__
/**
 * MQTT Header
 */
typedef struct _MQTT {
	uint8_t		type: 4;
	uint8_t		dup: 1;
	uint8_t		qos: 2;
	uint8_t		retain:1 ;
	uint8_t		length;
	uint8_t		body[0];
} __attribute__ ((packed)) MQTT;
#endif
