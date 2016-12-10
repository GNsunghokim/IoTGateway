/* 
 * tcpclient.c - A simple TCP client
 * usage: tcpclient <host> <port>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "client.h"

bool client_connect(Dup* dup) {
	if(dup->fd)
		return false;

	dup->fd = socket(AF_INET, SOCK_STREAM, 0);
	if(dup->fd < 0) {
		dup->fd = 0;
		return false;
	}

	if(connect(dup->fd, (struct sockaddr*)&dup->server, sizeof(struct sockaddr_in)) < 0) {  //TODO timeout
		dup->fd = 0;
		return false;
	}

	return true;
}

bool client_disconnect(Dup* dup) {
	if(!dup->fd)
		return true;

	close(dup->fd);
	dup->fd = 0;

	return true;
}

int client_send(Dup* dup, void* buf, ssize_t size) {
	if(!dup)
		return 0;

	if(!dup->fd)
		return 0;

	return write(dup->fd, buf, size);
}
