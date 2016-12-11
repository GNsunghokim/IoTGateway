#ifndef __PRINT_H__
#define __PRINT_H__

#include <actuator.h>

bool print_warm(Actuator* actuator);
bool print_cold(Actuator* actuator);
bool print_bright(Actuator* actuator);
bool print_dark(Actuator* actuator);
void* print_get_action(char* func);
#endif
