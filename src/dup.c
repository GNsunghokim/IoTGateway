#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <util/types.h>
#include <util/map.h>
#include <net/packet.h>
#include <net/ether.h>
#include <net/arp.h>
#include <net/ip.h>
#include <net/udp.h>
#include <net/tcp.h>
#include <net/ni.h>
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
	int32_t fd;
} Server;

bool dup_process(Packet* packet) {
	MapIterator iter;
	map_iterator_init(&iter, servers);

	while(map_iterator_has_next(&iter)) {
		MapEntry* entry = map_iterator_next(&iter);
		Server* server = entry->data;

		Ether* ether = (Ether*)(packet->buffer + packet->start);
		if(endian16(ether->type) == ETHER_TYPE_IPv4) {
			IP* ip = (IP*)ether->payload;
			if(ip->protocol == IP_PROTOCOL_UDP) {
// 				UDP* udp = (UDP*)ip->body;
// 				int len = 0;
// 				while(len == endian16(udp->length)) {
// 					len += write(server->fd, udp->body + len, endian16(udp->length) - len);
// 					printf("UDP Write %d\n", len);
// 				}
			} else if(ip->protocol == IP_PROTOCOL_TCP) {
				TCP* tcp = (TCP*)ip->body;
				int len = 0;
				int payload_len = endian16(ip->length) - ((ip->ihl + tcp->offset) * 4);
				while(len ==  payload_len) {
					len += write(server->fd, tcp->payload + len, payload_len - len);
					printf("TCP Write %d\n", len);
				}
			}
		}
//		if(server->type == DUP_SERVER_TYPE_OVERLAY) {
//
//		} else if(server->type == DUP_SERVER_TYPE_FORWARDING) { 
//
//		}
//		write(fd, );
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

	server->fd = open("test.dat", O_WRONLY | O_CREAT, 0644);
	if(server->fd < 0) {
		printf("Can't open file\n");
		free(server->name);
		free(server);
		return false;
	}
	
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
	close(server->fd);
	free(server->name);
	free(server);

	return true;
}
