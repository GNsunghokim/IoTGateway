#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <timer.h>

void _strncpy(char* dest, char* src, int len) {
	memmove(dest, src, len);
	dest[len] = '\0';
}

void remove_blank(char* str) {
	char* start = str;
	char* end = str + strlen(str);

	//cut head
	for(; start < end; start++) {
		if(*start != ' ' && *start != '"') {
			break;
		}
	}

	//cut tail
	for(; end > start; end--) {
		if(*(end - 1) != ' ' && *(end - 1) != '"') {
			break;
		}
	}

	_strncpy(str, start, end - start);
}	

#ifdef TIMER_LOG
static uint64_t pre_us;
static int fd = 0;
static int current_id = -1;
static char buf[128];

void timer_start(int id) {
	if(current_id != id || !fd) {
		if(fd) {
			close(fd);
			fd = 0;
		}
		char name[16];
		sprintf(name, "timer_log%d.dat", id);
		printf("timer file name: %s\n", name);
		fd = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		current_id = id;
	}
	pre_us = time_us();
}

void timer_end() {
	uint64_t us = time_us();
	if(fd) {
		sprintf(buf, "us:\t%ld\tmicro second\n", (us - pre_us));
		if(write(fd, buf, strlen(buf) + 1) < 0)
			printf("File Write error\n");
	}
	printf("us:\t%ld\tmicro second\n", (us - pre_us));
}
#endif
