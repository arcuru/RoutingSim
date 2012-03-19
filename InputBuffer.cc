#include "Global.h"


InputBuffer::InputBuffer ()
{
	channel_count = 2;
	vc = new VirtualChannel[channel_count];
	for (size_t i = 0; i < channel_count; i++) {
		vc[i].setSize( 4 );
		vc[i].setType( VC_IN );
		vc[i].setID( i );
	}
}

InputBuffer::InputBuffer ( size_t entries )
{
	channel_count = 2;
	vc = new VirtualChannel[channel_count];
	for (size_t i = 0; i < channel_count; i++) {
		vc[i].setSize( entries );
		vc[i].setType( VC_IN );
		vc[i].setID( i );
	}
}

InputBuffer::~InputBuffer ()
{
	delete [] vc;
}

/** setRC
 *  saved a pointer to the RouteComputation component
 *
 *  @arg rcomp Pointer to RouteComputation component
 */
void InputBuffer::setRC ( RouteComputation* rcomp )
{
	for (size_t i = 0; i < channel_count; i++) {
		vc[i].setRC( rcomp );
	}
}

/** WriteBack
 *  sets corresponding output buffer for flow control
 *
 *  @args write_back  Pointer to corresponding output buffer
 */
void InputBuffer::WriteBack ( OutputBuffer* write_back )
{
	for (size_t i = 0; i < channel_count; i++) {
		vc[i].setWB( write_back->getVC( i ) );
	}
	return ;
}

/** PopFlit
 *  overwrites Buffer::PopFlit for credit based flow control
 *
 */
/*
void InputBuffer::PopFlit ( )
{
}
*/

/** ProcessEvent
 *  overwrites Buffer::ProcessEvent to set up route computation
 *
 *  @arg e Event to be processed
 */
/*
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
*/

/** getVC
 *  retrieve a pointer to the requisite virtual channel
 *
 *  @arg channel  VC identifer to return
 *  @return  Pointer to virtual channel specified in input
 */
VirtualChannel* InputBuffer::getVC ( size_t channel ) const
{
	assert( channel < 2 );
	return &vc[channel];
}

