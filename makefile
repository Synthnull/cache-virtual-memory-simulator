CC = gcc
CFLAGS = -g -Wall
TARGET = cache_simulator.exe
SOURCES = main.c cmd_parser.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)
	
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

$(OBJECTS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean: 
	rm -f $(TARGET) $(OBJECTS)
