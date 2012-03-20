#include "Global.h"

RouteComputation::RouteComputation ()
{
	for (size_t i=0; i < 5; i++) {
		for (size_t j = 0; j < 9; j++) {
			ivc_xy[i][j] = NULL;
			ivc_ad[i][j] = NULL;
		}
	}
}

RouteComputation::RouteComputation ( Address a )
{
	addr = a;
	for (size_t i=0; i < 5; i++) {
		for (size_t j = 0; j < 9; j++) {
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

/** Insert
 *  sets up the given VirtualChannel to allow for routing in the given direction
 *  and output channel
 *
 *  @arg vc Pointer to the VirtualChannel to add
 *  @arg d  Direction of possible route
 *  @arg c  Output channel, either XY or adaptive
 */
void RouteComputation::Insert ( InputChannel* vc, Direction d, size_t c )
{
	assert( INVALID != d );

	size_t i = 0;
	switch ( c ) {
		case 0:	
			while ( NULL != ivc_xy[d][i] )
				i++;
			assert( i < 9 );
			ivc_xy[d][i] = vc;
			break;

		case 1:	
			while ( NULL != ivc_ad[d][i] )
				i++;
			assert( i < 9 );
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

	InputChannel* vc = (InputChannel*)e.o;

	//cout << "Addr: (" << addr.x << ", " << addr.y << ")" << endl;

	// Route packet adaptively
	bool xfin = false;
	bool yfin = false;
	size_t tmp = p->GetX();
	if ( tmp < addr.x ) {
		tmp += NInfo.width/2;
		if ( tmp < addr.x ) {
			Insert( vc, EAST, 1 );
			Insert( vc, EAST, 0 );
		}
		else {
			Insert( vc, WEST, 1 );
			Insert( vc, WEST, 0 );
		}
	}
	else if ( tmp > addr.x ) {
		tmp -= NInfo.width/2;
		if ( tmp > addr.x ) {
			Insert( vc, WEST, 1 );
			Insert( vc, WEST, 0 );
		}
		else {
			Insert( vc, EAST, 1 );
			Insert( vc, EAST, 0 );
		}
	}
	else
		xfin = true;

	tmp = p->GetY();
	if ( tmp < addr.y ) {
		tmp += NInfo.height/2;
		if ( tmp < addr.y ) {
			Insert( vc, NORTH, 1 );
			if ( xfin )
				Insert( vc, NORTH, 0 );
		}
		else {
			Insert( vc, SOUTH, 1 );
			if ( xfin )
				Insert( vc, SOUTH, 0 );
		}
	}
	else if ( tmp > addr.y ) {
		tmp -= NInfo.height/2;
		if ( tmp > addr.y ) {
			Insert( vc, SOUTH, 1 );
			if ( xfin )
				Insert( vc, SOUTH, 0 );
		}
		else {
			Insert( vc, NORTH, 1 );
			if ( xfin )
				Insert( vc, NORTH, 0 );
		}
	}
	else
		yfin = true;

	if ( xfin && yfin )
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
InputChannel* RouteComputation::getNext ( size_t dir, size_t c ) const
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
void RouteComputation::Remove ( InputChannel* vc )
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
void RouteComputation::_Remove ( InputChannel* vc, InputChannel* arr[5][9] )
{
	assert( NULL != vc );
	
	for (size_t i = 0; i < 5; i++) {
		// Search for vc
		size_t j;
		for (j = 0; j < 9; j++) {
			if ( NULL == arr[i][j] )
				break;
			if ( vc == arr[i][j] ) {
				// Move everything above it down
				arr[i][j] = NULL;
				for (size_t k = j+1; k < 9; k++) {
					arr[i][k-1] = arr[i][k];
				}
				arr[i][8] = NULL;
				break;
			}
		}
	}
}

