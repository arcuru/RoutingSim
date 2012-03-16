#include "Global.h"

RouteComputation::RouteComputation ()
{
	for (size_t i=0; i < 5; i++) {
		obuf[i] = NULL;
		for (size_t j = 0; j < 5; j++)
			ibuf[i][j] = NULL;
	}
}

RouteComputation::RouteComputation ( Address a )
{
	addr = a;
	for (size_t i=0; i < 5; i++) {
		obuf[i] = NULL;
		for (size_t j = 0; j < 5; j++)
			ibuf[i][j] = NULL;
	}
}

RouteComputation::~RouteComputation ()
{
}

/** setAddr
 *  saves the address of this router
 *
 *  @arg a Address to be saved
 */
void RouteComputation::setAddr ( Address a )
{
	addr = a;
}

/** setObuf
 *  saves an OutputBuffer and corresponding direction for route computation
 *
 *  @arg edge Direction of the input OutputBuffer
 *  @arg ob   Pointer to OutputBuffer
 */
void RouteComputation::setObuf ( Direction edge, OutputBuffer* ob )
{
	assert( NULL != ob );
	assert( edge < 5 );
	obuf[edge] = ob;
}

/** Insert
 *  sets up the given InputBuffer to allow for routing in the given direction
 *
 *  @arg ib Pointer to the InputBuffer
 *  @arg d  Direction of possible route
 */
void RouteComputation::Insert ( InputBuffer* ib, Direction d )
{
	assert( INVALID != d );
	size_t i = 0;
	while ( NULL != ibuf[d][i] )
		i++;
	assert( i < 5 );
	ibuf[d][i] = ib;
}

/** ProcessEvent
 *  routes a packet and saves the results for future retrieval
 *
 *  @arg e Event containing valid data for routing
 */
void RouteComputation::ProcessEvent ( Event e )
{
	assert( DATA == e.t );
	assert( NULL != e.d );
	assert( NULL != e.o );
	assert( ((Flit*)e.d)->isHead() );

	// Get pointer to packet corresponding to flit in buffer
	Packet* p = ((Flit*)e.d)->getPacket();

	InputBuffer* ib = (InputBuffer*)e.o;

	// Route packet
	if ( p->GetX() < addr.x)
		Insert( ib, WEST ); 
	else if ( p->GetX() > addr.x )
		Insert( ib, EAST ); 
	else if ( p->GetY() < addr.y )
		Insert( ib, SOUTH ); 
	else if ( p->GetY() > addr.y )
		Insert( ib, NORTH ); 
	else
		Insert( ib, HERE ); 
}

/** getnext
 *  returns the next valid InputBuffer that needs to be routed to the input
 *  OutputBuffer
 *
 *  @arg ob Target OutputBuffer
 *  @return Pointer to valid InputBuffer
 */
InputBuffer* RouteComputation::getNext ( size_t dir ) const
{
	return ibuf[dir][0];
}

/** Remove
 *  invalidates all the saved routes from the given InputBuffer
 *
 *  @arg ib Pointer to InputBuffer that is no longer to be routed
 */
void RouteComputation::Remove ( InputBuffer* ib )
{
	assert( NULL != ib );
	
	for (size_t i = 0; i < 5; i++) {
		// Search for ibuf
		size_t j;
		for (j = 0; j < 5; j++) {
			if ( NULL == ibuf[i][j] )
				break;
			if ( ib == ibuf[i][j] ) {
				// Move everything above it down
				ibuf[i][j] = NULL;
				for (size_t k = j+1; k < 5; k++) {
					ibuf[i][k-1] = ibuf[i][k];
				}
				ibuf[i][4] = NULL;
				break;
			}
		}
	}
}

