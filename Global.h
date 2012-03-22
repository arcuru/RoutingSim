#ifndef GLOBAL_H
#define GLOBAL_H

// Uncomment to disable assert's
//#define NDEBUG

#include <cstdint>
#include <cassert>
#include <iostream>
using namespace std;
#include <cstdlib>

#define END_TIME 1000

extern uint64_t Global_Time; //!< Current time step

typedef enum {
	NORTH   = 0,
	SOUTH   = 1,
	EAST    = 2,
	WEST    = 3,
	HERE    = 4, //!< Indicates current router. VALUE MUST BE 4 FOR Router::Process TO WORK PROPERLY.
	INVALID = 5
} Direction; //!< Cardinal direction for use in 2D Mesh

typedef struct {
	uint8_t x; //!< X address in mesh
	uint8_t y; //!< Y address in mesh
} Address; //!< Address in 2D mesh

typedef enum EventType {
	DATA,   //!< Data arrival
	CREDIT  //!< Credit arrival for flow control
} EventType; //!< Used to specify type of event

typedef struct Event {
	EventType t;  //!< Event type
	void* d;      //!< Data to be delivered
	void* o;      //!< Origin of event
} Event; //!< Event definition

typedef enum {
	RAND,
	BIT_REV,
	BIT_COMP
} Destination_Function; //!< Function for generating destinations

typedef struct {
	uint8_t width;  //!< Width of network (X max + 1)
	uint8_t height; //!< Height of network (Y max + 1)
	double chance;  //!< Chance of packet injection every cycle
	Destination_Function dest_func;
} NetworkInfo;

extern NetworkInfo NInfo;

#include "Flit.h"
#include "Packet.h"
#include "EventTarget.h"
#include "EventQueue.h"
#include "VirtualChannel.h"
#include "OutputChannel.h"
#include "InputChannel.h"
#include "OutputBuffer.h"
#include "InputBuffer.h"
#include "RouteComputation.h"
#include "PacketGen.h"
#include "Router.h"

extern EventQueue Global_Queue; //!< Single global queue handling all events

extern uint64_t packet_injections;
extern uint64_t packet_ejections;
extern uint64_t packets_blocked;
extern uint64_t packets_sent;
extern uint64_t packet_latency;
extern uint64_t link_util;
extern uint64_t aibuf_util;
extern uint64_t eibuf_util;
extern uint64_t aobuf_util;
extern uint64_t eobuf_util;

#endif
