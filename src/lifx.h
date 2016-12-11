#ifndef __LIFX_H__
#define __LIFX_H__

#include <actuator.h>

typedef struct _LIFX{
	/* frame */
	uint16_t size;
	uint16_t protocol :12;
	uint8_t  addressable :1;
	uint8_t  tagged :1;
	uint8_t  origin :2;
	uint32_t source;
	/* frame address */
	uint8_t  target[8];
	uint8_t  reserved[6];
	uint8_t  res_required :1;
	uint8_t  ack_required :1;
uint8_t  :6;
	uint8_t  sequence;
	/* protocol header */
uint64_t :64;
	uint16_t type;
uint16_t :16;
	uint8_t payload[0];
	/* variable length payload follows */
} __attribute__ ((packed)) LIFX;

/* LIFX Device Message */
#define LIFX_DEVICE_GET_SERVICE			2
#define LIFX_DEVICE_STATE_SERVICE		3
#define LIFX_DEVICE_GET_HOST_INFO		12
#define LIFX_DEVICE_STATE_HOST_INO		13
#define LIFX_DEVICE_GET_HOST_FIRMWARE		14
#define LIFX_DEVICE_STATE_HOST_FIRMWARE		15
#define LIFX_DEVICE_GET_WIFI_INFO		16
#define LIFX_DEVICE_STATE_WIFI_INFO		17
#define LIFX_DEVICE_GET_WIFI_FIRMWARE		18
#define LIFX_DEVICE_STATE_WIFI_FIRMWARE		19
#define LIFX_DEVICE_GET_WIFI_FIRMWARE		18
#define LIFX_DEVICE_STATE_WIFI_FIRMWARE		19
#define LIFX_DEVICE_GET_POWER			20
#define LIFX_DEVICE_SET_POWER			21
#define LIFX_DEVICE_STATE_POWER			22
#define LIFX_DEVICE_GET_LABEL			23
#define	LIFX_DEVICE_SET_LABEL			24
#define LIFX_DEVICE_STATE_LABEL			25
#define LIFX_DEVICE_GET_VERSION			32
#define LIFX_DEVICE_STATE_VERSION		33
#define LIFX_DEVICE_GET_INFO			34
#define LIFX_DEVICE_STATE_INFO			35
#define	LIFX_DEVICE_ACKNOWLEDGEMENT		45
#define LIFX_DEVICE_GET_LOCATION		48
#define LIFX_DEVICE_STATE_LOCATION		50
#define LIFX_DEVICE_GET_GROUP			51
#define LIFX_DEVICE_STATE_GROUP			53
#define LIFX_DEVICE_ECHO_REQUEST		58
#define LIFX_DEVICE_ECHO_RESPONSE		59

typedef struct _LIFX_Device_State_Service {
	uint8_t		service;
	uint32_t	port;
} __attribute__ ((packed)) LIFX_Device_State_Service;

typedef struct _LIFX_Device_State_Host_Info {
	float		signal;
	uint32_t	tx;
	uint32_t	rx;
	int16_t		reserved;
} __attribute__ ((packed)) LIFX_Device_State_Host_Info;

typedef struct _LIFX_Device_State_Host_Firmware {
	uint64_t	build;
	uint64_t	reserved;
	uint32_t	version;
} __attribute__ ((packed)) LIFX_Device_State_Host_Firmware;

typedef struct _LIFX_Device_State_WIFI_INFO {
	float		signal;
	uint32_t	tx;
	uint32_t	rx;
	int16_t		reserved;
} __attribute__ ((packed)) LIFX_Device_State_WIFI_INFO;

typedef struct _LIFX_Device_State_WIFI_Firmware {
	uint64_t	build;
	uint64_t	reserved;
	uint32_t	version;
} __attribute__ ((packed)) LIFX_Device_State_WIFI_Firmware;

typedef struct _LIFX_Device_Set_Power {
	uint16_t	level;
} __attribute__ ((packed)) LIFX_Device_Set_Power;

typedef struct _LIFX_Device_State_Power {
	uint16_t	level;
} __attribute__ ((packed)) LIFX_Device_State;

typedef struct _LIFX_Device_Set_Label {
	char		label[32];
} __attribute__ ((packed)) LIFX_Device_Set_Label;

typedef struct _LIFX_Device_State_Label {
	char		label[32];
} __attribute__ ((packed)) LIFX_Device_State_Label;

typedef struct _LIFX_Device_State_Version {
	uint32_t	vendor;
	uint32_t	product;
	uint32_t	version;
} __attribute__ ((packed)) LIFX_Device_State_Version;

typedef struct _LIFX_Device_State_Info {
	uint64_t	time;
	uint64_t	uptime;
	uint64_t	downtime;
} __attribute__ ((packed)) LIFX_Device_State_Info;

typedef struct _LIFX_Device_State_Location {
	uint8_t 	location[16];
	char		label[32];
	uint64_t	updated_at;
} __attribute__ ((packed)) LIFX_Device_State_Location;

typedef struct _LIFX_Device_State_Group {
	uint8_t 	group[16];
	char		label[32];
	uint64_t	updated_at;
} __attribute__ ((packed)) LIFX_Device_State_Group;

typedef struct _LIFX_Device_Echo_Request {
	uint8_t		payload[64];
} __attribute__ ((packed)) LIFX_Device_Echo_Request;

typedef struct _LIFX_Device_Echo_Response {
	uint8_t		payload[64];
} __attribute__ ((packed)) LIFX_Device_Echo_Response;

/* LIFX Light Message */
#define LIFX_LIGHT_GET			101
#define LIFX_LIGHT_SET_COLOR		102
#define LIFX_LIGHT_STATE		107
#define LIFX_LIGHT_GET_POWER		116
#define LIFX_LIGHT_SET_POWER		117
#define LIFX_LIGHT_STATE_POWER		118

typedef struct _HSBK {
	uint16_t	hue;
	uint16_t	saturation;
	uint16_t	brightness;
	uint16_t	kelvin;
} __attribute__ ((packed)) HSBK;	

typedef struct _LIFX_Light_Set_Color {
	uint8_t		reserved;
	HSBK		color;
	uint32_t	duration;
} __attribute__ ((packed)) LIFX_Light_Set_Color;

typedef struct _LIFX_Light_State {
	HSBK		color;
	uint16_t	reserved_0;
	uint16_t	power;
	char		label[32];
	uint64_t	reserved_1;
} __attribute__ ((packed)) LIFX_Light_State;

typedef struct _LIFX_Light_Set_Power {
	uint16_t	level;
	uint32_t	duration;
} __attribute__ ((packed)) LIFX_Light_Set_Power;

typedef struct _LIFX_Light_State_Power {
	uint16_t	level;
} __attribute__ ((packed)) LIFX_Light_State_Power;

bool lifx_turn_on(Actuator* actuator);
bool lifx_turn_off(Actuator* actuator);
bool lifx_turn_on_red(Actuator* actuator);
bool lifx_turn_on_blue(Actuator* actuator);
void* lifx_get_action(char* func);
#endif
