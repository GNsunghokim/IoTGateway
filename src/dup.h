/* PacketNgin Packet Duplicater */
#ifndef __DUP_H__
#define __DUP_H__

#include <stdbool.h>
#include <netinet/in.h>

#include <json.h>
#include <json_util.h>

#define DUP_TYPE_FILE			1
#define DUP_TYPE_TCP_CLIENT		2//Default

typedef struct _Dup {
	char* name;
	uint8_t type;
	char* description;
	union {
		char* path;
		struct sockaddr_in server;
	};
	int fd;

	bool (*open)(struct _Dup* dup);
	bool (*close)(struct _Dup* dup);
	int (*write)(struct _Dup* dup, void* buf, ssize_t size);
} Dup;

bool dup_init();
bool dup_process(void* buf, ssize_t size);
bool dup_delete(char* name);
bool dup_json_create(json_object *jso);
#endif
