#ifndef GLOBAL_H
#define GLOBAL_H

// Uncomment to disable assert's
//#define NDEBUG

#include <cstdint>
#include <cassert>
#include <iostream>
using namespace std;
#include <cstdlib>

#define END_TIME 1000000

extern uint32_t Global_Time; //!< Current time step

enum direction {
	NORTH = 0,
	SOUTH = 1,
	EAST  = 2,
	WEST  = 3
}; //!< Cardinal direction for use in 2D Mesh

typedef enum direction Direction;

struct address {
	uint8_t x; //!< X address in mesh
	uint8_t y; //!< Y address in mesh
}; //!< Address in 2D mesh

typedef struct address Address;

typedef enum EventType {
	DATA,   //!< Data arrival
	CREDIT  //!< Credit arrival for flow control
} EventType; //!< Used to specify type of event

typedef struct Event {
	EventType t;  //!< Event type
	void* d;      //!< Data to be delivered
} Event; //!< Event definition

#include "Packet.h"
#include "EventTarget.h"
#include "EventQueue.h"
#include "OutputBuffer.h"
#include "InputBuffer.h"
#include "PacketGen.h"
#include "Router.h"

extern EventQueue Global_Queue; //!< Single global queue handling all events

extern uint32_t packet_injections;
extern uint32_t packet_ejections;

#endif
