CC = gcc
CFLAGS = -g -Wall -pedantic
TARGET = cache_simulator.exe
SOURCES = main.c cmd_parser.c print.c memory_calculations.c cache_calculations.c page_table.c trace_parser.c virtual_memory_simulator.c vm_runner.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)
	
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

$(OBJECTS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean: 
	rm -f $(TARGET) $(OBJECTS)
