#include "Global.h"

PacketGen::PacketGen ( )
{
	ibuf = new InputBuffer(32);
	obuf = new OutputBuffer(32);
	packets_out = 0;
	packets_sent = 0;
	packets_blocked = 0;
	flits_received = 0;
}

PacketGen::PacketGen ( Address setAddress )
{
	SetAddr( setAddress );
	ibuf = new InputBuffer(32);
	obuf = new OutputBuffer(32);
	packets_out = 0;
	packets_sent = 0;
	packets_blocked = 0;
	flits_received = 0;
}

PacketGen::~PacketGen ()
{
	OutputChannel* vc = obuf->getOC( 0 );
	if ( vc->GetPacket() != NULL )
		delete vc->GetPacket();
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
		switch ( NInfo.dest_func ) {
			case RAND:	
				dest.x = rand() % NInfo.width;
				dest.y = rand() % NInfo.height;
				break;

			case BIT_REVERSE:	
				break;

			case BIT_COMP:	
				dest.x = NInfo.width - 1 - addr.x;
				dest.y = NInfo.height - 1 - addr.y;
				break;

			default:	
				break;
		}
	}

	// Generate packet and load appropriate data
	Packet* p = new Packet( dest, addr, 64 );

	// Add packet to output buffer
	InputChannel* vc = ibuf->getIC( 0 );
	packet_injections++;
	if ( p->GetSize() <= vc->Size() - vc->FlitsRemaining() ) {
		if ( vc->FlitsRemaining() == 0) { // For now only insert to empty buffer
			vc->InsertPacket( p );
			vc->schedRC();
			packets_sent++;
		}
		else {
			packets_blocked++;
			delete p;
		}
	}
	else {
	//	cout << "PacketGen OutputBuffer full" << endl;
		packets_blocked++;
		delete p;
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
	OutputChannel* vc = obuf->getOC( 0 );
	if ( vc->FlitsRemaining() != 0 ) {
		if ( vc->GetPacket()->GetSize() == vc->FlitsRemaining() ) {
			Packet* p = vc->GetPacket();
			assert( p->GetX() == addr.x );
			assert( p->GetY() == addr.y );
			//cout << "Got full packet at (" << addr.x << ", " << addr.y << ")" << endl;
			while ( vc->FlitsRemaining() != 0 )
				vc->sendFlit();
			assert( vc->FlitsRemaining() == 0 );
			packets_out++;
			packet_ejections++;
			delete p;
		}
	}
	return ;
}

