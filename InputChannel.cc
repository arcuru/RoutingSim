#include "Global.h"

InputChannel::InputChannel ( )
{
	RC = NULL;
	target = NULL;
	wb = NULL;
}

InputChannel::InputChannel ( size_t entries ) : VirtualChannel( entries )
{
	RC = NULL;
	target = NULL;
	wb = NULL;
}

InputChannel::~InputChannel ()
{
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

		schedRC();
	}
}

/** sendFlit
 *  send the head flit to the target VC if necessary
 *
 */
void InputChannel::sendFlit ( )
{
	assert( NULL != target );
	if (FlitsRemaining() == 0)
		return;
	Event e = {DATA, GetFlit(), this};
	Global_Queue.Add(e, target, Global_Time+1); 
	PopFlit();

	// Now send back credit
	if ( NULL != wb ) {
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
	assert( NULL != RC );

	// Schedule route computation for next cycle
	Event q = { DATA, GetFlit(), this };
	Global_Queue.Add( q, RC, Global_Time+1 );
}

