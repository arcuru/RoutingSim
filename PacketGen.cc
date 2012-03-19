#include "Global.h"

PacketGen::PacketGen ( )
{
	ibuf = new InputBuffer(32);
	obuf = new OutputBuffer(32);
	ibuf->getVC( 0 )->setType( VC_INJ );
	obuf->getVC( 0 )->setType( VC_EJ );
	packets_out = 0;
	packets_sent = 0;
	packets_blocked = 0;
}

PacketGen::PacketGen ( Address setAddress )
{
	SetAddr( setAddress );
	ibuf = new InputBuffer(32);
	obuf = new OutputBuffer(32);
	ibuf->getVC( 0 )->setType( VC_INJ );
	obuf->getVC( 0 )->setType( VC_EJ );
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
		switch ( NInfo.dest_func ) {
			case RAND:	
				dest.x = rand() % NInfo.width;
				dest.y = rand() % NInfo.height;
				break;

			case BIT_REVERSE:	
				break;

			case BIT_COMP:	
				break;

			case CROSS:	
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
	VirtualChannel* vc = ibuf->getVC( 0 );
	packet_injections++;
	if ( p->GetSize() <= vc->Size() - vc->FlitsRemaining() ) {
		if ( vc->FlitsRemaining() == 0) { // For now only insert to empty buffer
			for (size_t i = 0; i < p->GetSize(); i++) {
				Event e = { DATA, p->GetFlit( i ), this };
				vc->ProcessEvent( e ); // Don't wait before inserting
			}
			packets_sent++;
		}
		else
			packets_blocked++;
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
	VirtualChannel* vc = obuf->getVC( 0 );
	if ( vc->FlitsRemaining() != 0 ) {
		assert( 1 == vc->FlitsRemaining() );
		Packet* p = vc->GetPacket();
		assert( p->GetX() == addr.x );
		assert( p->GetY() == addr.y );
		vc->PopFlit();
		packets_out++;
		packet_ejections++;
	}
	return ;
}



