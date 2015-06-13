#include "Global.h"
#include <cstdlib>

PacketGen::PacketGen ( )
{
	ibuf = new InputBuffer(32);
	obuf = new OutputBuffer(32);
	packets_out = 0;
	flits_received = 0;
	saved_p = nullptr;
	memcontroller = false;
	dir = INVALID;
}

PacketGen::PacketGen ( Address setAddress )
{
	this->SetAddr( setAddress );
	ibuf = new InputBuffer(32);
	obuf = new OutputBuffer(32);
	packets_out = 0;
	flits_received = 0;
	saved_p = nullptr;
	memcontroller = false;
	dir = INVALID;
}

PacketGen::~PacketGen ()
{
	delete ibuf;
	delete obuf;
}

void PacketGen::SetAddr ( Address setAddress )
{
	addr = setAddress;
	memcontroller = false;
	if (NInfo.memcont) {
		for (size_t i = 0; i < sizeof(MC)/sizeof(MC[0]); i++) {
			if ( (MC[i].x == addr.x) && (MC[i].y == addr.y) )
				memcontroller = true;
		}
	}
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

/** InsertPacket
 *  Places a packet with the given address into the saved location. Returns true
 *  if successful, false otherwise
 *
 *  @arg dest Destination of packet
 *  @arg packet_size Size of packet to send in bytes
 *  @return Pointer to generated packet, nullptr if not generated
 */
Packet* PacketGen::InsertPacket ( Address dest, size_t packet_size )
{
	// Shouldn't try to insert a packet to the current address
	assert( (dest.x != addr.x) || (dest.y != addr.y));

	// Return false if full
	if ( nullptr != saved_p )
		return nullptr;
	saved_p = new Packet( dest, addr, packet_size );
	return saved_p;
}

/** GenPacket
 *  creates a packet with a valid address based on saved generator
 *  address and direction. We then add it to the OutputBuffer.
 *
 */
void PacketGen::GenPacket ( )
{
	assert( nullptr == saved_p );
	assert( false == memcontroller ); // Don't generate packet if memcontroller
	uint8_t tmp[] = { 0x0, 0x4, 0x2, 0x6, 0x1, 0x5, 0x3, 0x7 }; // Lookup table for 3bit BIT_REV

	// Generate destination address based on the destination function of the network
	Address dest = addr;
	switch ( NInfo.dest_func ) {
		// Random generation that will never return the current router
		case RAND:	
			while ( (dest.x == addr.x) && (dest.y == addr.y) ) {
				dest.x = rand() % NInfo.width;
				dest.y = rand() % NInfo.height;
			}
			break;

		// Bit reverse
		case BIT_REV:
			assert( NInfo.width <= 8 && NInfo.width > 4 ); // Only valid for 3 bytes
			assert( NInfo.height <= 8 && NInfo.height > 4 );
			dest.x = tmp[addr.x];
			dest.y = tmp[addr.y];
			break;

		// Bit compose
		case BIT_COMP:	
			dest.x = NInfo.width - addr.x - 1;
			dest.y = NInfo.height - addr.y - 1;
			break;

		// Memory controller lookup
		// Chooses a random memory controller from the memory controller addresses
		case MEM_CONT:
			dest = MC[ rand() % (sizeof(MC)/sizeof(MC[0])) ];
			break;

		default:	
			assert( false );
			break;
	}

	if ( (dest.x == addr.x) && (dest.y == addr.y) )
		return;

	// Generate packet and load appropriate data
#ifdef NDEBUG
	this->InsertPacket( dest, 12 );
#else
	Packet* p = InsertPacket( dest, 12 );
	assert( nullptr != p );
#endif

	// Count this as an injection
	packet_injections++;
	return ;
}

/** RandomGenPacket
 *  generate a packet based on random chance
 *
 *  @arg chances  Chance of generating a packet
 */
void PacketGen::RandomGenPacket ( double chances )
{
	// Don't generate packet if memcontroller
	if ( memcontroller )
		return;
	if ( nullptr == saved_p ) {
		if (rand() < (chances * (double)RAND_MAX)) {
			InputChannel* ic = ibuf->getIC( 0 );
			if ( ic->FlitsRemaining() <= ic->Size() - 3 ) {
				GenPacket();
			}
			else {
				packets_blocked++;
				packet_injections++; // Attempted injection
			}
		}
	}
	else if ( saved_p->GetCreated() < Global_Time - 3 ) // Increment if this packet had the chance to leave by now.
		if (rand() < (chances * (double)RAND_MAX)) // And if we would have passed our chance test
			packet_injections++;
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

			// Different functionality if this is a memory controller
			if ( memcontroller ) {
				Address dest;
				dest.x = p->GetOriginX();
				dest.y = p->GetOriginY();
				Packet* pp = NArray2[IND(addr.x,addr.y)].GetPacketGen()->InsertPacket( dest, 68 );
				if ( nullptr != pp ) {
					while ( vc->FlitsRemaining() != 0 )
						vc->sendFlit();
					assert( vc->FlitsRemaining() == 0 );
					pp->SetCreated( p->GetCreated() );
					delete p;
				}
			}
			else {
				while ( vc->FlitsRemaining() != 0 )
					vc->sendFlit();
				assert( vc->FlitsRemaining() == 0 );
				//assert( p->GetSize() == 17 ); Only true if NInfo.memcont is
				packets_out++;
				packet_ejections++;
				packet_latency += Global_Time - p->GetCreated();
				delete p;
			}
		}
	}
	// Insert waiting packet into injection queue
	InputChannel* ic = ibuf->getIC( 0 );
	if ( nullptr != saved_p && ic->FlitsRemaining() == 0 ) {
		ic->InsertPacket( saved_p );
		ic->schedRC();
		packets_sent++;
		saved_p = nullptr;
	}
	return ;
}

