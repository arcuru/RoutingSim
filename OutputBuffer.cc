#include "Global.h"

OutputBuffer::OutputBuffer ()
{
	channel_count = 2;
	oc = new OutputChannel[channel_count];
	for (size_t i = 0; i < channel_count; i++) {
		oc[i].setSize( 4 );
	}
}

OutputBuffer::OutputBuffer ( size_t entries )
{
	channel_count = 2;
	oc = new OutputChannel[channel_count];
	for (size_t i = 0; i < channel_count; i++) {
		oc[i].setSize( entries );
	}
}

OutputBuffer::~OutputBuffer ()
{
	delete [] oc;
}

/** ProcessBuffer
 *  processes the current buffer in the event that we need to do something
 *
 */
void OutputBuffer::ProcessBuffer (  )
{
	for (size_t i = 0; i < channel_count; i++) {
		if (oc[i].FlitsRemaining() != 0)
			oc[i].sendFlit();
	}
	return ;
}

/** Connect
 *  sets up the wire from this output buffer to an input buffer
 *
 *  @arg target The connected input buffer
 */
void OutputBuffer::Connect ( InputBuffer* target )
{
	for (size_t i = 0; i < channel_count; i++) {
		oc[i].setTarget( target->getIC( i ) );
	}
	return ;
}

/** getOC
 *  retrieve a pointer to the requisite virtual channel
 *
 *  @arg channel  VC identifer to return
 *  @return  Pointer to virtual channel specified in input
 */
OutputChannel* OutputBuffer::getOC ( size_t channel ) const
{
	assert( channel < 2 );
	return &oc[channel];
}

