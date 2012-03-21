#include "Global.h"

PacketGen::PacketGen ( )
{
	ibuf = new InputBuffer(32);
	obuf = new OutputBuffer(32);
	packets_out = 0;
	flits_received = 0;
	saved_p = NULL;
}

PacketGen::PacketGen ( Address setAddress )
{
	SetAddr( setAddress );
	ibuf = new InputBuffer(32);
	obuf = new OutputBuffer(32);
	packets_out = 0;
	flits_received = 0;
	saved_p = NULL;
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
	assert( NULL == saved_p );
	// Generate destination address as a random address in the network
	Address dest = addr;
	switch ( NInfo.dest_func ) {
		case RAND:	
			while ( (dest.x == addr.x) && (dest.y == addr.y) ) {
				dest.x = rand() % NInfo.width;
				dest.y = rand() % NInfo.height;
			}
			break;

		case BIT_REVERSE:	
			break;

		case BIT_COMP:	
			dest.x = NInfo.width - addr.x - 1;
			dest.y = NInfo.height - addr.y - 1;
			break;

		default:	
			break;
	}

	if ( (dest.x == addr.x) && (dest.y == addr.y) )
		return;

	// Generate packet and load appropriate data
	saved_p = new Packet( dest, addr, 64 );

	return ;
}

/** RandomGenPacket
 *  generate a packet based on random chance
 *
 *  @arg chances  Chance of generating a packet
 */
void PacketGen::RandomGenPacket ( double chances )
{
	if ( NULL == saved_p ) {
		if (rand() < (chances * (double)RAND_MAX)) {
			packet_injections++;
			InputChannel* ic = ibuf->getIC( 0 );
			if ( ic->FlitsRemaining() <= ic->Size() - 16 ) {
				GenPacket();
			}
			else
				packets_blocked++;
		}
	}
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
			packet_latency += Global_Time - p->GetCreated();
			delete p;
		}
	}
	// Insert waiting packet into injection queue
	InputChannel* ic = ibuf->getIC( 0 );
	if ( NULL != saved_p && ic->FlitsRemaining() == 0 ) {
		ic->InsertPacket( saved_p );
		ic->schedRC();
		packets_sent++;
		saved_p = NULL;
	}
	return ;
}

