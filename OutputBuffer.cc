#include "Global.h"

OutputBuffer::OutputBuffer ()
{
	channel_count = 2;
	oc = new OutputChannel[channel_count];
	for (size_t i = 0; i < channel_count; i++) {
		oc[i].setSize( 1 );
	}
	last_sent = 0;
}

OutputBuffer::OutputBuffer ( size_t entries )
{
	channel_count = 2;
	oc = new OutputChannel[channel_count];
	for (size_t i = 0; i < channel_count; i++) {
		oc[i].setSize( entries );
	}
	last_sent = 0;
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
	size_t j;
	for (size_t i = last_sent+1; i < channel_count+last_sent+1; i++) {
		j = i % channel_count;
		if ( oc[j].sendFlit() ) {
			link_util++;
			break;
		}
	}
	last_sent = j;
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

