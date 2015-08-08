#include "Global.h"

InputChannel::InputChannel ( )
{
	RC = nullptr;
	target = nullptr;
	wb = nullptr;
	addr.x = 10;
	addr.y = 10;
}

InputChannel::InputChannel ( size_t entries ) : VirtualChannel( (uint8_t)entries )
{
	RC = nullptr;
	target = nullptr;
	wb = nullptr;
	addr.x = 10;
	addr.y = 10;
}

InputChannel::~InputChannel ()
{
}

/** setAddr
 *  saved the address of the router this belongs to
 *
 *  @arg newAddress Address of current router
 */
void InputChannel::setAddr ( Address newAddress )
{
	addr = newAddress;
}

/** setRC
 *  saved a pointer to the RouteComputation component
 *
 *  @arg rcomp Pointer to RouteComputation component
 */
void InputChannel::setRC ( RouteComputation* rcomp )
{
	RC = rcomp;
}

/** ProcessEvent
 *  handles an incoming flit by adding it to the virtual channel
 *
 *  @arg e  Incoming event
 */
void InputChannel::ProcessEvent ( Event e )
{
	assert( DATA == e.t );
	assert( (OutputChannel*)e.o == wb );

	InsertFlit( (Flit*)e.d );
	if ( ((Flit*)e.d)->isHead() ) {
		// We've received a head flit
		assert( FlitsRemaining() == 1); // No other flits can be present
#ifndef NDEBUG
		GetPacket()->AddRouter( addr );
#endif

		schedRC();
	}
}

/** sendFlit
 *  send the head flit to the target VC if necessary
 *
 */
void InputChannel::sendFlit ( )
{
	assert( nullptr != target );
	if (FlitsRemaining() == 0)
		return;
	Event e = {DATA, GetFlit(), this};
	Global_Queue.Add(e, target, Global_Time+1); 
	PopFlit();

	// Now send back credit
	if ( nullptr != wb ) {
		size_t* i = new size_t(1); //Send back 1 credit
		Event d = {CREDIT, i, this};
		Global_Queue.Add(d, wb, Global_Time+1); 
	}
	return ;
}

/** SetTarget
 *  sets the route for the current packet
 *
 *  @arg t VC target that we are sending data to
 */
void InputChannel::setTarget ( OutputChannel* t )
{
	target = t;
}

/** getTarget
 *  gets the route for the current packet
 *
 *  @return Pointer to the target InputChannel
 */
OutputChannel* InputChannel::getTarget (  ) const
{
	return target;
}

/** setWB
 *  saves the VC we are receiving data from
 *
 *  @arg writeback OutputChannel to receive data from
 */
void InputChannel::setWB ( OutputChannel* writeback )
{
	wb = writeback;
}

/** schedRC
 *  schedules route computation for the current head flit
 *
 */
void InputChannel::schedRC ( )
{
	assert( GetFlit()->isHead() );
	assert( nullptr != RC );

	// Schedule route computation for next cycle
	Event q = { DATA, GetFlit(), this };
	Global_Queue.Add( q, RC, Global_Time+1 );
}

