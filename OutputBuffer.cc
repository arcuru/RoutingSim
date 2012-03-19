#include "Global.h"

OutputBuffer::OutputBuffer ()
{
	channel_count = 2;
	vc = new VirtualChannel[channel_count];
	for (size_t i = 0; i < channel_count; i++) {
		vc[i].setSize( 4 );
		vc[i].setType( VC_OUT );
		vc[i].setID( i );
	}
}

OutputBuffer::OutputBuffer ( size_t entries )
{
	channel_count = 2;
	vc = new VirtualChannel[channel_count];
	for (size_t i = 0; i < channel_count; i++) {
		vc[i].setSize( entries );
		vc[i].setType( VC_OUT );
		vc[i].setID( i );
	}
}

OutputBuffer::~OutputBuffer ()
{
	delete [] vc;
}

/** ProcessBuffer
 *  processes the current buffer in the event that we need to do something
 *
 */
void OutputBuffer::ProcessBuffer (  )
{
	for (size_t i = 0; i < channel_count; i++) {
		vc[i].sendFlit();
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
		vc[i].SetTarget( target->getVC( i ) );
	}
	return ;
}

/** ProcessEvent
 *  overwrites Buffer::ProcessEvent for credit based flow control
 *
 *  @arg e  Event to process
 */
/*
void OutputBuffer::ProcessEvent ( Event e )
{
	if ( CREDIT == e.t ) {
		available_space += *(uint32_t*)e.d;
		delete (uint32_t*)e.d;
	}
	else {
		if ( NULL == p ) {
			// No packet saved yet, this must be a head flit
			Flit* f = (Flit*)e.d;
			assert( f->isHead() == true );
			assert( 0 == flits_sent );
			p = f->getPacket();
			ib = (InputBuffer*)e.o;
			assert( NULL != ib );
			//cout << p->GetOriginX() << ", " << p->GetOriginY() << ", " << p->GetCreated() << endl;
		}
		assert( (InputBuffer*)e.o == ib ); // Make sure packet is from correct ibuf
		Buffer::InsertFlit( (Flit*)e.d );
	}
}
*/

/** GetPrev
 *  returns a pointer to the InputBuffer that is currently sending to this
 *  OutputBuffer
 *
 */
/*
InputBuffer* OutputBuffer::GetPrev ( ) const
{
	return ib;
}
*/

/** GetPacket
 *  returns a pointer to the Packet that is currently in this buffer
 *
 */
/*
Packet* OutputBuffer::GetP ( ) const
{
	return p;
}
*/

/** getVC
 *  retrieve a pointer to the requisite virtual channel
 *
 *  @arg channel  VC identifer to return
 *  @return  Pointer to virtual channel specified in input
 */
VirtualChannel* OutputBuffer::getVC ( size_t channel ) const
{
	assert( channel < 2 );
	return &vc[channel];
}

