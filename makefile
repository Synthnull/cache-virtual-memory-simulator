CC = gcc
CFLAGS = -g -Wall -pedantic
TARGET = cache_simulator.exe
SOURCES = main.c cmd_parser.c print.c memory_calculations.c cache_calculations.c page_table.c trace_parser.c virtual_memory_simulator.c cache_simulator.c sim_runner.c cpu_cache.c address_parser.c cache_replace.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)
	
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

$(OBJECTS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean: 
	rm -f $(TARGET) $(OBJECTS)
