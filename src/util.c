#include <string.h>

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
