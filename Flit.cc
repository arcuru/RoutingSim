#include "Global.h"

Flit::Flit( )
{
	origin = nullptr;
	head = false;
	data = 0;
}

Flit::Flit( Packet* p, bool header, uint32_t d )
{
	origin = p;
	head = header;
	data = d;
}

Flit::~Flit ()
{
}

void Flit::setPacket( Packet* p )
{
	origin = p;
}

void Flit::setHead( bool header )
{
	head = header;
}

void Flit::setData ( uint32_t d )
{
	data = d;
}

Packet* Flit::getPacket() const
{
	return origin;
}

bool Flit::isHead() const
{
	return head;
}

uint32_t Flit::getData ( ) const
{
	return data;
}

