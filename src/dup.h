/* PacketNgin Packet Duplicater */

#define DUP_SERVER_TYPE_OVERLAY		1//Default
#define DUP_SERVER_TYPE_FORWARDING	2

bool dup_init();
bool dup_process(Packet* packet);
bool dup_server_create(uint32_t ip, uint16_t port, char* name);
bool dup_server_remove(char* name);
