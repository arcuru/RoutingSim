#include "Global.h"

Router::Router ( )
{
	RC = new RouteComputation();
	InitBuffers();
}

Router::Router ( Address setAddress )
{
	addr = setAddress;
	RC = new RouteComputation();
	SetAddr(addr);
	InitBuffers();
}

Router::~Router ()
{
	for (int i=0; i < 5; i++) {
		if ( HERE != i ) {
			delete ibuf[i];
			delete obuf[i];
		}
	}
	free(ibuf);
	free(obuf);
	delete pgen;
	delete RC;
}

/** InitBuffers
 *  handles initialization for the packet generator, input / output buffers,
 *  and Route Computation device
 */
void Router::InitBuffers ( )
{
	pgen = new PacketGen( );
	ibuf = (InputBuffer**) malloc(sizeof(InputBuffer*) * 5);
	obuf = (OutputBuffer**) malloc(sizeof(OutputBuffer*) * 5);
	for (int i=0; i < 5; i++) {
		if ( HERE == i ) {
			ibuf[i] = pgen->GetInjection();
			obuf[i] = pgen->GetEjection();
		}
		else {
			ibuf[i] = new InputBuffer();
			obuf[i] = new OutputBuffer();
		}
		ibuf[i]->setRC( RC );
	}
}

/** SetAddr
 *  sets the value of the address for this router
 *
 */
void Router::SetAddr ( Address newAddress )
{
	addr = newAddress;
	pgen->SetAddr(addr);
	RC->setAddr(addr);
	for (size_t i=0; i < 5; i++)
		ibuf[i]->setAddr( addr );
}

/** GetTarget
 *  Returns the inputbuffer found at the specified edge
 *
 *  @arg edge Edge of router to return
 *  @return   Target input buffer
 */
InputBuffer* Router::GetTarget ( Direction edge ) const
{
	return ibuf[edge];
}

/** GetPacketGen
 *  Returns the packet generator
 *
 *  @return Pointer to the packet generator
 */
PacketGen* Router::GetPacketGen ( ) const
{
	return pgen;
}

/** Connect
 *  sets up the connection between this routers output buffer
 *  and target routers input buffer including writeback of target
 *  routers input buffer
 *
 *  @arg edge   Edge of this router we are connecting
 *  @arg target Target router
 */
void Router::Connect( Direction edge, Router* target)
{
	
	switch ( edge ) {
		case NORTH:	
			obuf[edge]->Connect(target->GetTarget(SOUTH));
			((InputBuffer*)target->GetTarget(SOUTH))->WriteBack(obuf[edge]);
			break;

		case SOUTH:	
			obuf[edge]->Connect(target->GetTarget(NORTH));
			((InputBuffer*)target->GetTarget(NORTH))->WriteBack(obuf[edge]);
			break;

		case EAST:	
			obuf[edge]->Connect(target->GetTarget(WEST));
			((InputBuffer*)target->GetTarget(WEST))->WriteBack(obuf[edge]);
			break;

		case WEST:	
			obuf[edge]->Connect(target->GetTarget(EAST));
			((InputBuffer*)target->GetTarget(EAST))->WriteBack(obuf[edge]);
			break;

		default:
			assert(false); // Invalid edge value
			break;
	}
	return ;
}

/** Process
 *  handles a single cycle of router operation
 *
 */
void Router::Process ( )
{
	// Calculate buffer utilization
	for (size_t i=0; i < 4; i++) {
		aibuf_util += ibuf[i]->getIC( 1 )->FlitsRemaining();
		eibuf_util += ibuf[i]->getIC( 0 )->FlitsRemaining();
		aobuf_util += obuf[i]->getOC( 1 )->FlitsRemaining();
		eobuf_util += obuf[i]->getOC( 0 )->FlitsRemaining();
	}

	// Process all the output buffers
	for (int i=0; i < 4; i++)
		obuf[i]->ProcessBuffer();
	pgen->Process(); // Eject packets from the output queue

	//cout << "(" << addr.x << ", " << addr.y << ") ";

	// Start Receiving from a random side
	for (int j = 2; j >= 0; j--) {
		for (size_t k=0; k < 5; k++) {
			size_t i = k % 5;
			if ( HERE == i && 0 != j) {
				//cout << "0 ";
				continue;
			}
			OutputChannel* vc = obuf[i]->getOC( j );
			if ( vc->FlitsRemaining() == vc->Size() ) {
				//cout << "1 ";
				continue; // This VC is full
			}
			else if ( vc->isWorking() == true ) {
				//cout << "2 ";
				// In the process of moving packet, send individual flit
				InputChannel* b = vc->getWB();
				if ( b->FlitsRemaining() > 0 ) {
					if ( b->GetFlit()->getPacket() == vc->GetPacket() ) {
						//assert( b->GetFlit()->isHead() == false );
						if ( b->GetFlit()->isHead() == false )
							b->sendFlit();
					}
				}
			}
			else if ( vc->isReady() ) {
				//cout << "3 ";
				// Need to see if there is a valid packet waiting to be routed here
				InputChannel* b = RC->getNext( i, j );
				if ( b != nullptr ) {
					// Valid VC exists
					assert(b->FlitsRemaining() > 0);
					assert( b->GetFlit()->isHead() );
					b->setTarget( vc );
					b->sendFlit();
					RC->Remove( b );
				}
			}
			else
				assert(false);
		}
	}
	//cout << endl;
	
	return ;
}

