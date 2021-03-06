OBJECTS = $(patsubst %.cpp,%.o, $(shell find . -name *.cpp)) $(patsubst %.c,%.o, $(shell find . -name *.c))
CC = g++
CFLAGS = -DPC_PORT -c -I lib -I src -I/opt/devkitpro/libogc/include/ -I/opt/devkitpro/libogc/include/ogc -Wall -std=c++2a -g -O3 -fno-exceptions

LDFLAGS = 

OUT_FILE = app.elf

.PHONY: build
build: $(OBJECTS)
	$(CC) -o $(OUT_FILE) $(OBJECTS) $(LDFLAGS)

run: build
	./$(OUT_FILE)

%.o: %.cpp
	$(CC) $(CFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm $(OBJECTS) $(OUT_FILE)
