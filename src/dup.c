#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <util/types.h>
#include <util/map.h>
#include <net/nic.h>
#include <net/packet.h>

static Map* servers;

bool dup_init() {
	servers = map_create(32, map_string_hash, map_string_equals, NULL);
	if(!servers)
		return false;

	return true;
}


typedef struct _Server {
	uint32_t ip;
	uint16_t port;
	char* name;
	uint8_t type;
} Server;

bool dup_process(Packet* packet) {
	MapIterator iter;
	map_iterator_init(&iter, servers);

	while(map_iterator_has_next(&iter)) {
		MapEntry* entry = map_iterator_next(&iter);
		Server* server = entry->data;

		printf("%p", server);
		if(server->type == DUP_SERVER_TYPE_OVERLAY) {

		} else if(server->type == DUP_SERVER_TYPE_FORWARDING) { 

		}
		//packet duplicate to CEP server and Storage Server
		//need protocol between server and iotgateway
	}

	return true;
}

bool dup_server_create(uint32_t ip, uint16_t port, char* name, uint8_t type) {
	Server* server = (Server*)malloc(sizeof(Server));
	if(!server)
		return false;

	server->name = malloc(strlen(name) + 1);
	if(!server->name) {
		free(server);
		return false;
	}

	strcpy(server->name, name);
	server->ip = ip;
	server->port = port;
	server->type = type;
	
	if(!map_put(servers, server->name, server)) {
		free(server->name);
		free(server);
	}

	printf("Add server: %s %d %d\n", name, ip, port);

	return true;
}

bool dup_server_remove(char* name) {
	Server* server = map_remove(servers, name);
	if(!server)
		return false;

	printf("Delete server: %s %d %d\n", server->name, server->ip, server->port);
	free(server->name);
	free(server);

	return true;
}
