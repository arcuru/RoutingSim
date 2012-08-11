#include "Global.h"


Packet::Packet ( Address destination, Address origin, size_t packet_size )
{
	// Make sure all data fits into header for 8x8 specific torus
	assert( (destination.x != origin.x) || (destination.y != origin.y) );
	assert(destination.x < 0x8);
	assert(destination.y < 0x8);
	assert(origin.x < 0x8);
	assert(origin.y < 0x8);
	uint32_t header = 0;
	header |= destination.x;
	header |= destination.y << 3;
	header |= origin.x << 6;
	header |= origin.y << 9;

	// Compute and allocate space for all flits
	flit_count = packet_size / 4;
	assert( packet_size%4 == 0 );
	flits = new Flit[flit_count];

	// Save flit_count into header
	assert( flit_count < 0x100 );
	header |= flit_count << 12;

	// Save data into each flit
	flits[0].setPacket( this );
	flits[0].setHead( true );
	for (size_t i=1; i < flit_count; i++) {
		flits[i].setPacket( this );
		flits[i].setHead( false );
	}
	created = Global_Time;

	header |= CalcHash() << 20;
	flits[0].setData( header );

	// Make sure all retrieval methods work
	assert( GetX() == destination.x );
	assert( GetY() == destination.y );
	assert( GetOriginX() == origin.x );
	assert( GetOriginY() == origin.y );
	assert( GetSize() == flit_count );
	assert( GetHash() == CalcHash() );

#ifndef NDEBUG
	route_pointer = 0;
#endif
}

Packet::~Packet ()
{
	assert( GetHash() == CalcHash() );
	// Too slow
	//delete [] flits;
}

/** GetFlit
 *  returns a pointer to the specified flit
 *
 *  @arg index  Index of the flit to return
 *  @return Pointer to specified flit
 */
Flit* Packet::GetFlit ( size_t index ) const
{
	return &flits[index];
}

/** GetSize
 *  returns the size of the packet in flits
 */
size_t Packet::GetSize ( ) const
{
	return flit_count;
}

/** GetX
 *  returns the value of the destination x address stored in the system
 *
 */
uint8_t Packet::GetX ( ) const
{
	return flits[0].getData() & 0x7;
}

/** GetY
 *  returns the value of the destination y address stored in the system
 *
 */
uint8_t Packet::GetY ( ) const
{
	return (flits[0].getData() >> 3) & 0x7;
}

/** GetOriginX
 *  returns the value of the originating x address stored in the system
 *
 */
uint32_t Packet::GetOriginX ( ) const
{
	return (flits[0].getData() >> 6) & 0x7;
}

/** GetOriginY
 *  returns the value of the originating y address stored in the system
 *
 */
uint32_t Packet::GetOriginY ( ) const
{
	return (flits[0].getData() >> 9) & 0x7;
}

/** GetHash
 *  returns the saved hash value of the data stored in the flits
 *
 */
uint8_t Packet::GetHash ( ) const
{
	return (flits[0].getData() >> 20) & 0xFF;
}

/** GetCreated
 *  returns the creation time of this packet
 *
 */
uint32_t Packet::GetCreated ( ) const
{
	return created;
}

/** SetCreated
 *  Sets the creation time of this packet
 *
 *  @arg creation New creation time
 */
void Packet::SetCreated ( uint32_t creation )
{
	created = creation;
}

#ifndef NDEBUG
/** AddRouter
 *  adds a router to the list of routers we have traversed through
 *
 *  @arg addr Address of a router
 */
void Packet::AddRouter ( Address addr )
{
	assert( addr.x < NInfo.width );
	assert( addr.y < NInfo.height );
	route[route_pointer] = addr;
	route_pointer++;
}
#endif

/** ComputeHash
 *  calculates the hash value from the data stored in each flit
 *
 */
uint8_t Packet::CalcHash ( ) const
{
	uint8_t hash = 0;
	for (size_t i=1; i < GetSize(); i++) {
		hash ^= (flits[i].getData() & 0xFF);
		hash ^= ( (flits[i].getData() >> 8) & 0xFF);
		hash ^= ( (flits[i].getData() >> 16) & 0xFF);
		hash ^= ( (flits[i].getData() >> 24) & 0xFF);
	}
	return hash;
}

