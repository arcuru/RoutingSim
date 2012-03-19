#include "Global.h"

RouteComputation::RouteComputation ()
{
	for (size_t i=0; i < 5; i++) {
		ovc_xy[i] = NULL;
		ovc_ad[i] = NULL;
		for (size_t j = 0; j < 5; j++) {
			ivc_xy[i][j] = NULL;
			ivc_ad[i][j] = NULL;
		}
	}
}

RouteComputation::RouteComputation ( Address a )
{
	addr = a;
	for (size_t i=0; i < 5; i++) {
		ovc_xy[i] = NULL;
		ovc_ad[i] = NULL;
		for (size_t j = 0; j < 5; j++) {
			ivc_xy[i][j] = NULL;
			ivc_ad[i][j] = NULL;
		}
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
	ovc_xy[edge] = ob->getVC( 0 );;
	ovc_ad[edge] = ob->getVC( 1 );;
}

/** Insert
 *  sets up the given VirtualChannel to allow for routing in the given direction
 *  and output channel
 *
 *  @arg vc Pointer to the VirtualChannel to add
 *  @arg d  Direction of possible route
 *  @arg c  Output channel, either XY or adaptive
 */
void RouteComputation::Insert ( VirtualChannel* vc, Direction d, size_t c )
{
	assert( INVALID != d );

	size_t i = 0;
	switch ( c ) {
		case 0:	
			while ( NULL != ivc_xy[d][i] )
				i++;
			assert( i < 5 );
			ivc_xy[d][i] = vc;
			break;

		case 1:	
			while ( NULL != ivc_ad[d][i] )
				i++;
			assert( i < 5 );
			ivc_ad[d][i] = vc;
			break;

		default:	
			break;
	}
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

	VirtualChannel* vc = (VirtualChannel*)e.o;

	// Route packet adaptively
	if ( addr.x > p->GetX() ) {
		if ( addr.x - (NInfo.width/2) > p->GetX() )
			Insert( vc, EAST, 1); 
		else
			Insert( vc, WEST, 1 ); 
	}
	else if ( addr.x < p->GetX() ) {
		if ( addr.x + (NInfo.width/2) < p->GetX() )
			Insert( vc, WEST, 1 );
		else
			Insert( vc, EAST, 1 );
	}
	if ( addr.y > p->GetY() ) {
		if ( addr.y - (NInfo.height/2) > p->GetY() )
			Insert( vc, NORTH, 1 ); 
		else
			Insert( vc, SOUTH, 1 ); 
	}
	else if ( addr.y < p->GetY() ) {
		if ( addr.y + (NInfo.height/2) < p->GetY() )
			Insert( vc, SOUTH, 1 );
		else
			Insert( vc, NORTH, 1 );
	}
	
	// Route packet for XY escape network
	if ( p->GetX() < addr.x)
		Insert( vc, WEST, 0 ); 
	else if ( p->GetX() > addr.x )
		Insert( vc, EAST, 0 ); 
	else if ( p->GetY() < addr.y )
		Insert( vc, SOUTH, 0 ); 
	else if ( p->GetY() > addr.y )
		Insert( vc, NORTH, 0 ); 
	else
		Insert( vc, HERE, 0 ); 
}

/** getNext
 *  returns the next valid VirtualChannel that has requested to be 
 *  routed to the input direction and channel
 *
 *  @arg dir Direction of routing
 *  @arg c   Channel requested, either XY or adaptiver
 *  @return Pointer to valid InputBuffer
 */
VirtualChannel* RouteComputation::getNext ( size_t dir, size_t c ) const
{
	if ( 0 == c )
		return ivc_xy[dir][0];
	return ivc_ad[dir][0];
}

/** Remove
 *  invalidates all the saved routes from the given VirtualChannel
 *
 *  @arg vc Pointer to VirtualChannel that is no longer to be routed
 */
void RouteComputation::Remove ( VirtualChannel* vc )
{
	_Remove( vc, ivc_xy );
	_Remove( vc, ivc_ad );
}

/** _Remove
 *  invalidates all the saved routes from the given VirtualChannel
 *  only in the input output array
 *
 *  @arg vc Pointer to VirtualChannel that is no longer to be routed
 *  @arg arr Poniter to array of VC's that it should be removed from
 */
void RouteComputation::_Remove ( VirtualChannel* vc, VirtualChannel* arr[5][5] )
{
	assert( NULL != vc );
	
	for (size_t i = 0; i < 5; i++) {
		// Search for vc
		size_t j;
		for (j = 0; j < 5; j++) {
			if ( NULL == arr[i][j] )
				break;
			if ( vc == arr[i][j] ) {
				// Move everything above it down
				arr[i][j] = NULL;
				for (size_t k = j+1; k < 5; k++) {
					arr[i][k-1] = arr[i][k];
				}
				arr[i][4] = NULL;
				break;
			}
		}
	}
}

