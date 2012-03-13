#include "Global.h"


InputBuffer::InputBuffer ()
{
	obuf = NULL;
	buf_route = SIZE_MAX;
	routes = (Direction*) malloc(sizeof(Direction) * 4);
}

InputBuffer::InputBuffer ( size_t entries ) : Buffer(entries)
{
	obuf = NULL;
	buf_route = SIZE_MAX;
	routes = (Direction*) malloc(sizeof(Direction) * entries);
}

InputBuffer::~InputBuffer ()
{
	free(routes);
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

/** PopPacket
 *  overwrites Buffer::PopPacket for credit based flow control
 *
 */
void InputBuffer::PopPacket ( )
{
	// Send credit back if we are connected to somewhere
	if ( obuf ) {
		uint32_t* i = new uint32_t(1); //Send back 1 credit
		Event e = {CREDIT, i};
		Global_Queue.Add(e, obuf, Global_Time+1);
	}
	if ( buf_route == buf_valid )
		buf_route = SIZE_MAX;
	Buffer::PopPacket();
}

/** RoutePacket
 *  performs routing computation on the next packet to be routed
 *
 *  @args routerAddr XY address of router
 */
void InputBuffer::RoutePacket ( Address routerAddr )
{
	if ( Routed() < PacketsRemaining() ) {
		// Routing packet buf[buf_route]
		if ( buf_route >= buf_size )
			buf_route = buf_valid;
		else {
			buf_route++;
			buf_route %= buf_size;
		}

		assert( buf_route < buf_size ); // No valid packets

		// Route packet stored in buf[buf_route]
		if ( buf[buf_route]->GetX() < routerAddr.x)
			routes[buf_route] = WEST;
		else if ( buf[buf_route]->GetX() > routerAddr.x )
			routes[buf_route] = EAST;
		else if ( buf[buf_route]->GetY() < routerAddr.y )
			routes[buf_route] = SOUTH;
		else if ( buf[buf_route]->GetY() > routerAddr.y )
			routes[buf_route] = NORTH;
		else
			routes[buf_route] = HERE;
	}
}

/** GetRoute
 *  returns the direction that the next valid packet as been routed to
 *
 *  @return  Direction of output port
 */
Direction InputBuffer::GetRoute ( )
{
	assert( (buf_valid <= buf_size) && (buf_route <= buf_size) ); // No routed packets
	return routes[buf_valid];
}

/** Routed
 *  returns the number of packets in the buffer that have been routed
 *
 *  @return  Number of routed packets
 */
size_t InputBuffer::Routed ( )
{
	if ( buf_route >= buf_size )
		return 0;
	size_t tmp = buf_index;
	if (tmp <= buf_route)
		tmp += buf_size;
	return tmp - buf_route;
}

