#include "Global.h"

OutputChannel::OutputChannel ( )
{
	available_space = 4;
	flits_sent = 0;
	wb = NULL;
	target = NULL;
}

OutputChannel::OutputChannel ( size_t entries ) : VirtualChannel( entries )
{
	available_space = 4;
	flits_sent = 0;
	wb = NULL;
	target = NULL;
}

OutputChannel::~OutputChannel ()
{
}

/** ProcessEvent
 *  handles an incoming flit by adding it to the virtual channel
 *
 *  @arg e  Incoming event
 */
void OutputChannel::ProcessEvent ( Event e )
{
	assert( DATA == e.t || CREDIT == e.t );
	switch ( e.t ) {
		case CREDIT:
			available_space += *(size_t*)e.d;
			delete (size_t*)e.d;
			break;

		case DATA:
			if ( (VirtualChannel*)e.o != wb ) {
				assert( isReady() );
				assert( ((Flit*)e.d)->isHead() );
				assert( FlitsRemaining() == 0 );
				wb = (InputChannel*)e.o;
			}
			if ( ((Flit*)e.d)->isHead() ) {
				assert( FlitsRemaining() == 0 );
				flits_sent = 0;
			}

			InsertFlit( (Flit*)e.d );
			break;
	}
}

/** sendFlit
 *  send the head flit to the target VC if necessary
 *
 *  @return Boolean indicating whether send was successful
 */
bool OutputChannel::sendFlit ( )
{
	if ( FlitsRemaining() == 0 || 0 == available_space )
		return false;
	assert( NULL != target );
	if ( GetFlit()->isHead() && target->Size() != available_space )
		return false;
	Event e = {DATA, GetFlit(), this};
	Global_Queue.Add(e, target, Global_Time+1); 
	available_space--;
	PopFlit();
	flits_sent++;
	assert( flits_sent <= cur_packet->GetSize() );
	return true;
}

/** SetTarget
 *  sets the route for the current packet
 *
 *  @arg t VC target that we are sending data to
 */
void OutputChannel::setTarget ( InputChannel* t )
{
	target = t;
}

/** getTarget
 *  gets the route for the current packet
 *
 *  @return Pointer to the target InputChannel
 */
InputChannel* OutputChannel::getTarget (  ) const
{
	return target;
}

/** setWB
 *  saves the VC we are receiving data from
 *
 *  @arg writeback Pointer to VC we are receiving from
 */
void OutputChannel::setWB ( InputChannel* writeback )
{
	wb = writeback;
}

/** getWB
 *  returns a pointer to the VC we are receiving from
 *
 *  @return Pointer to VC
 */
InputChannel* OutputChannel::getWB ( ) const
{
	return wb;
}

/** isReady
 *  returns a boolean indicating whether this buffer is ready to receive
 *  a new Packet
 *
 *  @return Whether or not we have sent all flits and those have cleared from target
 */
bool OutputChannel::isReady ( ) const
{
	// First time checking
	if ( NULL == cur_packet )
		return true;
	// Ejection queue
	if ( NULL == target && FlitsRemaining() < buf_size )
		return true;
	if ( cur_packet->GetSize() == flits_sent && available_space == target->Size() ) {
		assert( FlitsRemaining() == 0 );
		return true;
	}
	return false;
}

/** isWorking
 *  returns a boolean indicating whether this buffer is in the process of
 *  moving a packet
 *
 *  @return Whether or not we are processing a packet
 */
bool OutputChannel::isWorking ( ) const
{
	if ( FlitsRemaining() > 0 )
		return true;
	else if ( NULL == target )
		return false;
	else if ( available_space != target->Size() )
		return true;
	else if ( NULL != cur_packet && flits_sent < cur_packet->GetSize() )
		return true;
	else
		return false;
}

