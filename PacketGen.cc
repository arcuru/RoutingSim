#include "Global.h"

PacketGen::PacketGen ( )
{
	ibuf = new InputBuffer(32);
	obuf = new OutputBuffer(32);
	packets_out = 0;
	packets_sent = 0;
	packets_blocked = 0;
}

PacketGen::PacketGen ( Address setAddress )
{
	SetAddr( setAddress );
	ibuf = new InputBuffer(32);
	obuf = new OutputBuffer(32);
	packets_out = 0;
	packets_sent = 0;
	packets_blocked = 0;
}

PacketGen::~PacketGen ()
{
	delete ibuf;
	delete obuf;
}

void PacketGen::SetAddr ( Address setAddress )
{
	addr = setAddress;
}

/** SetDirection
 *  saves the direction this generator is pointing for valid
 *  generation of destination coords
 *
 *  @arg edge  Direction this packet generator is pointing
 */
void PacketGen::SetDirection ( Direction edge )
{
	dir = edge;
	return ;
}

/** GetInjection
 *  returns the injection queue for this packet generator
 *
 *  @return   Target injection queue
 */
InputBuffer* PacketGen::GetInjection ( ) const
{
	return ibuf;
}

/** GetEjection
 *  returns the ejection queue for this packet generator
 *
 *  @return   Target ejection queue
 */
OutputBuffer* PacketGen::GetEjection ( ) const
{
	return obuf;
}

/** GenPacket
 *  creates a packet with a valid address based on saved generator
 *  address and direction. We then add it to the OutputBuffer.
 *
 */
void PacketGen::GenPacket ( )
{
	// Generate destination address as a random address in the network
	Address dest = addr;
	while ( (dest.x == addr.x) && (dest.y == addr.y) ) {
		dest.x = rand() % NetworkInfo.width;
		dest.y = rand() % NetworkInfo.height;
	}

	// Generate packet and load appropriate data
	Packet* p = new Packet( dest, addr, true, rand() & 0xFFFFFFFF);

	// Add packet to output buffer
	packet_injections++;
	if (ibuf->PacketsRemaining() < 32) {
		ibuf->InsertPacket(p);
		packets_sent++;
	}
	else {
	//	cout << "PacketGen OutputBuffer full" << endl;
		packets_blocked++;
	}
	return ;
}

/** RandomGenPacket
 *  generate a packet based on random chance
 *
 *  @arg chances  Chance of generating a packet
 */
void PacketGen::RandomGenPacket ( double chances )
{
	if (rand() < (chances * (double)RAND_MAX))
		GenPacket();
	return ;
}

/** Process
 *  handles all necessary buffer operations for a single time step
 *
 */
void PacketGen::Process ( )
{
	if (obuf->PacketsRemaining() != 0) {
		assert(1 == obuf->PacketsRemaining());
		Packet* p = obuf->GetPacket();
		assert(p->GetX() == addr.x);
		assert(p->GetY() == addr.y);
		obuf->PopPacket();
		delete p;
		packets_out++;
		packet_ejections++;
	}
	return ;
}



