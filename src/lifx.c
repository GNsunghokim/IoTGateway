#include <stdio.h>
#include <stdbool.h>

bool lifx_turn_on_red(void* data) {
	printf("Lifix Reaction : Turn on Red\n");
	return true;
}

bool lifx_turn_on_blue(void* data) {
	printf("Lifix Reaction: Turn on Blue\n");
	return true;
}
