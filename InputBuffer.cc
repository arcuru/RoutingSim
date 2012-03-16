#include "Global.h"


InputBuffer::InputBuffer ()
{
	obuf = NULL;
}

InputBuffer::InputBuffer ( size_t entries ) : Buffer(entries)
{
	obuf = NULL;
}

InputBuffer::~InputBuffer ()
{
}

/** setRC
 *  saved a pointer to the RouteComputation component
 *
 *  @arg rcomp Pointer to RouteComputation component
 */
void InputBuffer::setRC ( RouteComputation* rcomp )
{
	RC = rcomp;
}

/** WriteBack
 *  sets corresponding output buffer for flow control
 *
 *  @args write_back  Pointer to corresponding output buffer
 */
void InputBuffer::WriteBack ( OutputBuffer* write_back )
{
	obuf = write_back;
	return ;
}

/** PopFlit
 *  overwrites Buffer::PopFlit for credit based flow control
 *
 */
void InputBuffer::PopFlit ( )
{
	// Send credit back if we are connected to somewhere
	if ( obuf ) {
		uint32_t* i = new uint32_t(1); //Send back 1 credit
		Event e = {CREDIT, i, this};
		Global_Queue.Add(e, obuf, Global_Time+1);
	}
	Buffer::PopFlit();
}

/** ProcessEvent
 *  overwrites Buffer::ProcessEvent to set up route computation
 *
 *  @arg e Event to be processed
 */
void InputBuffer::ProcessEvent ( Event e )
{
	assert( DATA == e.t );
	if ( ((Flit*)e.d)->isHead() ) {
		// We've received a head flit
		assert( FlitsRemaining() == 0); // No other flits can be present
		assert( NULL != RC );

		Event q = { DATA, e.d, this };
		Global_Queue.Add( q, RC, Global_Time+1 );
	}
	Buffer::ProcessEvent( e );
}

