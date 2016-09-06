.PHONY: run all clean

CFLAGS = -I ../../include -O2 -g -Wall -Werror -m64 -ffreestanding -fno-stack-protector -std=gnu99
CFLAGS += $(shell pkg-config --cflags json-c)

DIR = obj

OBJS = obj/main.o obj/dup.o

LIBS = ../../lib/libpacketngin.a
LIBS += $(shell pkg--config --libs josn-c)

all: $(OBJS)
	ld -melf_x86_64 -nostdlib -e main -o main $^ $(LIBS)

obj/%.o: src/%.c
	mkdir -p $(DIR)
	gcc $(CFLAGS) -c -o $@ $<

clean:
	rm -rf obj
	rm -f main

run: all
	../../bin/console script

install:
	git clone https://github.com/json-c/json-c.git
