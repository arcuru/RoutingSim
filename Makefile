CXX=clang++
COMMONFLAGS=-std=c++1z -Weverything -Werror -flto -O3
CFLAGS=$(COMMONFLAGS) -Wno-c++98-compat -Wno-padded -Wno-weak-vtables -Wno-old-style-cast -Wno-exit-time-destructors -Wno-global-constructors
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
	$(CXX) -o $(TARGET) $(OBJECTS) $(CFLAGS)

%.o: %.cc $(HEADERS)
	$(CXX) -c -o $@ $< $(INCLUDE_DIR) $(CFLAGS)

clean:
	rm -rf $(OBJECTS) $(TARGET)

# Testing is out of date
test: all
	./$(TARGET) 1 1000 > rand.csv && diff rand.csv saved_rand.csv > diff_rand.txt && echo "Finished rand." &
	./$(TARGET) 2 1000 > bit_rev.csv && diff bit_rev.csv saved_bit_rev.csv > diff_rev.txt && echo "Finished bit reverse." &
	./$(TARGET) 3 1000 > bit_comp.csv && diff bit_comp.csv saved_bit_comp.csv > diff_comp.txt && echo "Finished bit complement." &

