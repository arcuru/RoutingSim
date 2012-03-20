#include "Global.h"

VirtualChannel::VirtualChannel ( )
{
	buf_index = 0;
	buf_valid = SIZE_MAX;
	buf_size = 4;
	buf = (Flit**) malloc(sizeof(Flit*) * buf_size);
	cur_packet = NULL;
}

VirtualChannel::VirtualChannel ( size_t entries )
{
	buf_index = 0;
	buf_valid = SIZE_MAX;
	buf_size = entries;
	buf = (Flit**) malloc(sizeof(Flit*) * buf_size);
}

VirtualChannel::~VirtualChannel ()
{
	free(buf);
}

/** setSize
 *  sets the size of the VC. Can only be run with nothing in the VC
 *
 *  @arg entries New size of VC buffer
 */
void VirtualChannel::setSize ( size_t entries )
{
	// Check to make sure we are't changing it after saving things into the buffer
	assert( 0 == buf_index );
	assert( SIZE_MAX == buf_valid );

	buf_size = entries;
	free( buf );
	buf = (Flit**) malloc(sizeof(Flit*) * buf_size);
}

/** Size
 *  returns the size of the virtual channel
 *
 *  @return Size of virtual channel
 */
size_t VirtualChannel::Size ( ) const
{
	return buf_size;
}

/** InsertFlit
 *  handles an incoming flit by adding it to the virtual channel
 *
 *  @arg p  Flit to add into virtual channel
 */
void VirtualChannel::InsertFlit ( Flit* p )
{
	assert( buf_index != buf_valid ); // VirtualChannel full
	assert( buf_index < buf_size ); // VirtualChannel indexing invalid

	if ( p->isHead() ) {
		cur_packet = p->getPacket();
	}

	buf[buf_index] = p;
	if ( buf_valid >= buf_size )
		buf_valid = buf_index;
	buf_index++;
	buf_index %= buf_size;
	return ;
}

/** InsertPacket
 *  handles an incoming packet by adding it to the virtual channel
 *
 *  @arg p  Packet to add into virtual channel.
 */
void VirtualChannel::InsertPacket ( Packet* p )
{
	assert( p->GetSize() <= buf_size - FlitsRemaining() );
	for (size_t i = 0; i < p->GetSize(); i++)
		InsertFlit( p->GetFlit(i) );
}

/** PopFlit
 *  delete the oldest flit in virtual channel
 *
 */
void VirtualChannel::PopFlit ( )
{
	assert( buf_valid < buf_size ); // No valid flits
	buf[buf_valid] = NULL;
	buf_valid++;
	buf_valid %= buf_size;
	if ( buf_valid == buf_index ) {
		buf_valid = SIZE_MAX;
	}
	return ;
}

/** GetFlit
 *  retrieve oldest flit in virtual channel
 *
 *  @return Oldest flit in virtual channel
 */
Flit* VirtualChannel::GetFlit ( ) const
{
	assert( buf_valid < buf_size ); // No valid flits
	return buf[buf_valid];
}

/** FlitsRemaining
 *  returns the number of flits stored in the virtual channel
 *
 *  @return Number of flits in the virtual channel
 */
size_t VirtualChannel::FlitsRemaining ( ) const
{
	if ( buf_valid >= buf_size )
		return 0;
	size_t tmp = buf_index;
	if ( tmp <= buf_valid )
		tmp += buf_size;
	return tmp - buf_valid;
}

/** GetPacket
 *  returns the current packet being routed
 *
 *  @return Packet currently in virtual channel
 */
Packet* VirtualChannel::GetPacket ( ) const
{
	return cur_packet;
}