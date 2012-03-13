#include "Global.h"

Buffer::Buffer ()
{
	buf_index = 0;
	buf_valid = SIZE_MAX;
	buf_size = 4;
	buf = (Packet*) malloc(sizeof(Packet) * buf_size);
}

Buffer::Buffer ( size_t entries )
{
	buf_index = 0;
	buf_valid = SIZE_MAX;
	buf_size = entries;
	buf = (Packet*) malloc(sizeof(Packet) * buf_size);
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
uint32_t Buffer::Size ( )
{
	return buf_size;
}

/** ProcessPacket
 *  handles an incoming packet by adding it to the buffer
 *
 *  @p  Packet to add into buffer
 */
void Buffer::ProcessPacket ( Packet p )
{
	assert( buf_index != buf_valid ); // Buffer full
	assert( buf_index < buf_size ); // Buffer indexing invalid
	buf[buf_index] = p;
	if ( buf_valid >= buf_size )
		buf_valid = buf_index;
	buf_index++;
	buf_index %= buf_size;
	return ;
}

/** PopPacket
 *  delete the oldest packet in buffer
 *
 */
void Buffer::PopPacket ( )
{
	assert( buf_valid < buf_size ); // No valid packets
	buf_valid++;
	buf_valid %= buf_size;
	if ( buf_valid == buf_index ) {
		buf_valid = SIZE_MAX;
	}
	return ;
}

/** GetPacket
 *  retrieve oldest packet in buffer
 *
 *  @return Oldest packet in buffer
 */
Packet Buffer::GetPacket ( )
{
	assert( buf_valid < buf_size ); // No valid packets
	return buf[buf_valid];
}

/** PacketsRemaining
 *  returns the number of packets stored in the buffer
 *
 *  @return Number of packets in the buffer
 */
uint32_t Buffer::PacketsRemaining ( )
{
	if ( buf_valid >= buf_size )
		return 0;
	size_t tmp = buf_index;
	if ( tmp <= buf_valid )
		tmp += buf_size;
	return tmp - buf_valid;
}
