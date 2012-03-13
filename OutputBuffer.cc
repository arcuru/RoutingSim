#include "Global.h"

OutputBuffer::OutputBuffer ()
{
	t = NULL;
	available_space = 4;
}

OutputBuffer::OutputBuffer ( size_t entries ) : Buffer(entries)
{
	t = NULL;
	available_space = 4;
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
	if (PacketsRemaining() == 0)
		return;
	if ( 0 == available_space )
		return;
	Event e = {DATA, GetPacket()};
	Global_Queue.Add(e, t, Global_Time+1); 
	available_space--;
	PopPacket();
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
	else
		Buffer::InsertPacket((Packet*)e.d);
}
