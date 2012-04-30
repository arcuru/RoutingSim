#include "Global.h"

RouteComputation::RouteComputation ()
{
	for (size_t i=0; i < 5; i++) {
		for (size_t j = 0; j < 13; j++) {
			ivc_xy[i][j] = NULL;
			ivc_xya[i][j] = NULL;
			ivc_ad[i][j] = NULL;
		}
	}
}

RouteComputation::RouteComputation ( Address a )
{
	addr = a;
	for (size_t i=0; i < 5; i++) {
		for (size_t j = 0; j < 13; j++) {
			ivc_xy[i][j] = NULL;
			ivc_xya[i][j] = NULL;
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
			assert( i < 13 );
			ivc_xy[d][i] = vc;
			break;

		case 1:	
			while ( NULL != ivc_xya[d][i] )
				i++;
			assert( i < 13 );
			ivc_xya[d][i] = vc;
			break;

		case 2:	
			while ( NULL != ivc_ad[d][i] )
				i++;
			assert( i < 13 );
			ivc_ad[d][i] = vc;
			break;

		default:	
			break;
	}
}

/** RouteXYEscape
 *  Sets up the correct routing for XY escape channel in a torus.
 *
 *  @arg vc Input virtual channel to route
 *  @arg p  Packet to be routed
 *  @arg a  Escape channel A routing table ID
 *  @arg b  Escape channel B routing table ID
 */
void RouteComputation::RouteXYEscape ( InputChannel* vc, Packet* p, size_t a, size_t b )
{
	// Route packet using XY in torus
	int tmp = p->GetX();
	if ( tmp < addr.x ) {
		tmp += NInfo.width/2;
		if ( tmp <= addr.x ) {
			if ( (p->GetOriginX() > addr.x) || (addr.x == NInfo.width - 1) )
				Insert( vc, EAST, b );
			else
				Insert( vc, EAST, a );
		}
		else {
			if ( (p->GetOriginX() < addr.x) || (addr.x == 0) )
				Insert( vc, WEST, b );
			else
				Insert( vc, WEST, a );
		}
	}
	else if ( tmp > addr.x ) {
		tmp -= NInfo.width/2;
		if ( tmp <= addr.x ) {
			if ( (p->GetOriginX() > addr.x) || (addr.x == NInfo.width - 1) )
				Insert( vc, EAST, b );
			else
				Insert( vc, EAST, a );
		}
		else {
			if ( (p->GetOriginX() < addr.x) || (addr.x == 0) )
				Insert( vc, WEST, b );
			else
				Insert( vc, WEST, a );
		}
	}
	else {
		tmp = p->GetY();
		if ( tmp < addr.y ) {
			tmp += NInfo.height/2;
			if ( tmp <= addr.y ) {
				if ( (p->GetOriginY() > addr.y) || (addr.y == NInfo.width - 1) )
					Insert( vc, NORTH, b );
				else
					Insert( vc, NORTH, a );
			}
			else {
				if ( (p->GetOriginY() < addr.y) || (addr.y == 0) )
					Insert( vc, SOUTH, b );
				else
					Insert( vc, SOUTH, a );
			}
		}
		else if ( tmp > addr.y ) {
			tmp -= NInfo.height/2;
			if ( tmp <= addr.y ) {
				if ( (p->GetOriginY() > addr.y) || (addr.y == NInfo.width - 1) )
					Insert( vc, NORTH, b );
				else
					Insert( vc, NORTH, a );
			}
			else {
				if ( (p->GetOriginY() < addr.y) || (addr.y == 0) )
					Insert( vc, SOUTH, b );
				else
					Insert( vc, SOUTH, a );
			}
		}
	}
}

/** RouteXY
 *  Sets up the correct routing for XY routing in a torus. Don't worry about
 *  deadlock
 *
 *  @arg vc Input virtual channel to route
 *  @arg p  Packet to be routed
 *  @arg c  XY routing table ID
 */
void RouteComputation::RouteXY ( InputChannel* vc, Packet* p, size_t c )
{
	// Route packet using XY in torus
	int tmp = p->GetX();
	if ( tmp < addr.x ) {
		tmp += NInfo.width/2;
		if ( tmp <= addr.x )
			Insert( vc, EAST, c );
		else
			Insert( vc, WEST, c );
	}
	else if ( tmp > addr.x ) {
		tmp -= NInfo.width/2;
		if ( tmp <= addr.x )
			Insert( vc, EAST, c );
		else
			Insert( vc, WEST, c );
	}
	else {
		tmp = p->GetY();
		if ( tmp < addr.y ) {
			tmp += NInfo.height/2;
			if ( tmp <= addr.y )
				Insert( vc, NORTH, c );
			else
				Insert( vc, SOUTH, c );
		}
		else if ( tmp > addr.y ) {
			tmp -= NInfo.height/2;
			if ( tmp <= addr.y )
				Insert( vc, NORTH, c );
			else
				Insert( vc, SOUTH, c );
		}
	}
}

/** RouteAdaptive
 *  Sets up the correct routing for adaptive routing in a torus
 *
 *  @arg vc Input virtual channel to route
 *  @arg p  Packet to be routed
 *  @arg c  Adaptive routing table ID
 */
void RouteComputation::RouteAdaptive ( InputChannel* vc, Packet* p, size_t c )
{
	// Route packet adaptively
	int tmp = p->GetX();
	if ( tmp < addr.x ) {
		tmp += NInfo.width/2;
		if ( tmp <= addr.x )
			Insert( vc, EAST, c );
		else
			Insert( vc, WEST, c );
	}
	else if ( tmp > addr.x ) {
		tmp -= NInfo.width/2;
		if ( tmp <= addr.x )
			Insert( vc, EAST, c );
		else
			Insert( vc, WEST, c );
	}

	tmp = p->GetY();
	if ( tmp < addr.y ) {
		tmp += NInfo.height/2;
		if ( tmp <= addr.y )
			Insert( vc, NORTH, c );
		else
			Insert( vc, SOUTH, c );
	}
	else if ( tmp > addr.y ) {
		tmp -= NInfo.height/2;
		if ( tmp <= addr.y )
			Insert( vc, NORTH, c );
		else
			Insert( vc, SOUTH, c );
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

	if ( p->GetX() == addr.x && p->GetY() == addr.y )
		Insert( vc, HERE, 0 ); 
	else {
		RouteXYEscape( vc, p, 0, 1 );
		if ( NInfo.adaptive )
			RouteAdaptive( vc, p, 2 );
		else
			RouteXY( vc, p, 2 );
	}

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
	switch ( c ) {
		case 0:	
			return ivc_xy[dir][0];
			break;

		case 1:	
			return ivc_xya[dir][0];
			break;

		case 2:	
			return ivc_ad[dir][0];
			break;

		default:	
			assert( false );
			return NULL;
			break;
	}
}

/** Remove
 *  invalidates all the saved routes from the given VirtualChannel
 *
 *  @arg vc Pointer to VirtualChannel that is no longer to be routed
 */
void RouteComputation::Remove ( InputChannel* vc )
{
	_Remove( vc, ivc_xy );
	_Remove( vc, ivc_xya );
	_Remove( vc, ivc_ad );
}

/** _Remove
 *  invalidates all the saved routes from the given VirtualChannel
 *  only in the input output array
 *
 *  @arg vc Pointer to VirtualChannel that is no longer to be routed
 *  @arg arr Poniter to array of VC's that it should be removed from
 */
void RouteComputation::_Remove ( InputChannel* vc, InputChannel* arr[5][13] )
{
	assert( NULL != vc );
	
	for (size_t i = 0; i < 5; i++) {
		// Search for vc
		size_t j;
		for (j = 0; j < 13; j++) {
			if ( NULL == arr[i][j] )
				break;
			if ( vc == arr[i][j] ) {
				// Move everything above it down
				arr[i][j] = NULL;
				for (size_t k = j+1; k < 13; k++) {
					arr[i][k-1] = arr[i][k];
				}
				arr[i][8] = NULL;
				break;
			}
		}
	}
}

