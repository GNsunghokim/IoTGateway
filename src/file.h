#include "dup.h"

bool file_open(Dup* dup);
bool file_close(Dup* dup);
int file_write(Dup* dup, void* buf, ssize_t size);
