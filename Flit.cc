#include "Global.h"

Flit::Flit( )
{
}

Flit::Flit( Packet* p, bool header)
{
	origin = p;
	head = header;
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

Packet* Flit::getPacket() const
{
	return origin;
}

bool Flit::isHead() const
{
	return head;
}

