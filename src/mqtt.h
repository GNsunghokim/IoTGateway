/**
 * MQTT Header
 */
typedef struct _UDP {
	uint8_t		type: 4;
	uint8_t		dup: 1;
	uint8_t		qos: 2;
	uint8_t		retain:1 ;
	uint8_t		body[0];
} __attribute__ ((packed)) MQTT;
