#include "Global.h"

Buffer::Buffer ()
{
	buf_index = 0;
	buf_valid = SIZE_MAX;
	buf_size = 4;
	buf = (Flit**) malloc(sizeof(Flit*) * buf_size);
}

Buffer::Buffer ( size_t entries )
{
	buf_index = 0;
	buf_valid = SIZE_MAX;
	buf_size = entries;
	buf = (Flit**) malloc(sizeof(Flit*) * buf_size);
}

Buffer::~Buffer ()
{
	free(buf);
}

/** Size
 *  returns the size of the buffer
 *
 *  @return Size of buffer
 */
size_t Buffer::Size ( ) const
{
	return buf_size;
}

/** ProcessEvent
 *  handles an incoming flit by adding it to the buffer
 *
 *  @arg e  Incoming event
 */
void Buffer::ProcessEvent ( Event e )
{
	assert( DATA == e.t );
	InsertFlit( (Flit*)e.d );
}

/** InsertFlit
 *  handles an incoming flit by adding it to the buffer
 *
 *  @arg p  Flit to add into buffer
 */
void Buffer::InsertFlit ( Flit* p )
{
	assert( buf_index != buf_valid ); // Buffer full
	assert( buf_index < buf_size ); // Buffer indexing invalid
	// Below not valid for injection queue
//	assert( p->isHead() == false || FlitsRemaining() == 0 );

	if ( FlitsRemaining() == 0 && p->isHead() ) {
		current = p->getPacket();
		target = INVALID;
	}
	assert( current = p->getPacket() );

	buf[buf_index] = p;
	if ( buf_valid >= buf_size )
		buf_valid = buf_index;
	buf_index++;
	buf_index %= buf_size;
	return ;
}

/** InsertPacket
 *  handles an incoming packet by adding it to the buffer
 *
 *  @arg p  Packet to add into buffer.
 */
void Buffer::InsertPacket ( Packet* p )
{
	assert( p->GetSize() <= buf_size - FlitsRemaining() );
	for (size_t i = 0; i < p->GetSize(); i++)
		InsertFlit( p->GetFlit(i) );
}

/** PopFlit
 *  delete the oldest flit in buffer
 *
 */
void Buffer::PopFlit ( )
{
	assert( buf_valid < buf_size ); // No valid flits
	buf_valid++;
	buf_valid %= buf_size;
	if ( buf_valid == buf_index ) {
		buf_valid = SIZE_MAX;
	}
	return ;
}

/** GetFlit
 *  retrieve oldest flit in buffer
 *
 *  @return Oldest flit in buffer
 */
Flit* Buffer::GetFlit ( ) const
{
	assert( buf_valid < buf_size ); // No valid flits
	return buf[buf_valid];
}

/** FlitsRemaining
 *  returns the number of flits stored in the buffer
 *
 *  @return Number of flits in the buffer
 */
size_t Buffer::FlitsRemaining ( ) const
{
	if ( buf_valid >= buf_size )
		return 0;
	size_t tmp = buf_index;
	if ( tmp <= buf_valid )
		tmp += buf_size;
	return tmp - buf_valid;
}

/** SetTarget
 *  sets the route for the current packet
 *
 *  @arg t Direction for the current packet to travel
 */
void Buffer::SetTarget ( Direction t )
{
	target = t;
}

/** GetTarget
 *  returns the current routing direction
 *
 *  @return Direction of travel for current packet
 */
Direction Buffer::GetTarget ( ) const
{
	return target;
}

/** GetPacket
 *  returns the current packet being routed
 *
 *  @return Packet currently in buffer
 */
Packet* Buffer::GetPacket ( ) const
{
	return current;
}

