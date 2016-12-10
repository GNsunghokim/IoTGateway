#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <sys/socket.h>
#include <util/types.h>
#include <util/map.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "util.h"
#include "dup.h"
#include "file.h"
#include "client.h"

static Map* dups;
bool dup_init() {
	dups = map_create(32, map_string_hash, map_string_equals, NULL);
	if(!dups)
		return false;

	return true;
}

bool dup_process(void* buf, ssize_t length) {
	MapIterator iter;
	map_iterator_init(&iter, dups);

	while(map_iterator_has_next(&iter)) {
		MapEntry* entry = map_iterator_next(&iter);
		Dup* dup = entry->data;
		int ret = dup->write(dup, buf, length);
		if(ret < 0) {
			printf("Error\n");
			dup->close(dup);
			map_iterator_remove(&iter);
		}
	}

	return true;
}

static Dup* dup_file_create(char* name, char* path, char* description) {
	Dup* dup = (Dup*)malloc(sizeof(Dup));
	if(!dup)
		return NULL;
	memset(dup, 0, sizeof(Dup));

	dup->name = malloc(strlen(name) + 1);
	if(!dup->name) {
		printf("Allocation Fail\n");
		goto fail;
	}
	strcpy(dup->name, name);

	dup->path = malloc(strlen(path) + 1);
	if(!dup->path) {
		printf("Allocation Fail\n");
		goto fail;
	}
	strcpy(dup->path, path);

	dup->description = malloc(strlen(description) + 1);
	if(!dup->description) {
		printf("Allocation Fail\n");
		goto fail;
	}
	strcpy(dup->description, description);

	dup->type = DUP_TYPE_FILE;

	dup->open = file_open;
	dup->close = file_close;
	dup->write = file_write;

	if(!map_put(dups, dup->name, dup)) {
		printf("Map put Fail\n");
		goto fail;
	}

	return dup;

fail:
	if(dup->name)
		free(dup->name);
	if(dup->path)
		free(dup->path);
	if(dup->description)
		free(dup->description);

	free(dup);

	return NULL;
}

static bool dup_file_delete(Dup* dup) {
	if(!dup)
		return false;
	if(dup->fd)
		dup->close(dup);

	if(dup->name)
		free(dup->name);
	if(dup->path)
		free(dup->path);
	if(dup->description)
		free(dup->description);

	free(dup);

	return true;
}

static Dup* dup_tcp_client_create(char* name, char* ip, char* port, char* description) {
	Dup* dup = (Dup*)malloc(sizeof(Dup));
	if(!dup)
		return NULL;
	memset(dup, 0, sizeof(Dup));

	dup->name = malloc(strlen(name) + 1);
	if(!dup->name) {
		free(dup);
		return dup;
	}
	strcpy(dup->name, name);

	dup->description = malloc(strlen(description) + 1);
	if(!dup->description) {
	}
	strcpy(dup->description, description);

	dup->type = DUP_TYPE_TCP_CLIENT;

	dup->server.sin_family = AF_INET;
	dup->server.sin_addr.s_addr = inet_addr(ip);
	dup->server.sin_port = htons(port);

	dup->open = client_connect;
	dup->close = client_disconnect;
	dup->write = client_send;

	if(!map_put(dups, dup->name, dup)) {
		goto fail;
	}

	return dup;

fail:
	if(dup->name)
		free(dup->name);

	if(dup->description)
		free(dup->description);

	free(dup);
	
	return NULL;
}

static bool dup_tcp_client_delete(Dup* dup) {
	if(!dup)
		return false;
	if(dup->fd)
		dup->close(dup);

	if(dup->name)
		free(dup->name);

	if(dup->description)
		free(dup->description);

	free(dup);
	//TODO
	return true;
}

bool dup_delete(char* name) {
	Dup* dup = map_remove(dups, name);
	if(!dup)
		return false;

	dup->close(dup);
	if(dup->type == DUP_TYPE_FILE) {
		dup_file_delete(dup);
	} else if(dup->type == DUP_TYPE_TCP_CLIENT) {
		dup_tcp_client_delete(dup);
	}

	printf("Delete dup: %s\n", dup->name);

	return true;
}

bool dup_json_create(json_object *jso) {
	char name[64];
	char path[128];//path
	char type[16];
	char description[128];
	char ip[32];
	char port[32];

	json_object_object_foreach(jso, key1, child_object1) {
		if(!strcmp(key1, "name")) {
			strcpy(name, json_object_to_json_string(child_object1));
		} else if(!strcmp(key1, "path")) {
			strcpy(path, json_object_to_json_string(child_object1));
		} else if(!strcmp(key1, "type")) {
			strcpy(type, json_object_to_json_string(child_object1));
		} else if(!strcmp(key1, "ip")) {
			strcpy(ip, json_object_to_json_string(child_object1));
		} else if(!strcmp(key1, "port")) {
			strcpy(port, json_object_to_json_string(child_object1));
		} else if(!strcmp(key1, "description")) {
			strcpy(description, json_object_to_json_string(child_object1));
		} else {
			printf("Unknown Value\n");
			return false;
		}
	}

	if(!strlen(type))
		return false;

	remove_blank(type);
	if(!strcmp("file", type)) {
		remove_blank(name);
		remove_blank(path);
		remove_blank(description);

		Dup* dup =  dup_file_create(name, path, description);
		if(!dup) {
			printf("dup %p\n", dup);
			return false;
		}

		printf("\tName: %s\n", name);
		printf("\tPath: %s\n", path);
		printf("\tdescription: %s\n", description);
		dup->open(dup);

		return true;
	} else if(!strcmp("tcp-client", type)) {
		remove_blank(name);
		remove_blank(ip);
		remove_blank(port);
		remove_blank(description);

		Dup* dup =  dup_tcp_client_create(name, ip, port, description);
		if(!dup)
			return false;
		printf("\tName: %s\n", name);
		printf("\tPath: %s:%s\n", ip, port);
		printf("\tdescription: %s\n", description);
		dup->open(dup);

		return true;
	}

	return false;
}
