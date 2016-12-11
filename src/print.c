#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <print.h>

bool print_warm(Actuator* actuator) {
	printf("warm\n");
	return true;
}

bool print_cold(Actuator* actuator) {
	printf("cold\n");
	return true;
}

bool print_bright(Actuator* actuator) {
	printf("bright\n");
	return true;
}

bool print_dark(Actuator* actuator) {
	printf("dark\n");
	return true;
}

void* print_get_action(char* func) {
	if(!strcmp(func, "warm")) {
		return print_warm;
	} else if(!strcmp(func, "cold")) {
		return print_cold;
	} else if(!strcmp(func, "bright")) {
		return print_bright;
	} else if(!strcmp(func, "dark")) {
		return print_dark;
	} else
		return NULL;
}
