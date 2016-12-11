#include <stdio.h>
#include <stdbool.h>
#include <net/ether.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "lifx.h"

int fd = 0;
uint8_t sequence = 0;
static void broadcast(void* data, ssize_t size) {
	if(fd == 0) {
		fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	int broadcastPermission = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, 
				sizeof(broadcastPermission)) < 0)
		perror("setsockopt() failed");
	}
	if(fd < 0)
		return;

	struct sockaddr_in s;
	memset(&s, 0, sizeof(struct sockaddr_in));
	s.sin_family = AF_INET;
	s.sin_port = htons(56700);
	s.sin_addr.s_addr = inet_addr("192.168.100.255");

	if(sendto(fd, data, size, 0, (struct sockaddr *)&s, sizeof(struct sockaddr_in)) < 0)
		perror("sendto");
}

static void lifx_set_color(Actuator* actuator, HSBK* color, uint32_t duration) {
	LIFX* lifx = malloc(sizeof(LIFX) + sizeof(LIFX_Light_Set_Color));
	memset(lifx, 0, sizeof(LIFX) + sizeof(LIFX_Light_Set_Color));

	lifx->size = sizeof(LIFX) + sizeof(LIFX_Light_Set_Color);
	//lifx->target = 0;//or mac address 0 mean all device
	lifx->protocol = 1024;
	lifx->addressable = 1;
	lifx->sequence = sequence++;
	lifx->type = LIFX_LIGHT_SET_COLOR;

	LIFX_Light_Set_Color* lifx_set_color = (LIFX_Light_Set_Color*)lifx->payload;
	memcpy(&lifx_set_color->color, color, sizeof(HSBK));
	lifx_set_color->duration = duration;

	broadcast(lifx, sizeof(LIFX) + sizeof(LIFX_Light_Set_Color));
}	

static void lifx_set_power(Actuator* actuator, uint16_t level, uint32_t duration){
	LIFX* lifx = malloc(sizeof(LIFX) + sizeof(LIFX_Device_Set_Power));
	memset(lifx, 0, sizeof(LIFX) + sizeof(LIFX_Device_Set_Power));

	lifx->size = sizeof(LIFX) + sizeof(LIFX_Device_Set_Power);
	//lifx->target = 0;//or mac address 0 mean all device
	lifx->protocol = 1024;
	lifx->addressable = 1;

	lifx->sequence = sequence++;
	lifx->type = LIFX_DEVICE_SET_POWER;

	LIFX_Device_Set_Power* lifx_set_power = (LIFX_Device_Set_Power*)lifx->payload;
	lifx_set_power->level = level;

	broadcast(lifx, sizeof(LIFX) + sizeof(LIFX_Device_Set_Power));
}

bool lifx_turn_on(Actuator* actuator) {
	lifx_set_power(actuator, 0xffff, 0xffffffff);
	printf("Lifix Reaction : Turn on\n");
	return true;
}

bool lifx_turn_off(Actuator* actuator) {
	lifx_set_power(actuator, 0x0, 0xffffffff);
	printf("Lifix Reaction : Turn off\n");
	return true;
}

bool lifx_turn_on_red(Actuator* actuator) {
	HSBK color;
	color.hue = 0xff00;
	color.saturation = 0xff00;
	color.brightness = 0xff00;
	color.kelvin = 0xff00;

	lifx_set_power(actuator, 0xffff, 0xffffffff);
	lifx_set_color(actuator, &color, 0xffffffff);

	return true;
}

#include <unistd.h>
bool lifx_turn_on_blue(Actuator* actuator) {
	lifx_set_power(actuator, 0xffff, 0xffffffff);

	HSBK color;
	for(int i = 0; i < 0xffff; i++) {
		color.hue = endian16(i);
		color.saturation = 0xffff;
		color.brightness = 0xffff;
		color.kelvin = 0x0dac;

		lifx_set_color(actuator, &color, 0x0);
	}

	return true;
}

void* lifx_get_action(char* func) {
	if(!strcmp(func, "turn_on"))
		return lifx_turn_on;
	else if(!strcmp(func, "turn_off"))
		return lifx_turn_off;
	else if(!strcmp(func, "turn_on_red"))
		return lifx_turn_on_red;
	else if(!strcmp(func, "turn_on_blue"))
		return lifx_turn_on_blue;
	else
		return NULL;
}
