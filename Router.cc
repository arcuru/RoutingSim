#include "Global.h"

Router::Router ( )
{
	packet_collision = 0;
	RC = new RouteComputation();
	InitBuffers();
}

Router::Router ( Address setAddress )
{
	addr = setAddress;
	packet_collision = 0;
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
	RC->setObuf( EAST, obuf[EAST] );
	RC->setObuf( WEST, obuf[WEST] );
	RC->setObuf( SOUTH, obuf[SOUTH] );
	RC->setObuf( NORTH, obuf[NORTH] );
	RC->setObuf( HERE, obuf[HERE] );
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
}

/** GetTarget
 *  Returns the inputbuffer found at the specified edge
 *
 *  @arg edge Edge of router to return
 *  @return   Target input buffer
 */
InputBuffer* Router::GetTarget ( Direction edge )
{
	return ibuf[edge];
}

/** GetCollisions
 *  returns the total number of packet collisions observed by this router
 *
 * @return  Number of packet collisions
 */
uint32_t Router::GetCollisions ( )
{
	return packet_collision;
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
	// Place new packet into injection queue
	pgen->RandomGenPacket(NInfo.chance);

	// Process all the output buffers
	for (int i=0; i < 4; i++)
		obuf[i]->ProcessBuffer();
	pgen->Process(); // Eject packets from the output queue

	// Traverse packets through the switch
	bool sent[5];
	for (size_t i=0; i < 5; i++)
		sent[i] = false;

	//cout << "(" << addr.x << ", " << addr.y << ") ";

	// Start Receiving from a random side
	size_t start = rand() % 5;
	start = 0;
	for (size_t k=start; k < 5+start; k++) {
		size_t i = k % 5;
		if ( obuf[i]->FlitsRemaining() == obuf[i]->Size() ) {
			//cout << "1 ";
			continue; // This output buffer is full
		}
		if ( obuf[i]->GetP() != NULL ) {
			//cout << "2 ";
			// In the process of moving packet, send individual flit
			InputBuffer* b = obuf[i]->GetPrev();
			if ( b->FlitsRemaining() > 0 ) {
				if ( b->GetFlit()->getPacket() == obuf[i]->GetP() ) {
					Flit* f = b->GetFlit();
					Event e = {DATA, f, b};
					Global_Queue.Add(e, obuf[i], Global_Time+1);
					b->PopFlit();
				}
			}
		}
		else {
			//cout << "3 ";
			// Need to see if there is a valid packet waiting to be routed here
			InputBuffer* b = RC->getNext( i );
			if ( b != NULL ) {
				// Valid InputBuffer exists
				assert(b->FlitsRemaining() > 0);

				Flit* f = b->GetFlit();
				Event e = {DATA, f, b};
				Global_Queue.Add(e, obuf[i], Global_Time+1);
				b->PopFlit();
				RC->Remove( b );
			}
		}
	}
	//cout << endl;

	return ;
}

