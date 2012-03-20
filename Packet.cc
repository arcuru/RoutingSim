#include "Global.h"


Packet::Packet ( Address destination, Address origin, size_t packet_size )
{
	// Make sure all data fits into packet
	assert(destination.x < 0x10);
	assert(destination.y < 0x10);
	assert(origin.x < 0x10);
	assert(origin.y < 0x10);
	//info = data;
	//info <<= 32;
	info = 0;
	info |= destination.x;
	info |= destination.y << 4;
	info |= origin.x << 8;
	info |= origin.y << 12;
	//info |= (head ? 1 : 0) << 16;

	// Make sure all retrieval methods work
	assert(GetX() == destination.x);
	assert(GetY() == destination.y);
	assert(GetOriginX() == origin.x);
	assert(GetOriginY() == origin.y);
	//assert(GetHead() == head);
	//assert(GetData() == data);
	

	flit_count = packet_size / 4;
	assert( packet_size%4 == 0 );
	//flits = (Flit*)malloc(sizeof(Flit)*(flit_count));
	flits = new Flit[flit_count];

	flits[0].setPacket( this );
	flits[0].setHead( true );
	for (size_t i=1; i < flit_count; i++) {
		flits[i].setPacket( this );
		flits[i].setHead( false );
	}
	created = Global_Time;
	route_pointer = 0;
}

Packet::~Packet ()
{
	delete [] flits;
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
	assert( flit_count == 16 );
	return flit_count;
}

/** GetX
 *  returns the value of the destination x address stored in the system
 *
 */
uint8_t Packet::GetX ( ) const
{
	return info & 0xF;
}

/** GetY
 *  returns the value of the destination y address stored in the system
 *
 */
uint8_t Packet::GetY ( ) const
{
	return (info >> 4) & 0xF;
}

/** GetOriginX
 *  returns the value of the originating x address stored in the system
 *
 */
uint32_t Packet::GetOriginX ( ) const
{
	return (info >> 8) & 0xF;
}

/** GetOriginY
 *  returns the value of the originating y address stored in the system
 *
 */
uint32_t Packet::GetOriginY ( ) const
{
	return (info >> 12) & 0xF;
}

/** GetHead
 *  returns true or false depending on if this packet is a head packet
 *
 */
bool Packet::GetHead ( ) const
{
	if ( (info >> 16) & 0x1 )
		return true;
	return false;
}

/** GetData
 *  returns the data stored in this packet
 *
 */
uint32_t Packet::GetData ( ) const
{
	return info >> 32;
}

/** GetCreated
 *  returns the creation time of this packet
 *
 */
uint32_t Packet::GetCreated ( ) const
{
	return created;
}

/** AddRouter
 *  adds a router to the list of routers we have traversed through
 *
 *  @arg addr Address of a router
 */
void Packet::AddRouter ( Address addr )
{
	assert( route_pointer < 16 );
	assert( addr.x < NInfo.width );
	assert( addr.y < NInfo.height );
	route[route_pointer] = addr;
	route_pointer++;
}

