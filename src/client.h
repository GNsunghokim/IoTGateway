#include "dup.h"

bool client_connect(Dup* dup);
bool client_disconnect(Dup* dup);
int client_send(Dup* dup, void* buf, ssize_t size);
