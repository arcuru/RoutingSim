#include "Global.h"

VirtualChannel::VirtualChannel ( )
{
	buf_index = 0;
	buf_valid = SIZE_MAX;
	buf_size = 4;
	buf = (Flit**) malloc(sizeof(Flit*) * buf_size);
	available_space = 4;
	flits_sent = 0;
	wb = NULL;
}

VirtualChannel::VirtualChannel ( size_t entries )
{
	buf_index = 0;
	buf_valid = SIZE_MAX;
	buf_size = entries;
	buf = (Flit**) malloc(sizeof(Flit*) * buf_size);
	available_space = 4;
	flits_sent = 0;
	wb = NULL;
}

VirtualChannel::~VirtualChannel ()
{
	free(buf);
}

/** setID
 *  sets the channel ID of the current vc
 *
 *  @arg id New id of this VC
 */
void VirtualChannel::setID ( size_t id )
{
	channel_id = id;
}

/** getID
 *  gets the channel ID of the current vc
 *
 *  @return Channel ID of this VC
 */
size_t VirtualChannel::getID ( ) const
{
	return channel_id;
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

/** setRC
 *  saved a pointer to the RouteComputation component
 *
 *  @arg rcomp Pointer to RouteComputation component
 */
void VirtualChannel::setRC ( RouteComputation* rcomp )
{
	RC = rcomp;
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

/** ProcessEvent
 *  handles an incoming flit by adding it to the virtual channel
 *
 *  @arg e  Incoming event
 */
void VirtualChannel::ProcessEvent ( Event e )
{
	assert( DATA == e.t || CREDIT == e.t );
	switch ( e.t ) {
		case CREDIT:	
			available_space += *(uint32_t*)e.d;
			delete (uint32_t*)e.d;
			assert( NULL != current );
			// Wait for whole packet to be sent and target VC to clear
			if ( current->GetSize() == flits_sent && 4 == available_space ) {
				flits_sent = 0;
				current = NULL;
				if ( VC_OUT == type )
					wb = NULL;
			}
			break;

		case DATA:	
			if ( NULL == wb )
				wb = (VirtualChannel*)e.o;

			switch ( type ) {
				case VC_IN:	
					if ( ((Flit*)e.d)->isHead() ) {
						// We've received a head flit
						assert( FlitsRemaining() == 0); // No other flits can be present
						assert( NULL != RC );

						Event q = { DATA, e.d, this };
						Global_Queue.Add( q, RC, Global_Time+1 );
					}
					break;

				case VC_OUT:	
					break;

				default:	
					break;
			}
			InsertFlit( (Flit*)e.d );
			break;
	}
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
	// Below not valid for injection queue
//	assert( p->isHead() == false || FlitsRemaining() == 0 );

	if ( FlitsRemaining() == 0 && p->isHead() ) {
		assert( NULL == current );
		current = p->getPacket();
		if ( VC_IN == type )
			target = NULL;
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
	buf_valid++;
	buf_valid %= buf_size;
	if ( buf_valid == buf_index ) {
		buf_valid = SIZE_MAX;
	}

	if ( VC_IN == type && NULL != wb ) {
		// Send credit back to corresponding output VC
		uint32_t* i = new uint32_t(1); //Send back 1 credit
		Event e = {CREDIT, i, this};
		Global_Queue.Add(e, wb, Global_Time+1);
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

/** sendFlit
 *  send the head flit to the target VC if necessary
 *
 */
void VirtualChannel::sendFlit ( )
{
	if (FlitsRemaining() == 0)
		return;
	if ( 0 == available_space )
		return;
	if ( GetFlit()->isHead() )
		assert( available_space == 4 );
	assert( NULL != target );
	Event e = {DATA, GetFlit(), this};
	Global_Queue.Add(e, target, Global_Time+1); 
	available_space--;
	PopFlit();
	flits_sent++;
	return ;
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

/** SetTarget
 *  sets the route for the current packet
 *
 *  @arg t VC target that we are sending data to
 */
void VirtualChannel::SetTarget ( VirtualChannel* t )
{
	target = t;
}

/** setType
 *  saves the type of this VC
 *
 *  @arg t Type of virtual channel
 */
void VirtualChannel::setType ( VC_t t )
{
	type = t;
}

/** setWB
 *  saves the VC we are receiving data from
 *
 *  @arg t Type of virtual channel
 */
void VirtualChannel::setWB ( VirtualChannel* writeback )
{
	wb = writeback;
}

/** GetTarget
 *  returns the current routing direction
 *
 *  @return Direction of travel for current packet
 */
VirtualChannel* VirtualChannel::GetTarget ( ) const
{
	return target;
}

/** GetPacket
 *  returns the current packet being routed
 *
 *  @return Packet currently in virtual channel
 */
Packet* VirtualChannel::GetPacket ( ) const
{
	return current;
}

/** getWB
 *  returns a pointer to the VC we are receiving from
 *
 *  @return Pointer to VC
 */
VirtualChannel* VirtualChannel::getWB ( ) const
{
	return wb;
}

/** getSpace
 *  returns the amount of space available at the corresponding VC
 *
 *  @return The free space in connected VC
 */
size_t VirtualChannel::getSpace ( ) const
{
	return available_space;
}

