#include "Global.h"


Packet::Packet ( Address destination, Address origin, bool head, bool credit, uint32_t data)
{
	// Make sure all data fits into packet
	assert(destination.x < 0x10);
	assert(destination.y < 0x10);
	assert(origin.x < 0x10);
	assert(origin.y < 0x10);
	info = data;
	info <<= 32;
	info |= destination.x;
	info |= destination.y << 4;
	info |= origin.x << 8;
	info |= origin.y << 12;
	info |= (head ? 1 : 0) << 16;
	info |= (credit ? 1 : 0) << 17;

	// Make sure all retrieval methods work
	assert(GetX() == destination.x);
	assert(GetY() == destination.y);
	assert(GetOriginX() == origin.x);
	assert(GetOriginY() == origin.y);
	assert(GetHead() == head);
	assert(GetCredit() == credit);
	assert(GetData() == data);
}

Packet::~Packet ()
{
}

/** GetX
 *  returns the value of the destination x address stored in the system
 *
 */
uint8_t Packet::GetX ( )
{
	return info & 0xF;
}

/** GetY
 *  returns the value of the destination y address stored in the system
 *
 */
uint8_t Packet::GetY ( )
{
	return (info >> 4) & 0xF;
}

/** GetOriginX
 *  returns the value of the originating x address stored in the system
 *
 */
uint8_t Packet::GetOriginX ( )
{
	return (info >> 8) & 0xF;
}

/** GetOriginY
 *  returns the value of the originating y address stored in the system
 *
 */
uint8_t Packet::GetOriginY ( )
{
	return (info >> 12) & 0xF;
}

/** GetHead
 *  returns true or false depending on if this packet is a head packet
 *
 */
bool Packet::GetHead ( )
{
	if ( (info >> 16) & 0x1 )
		return true;
	return false;
}

/** GetCredit
 *  returns true or false depending on if this packet is a credit
 *
 */
bool Packet::GetCredit ( )
{
	if ( (info >> 17) & 0x1 )
		return true;
	return false;
}

/** GetData
 *  returns the data stored in this packet
 *
 */
uint32_t Packet::GetData ( )
{
	return info >> 32;
}

