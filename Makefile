CC=clang++
CFLAGS=-std=c++0x -Wall -Wextra -Werror -pedantic -O4
INCLUDE_DIR=
LIBS=
HEADERS=Global.h
SOURCES=Flit.cc \
		Packet.cc \
		EventQueue.cc \
		VirtualChannel.cc \
		OutputChannel.cc \
		InputChannel.cc \
		InputBuffer.cc \
		OutputBuffer.cc \
		RouteComputation.cc \
		Router.cc \
		PacketGen.cc \
		main.cc
ASOBJECTS=$(ASSOURCES:.s=.o)
OBJECTS=$(SOURCES:.cc=.o)
TARGET=RoutingSim

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS)

%.o: %.cc $(HEADERS)
	$(CC) -c -o $@ $< $(INCLUDE_DIR) $(CFLAGS)

clean:
	rm -rf $(OBJECTS) $(TARGET)

test: all
	time ./$(TARGET) 0 1000 > rand.csv
	time ./$(TARGET) 1 1000 > bit_rev.csv
	time ./$(TARGET) 2 1000 > bit_comp.csv
	diff rand.csv saved_rand.csv
	diff bit_rev.csv saved_bit_rev.csv
	diff bit_comp.csv saved_bit_comp.csv

