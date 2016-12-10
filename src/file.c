#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "file.h"

bool file_open(Dup* dup) {
	if(dup->fd)
		return false;

	dup->fd = open(dup->path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if(dup->fd < 0) {
		dup->fd = 0;
		return false;
	}

	return true;
}

bool file_close(Dup* dup) {
	if(!dup->fd)
		return true;

	close(dup->fd);
	dup->fd = 0;

	return true;
}

int file_write(Dup* dup, void* buf, ssize_t size) {
	if(!dup)
		return 0;

	if(!dup->fd)
		return 0;

	return write(dup->fd, buf, size);
}
