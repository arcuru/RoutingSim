#include "Global.h"

InputBuffer::InputBuffer ()
{
	channel_count = 3;
	ic = new InputChannel[channel_count];
	for (size_t i = 0; i < channel_count; i++) {
		ic[i].setSize( 4 );
	}
}

InputBuffer::InputBuffer ( size_t entries )
{
	channel_count = 3;
	ic = new InputChannel[channel_count];
	for (size_t i = 0; i < channel_count; i++) {
		ic[i].setSize( entries );
	}
}

InputBuffer::~InputBuffer ()
{
	delete [] ic;
}

/** setAddr
 *  saved the address of this router to each of the InputChannels
 *
 *  @arg newAddr Address of current router
 */
void InputBuffer::setAddr ( Address newAddr )
{
	for (size_t i = 0; i < channel_count; i++) {
		ic[i].setAddr( newAddr );
	}
}

/** setRC
 *  saved a pointer to the RouteComputation component
 *
 *  @arg rcomp Pointer to RouteComputation component
 */
void InputBuffer::setRC ( RouteComputation* rcomp )
{
	for (size_t i = 0; i < channel_count; i++) {
		ic[i].setRC( rcomp );
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
		ic[i].setWB( write_back->getOC( i ) );
	}
	return ;
}

/** getIC
 *  retrieve a pointer to the requisite virtual channel
 *
 *  @arg channel  IC identifer to return
 *  @return  Pointer to virtual channel specified in input
 */
InputChannel* InputBuffer::getIC ( size_t channel ) const
{
	assert( channel < 3 );
	return &ic[channel];
}

