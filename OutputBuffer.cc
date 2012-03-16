#include "Global.h"

OutputBuffer::OutputBuffer ()
{
	t = NULL;
	available_space = 4;
	p = NULL;
	ib = NULL;
	flits_sent = 0;
}

OutputBuffer::OutputBuffer ( size_t entries ) : Buffer(entries)
{
	t = NULL;
	available_space = 4;
	p = NULL;
	ib = NULL;
	flits_sent = 0;
}

OutputBuffer::~OutputBuffer ()
{
}

/** ProcessBuffer
 *  processes the current buffer in the event that we need to do something
 *
 */
void OutputBuffer::ProcessBuffer (  )
{
	if (FlitsRemaining() == 0)
		return;
	if ( 0 == available_space )
		return;
	if ( GetFlit()->isHead() ) {
		// Have to wait for empty corresponding input buffer
		if ( 4 != available_space ) {
			return ;
		}
	}
	if ( NULL != t ) {
		Event e = {DATA, GetFlit(), this};
		Global_Queue.Add(e, t, Global_Time+1); 
		available_space--;
	}
	PopFlit();
	flits_sent++;
	if ( flits_sent == p->GetSize() ) {
		assert( FlitsRemaining() == 0 );
		p = NULL;
		ib = NULL;
		flits_sent = 0;
		//cout << "Cleared obuf" << endl;
	}
	return ;
}

/** Connect
 *  sets up the wire from this output buffer to an input buffer
 *
 *  @arg target The connected input buffer
 */
void OutputBuffer::Connect ( EventTarget* target )
{
	t = target;
	return ;
}

/** ProcessEvent
 *  overwrites Buffer::ProcessEvent for credit based flow control
 *
 *  @arg e  Event to process
 */
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

/** GetPrev
 *  returns a pointer to the InputBuffer that is currently sending to this
 *  OutputBuffer
 *
 */
InputBuffer* OutputBuffer::GetPrev ( ) const
{
	return ib;
}

/** GetPacket
 *  returns a pointer to the Packet that is currently in this buffer
 *
 */
Packet* OutputBuffer::GetP ( ) const
{
	return p;
}

