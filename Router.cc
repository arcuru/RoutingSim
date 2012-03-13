#include "Global.h"

Router::Router ( )
{
	packet_collision = 0;
	InitBuffers();
}

Router::Router ( Address setAddress )
{
	addr = setAddress;
	packet_collision = 0;
	SetAddr(addr);
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
 *  handles initialization for the packet generator and input / output buffers
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
	pgen->RandomGenPacket(NetworkInfo.chance);

	// Process all the output buffers
	for (int i=0; i < 4; i++)
		obuf[i]->ProcessBuffer();
	pgen->Process(); // Eject packets from the output queue

	// Traverse packets through the switch
	bool sent[5];
	for (int i=0; i < 5; i++)
		sent[i] = false;

	// Start sending from a random side
	int start = rand() % 5;
	for (int k=start; k < 5+start; k++) {
		int i = k % 5;
		if (ibuf[i]->Routed() > 0) {
			// Valid packets are routed
			Packet* p = ibuf[i]->GetPacket();
			Direction d = ibuf[i]->GetRoute();
			if ( false == sent[d] ) {
				if (obuf[d]->PacketsRemaining() < obuf[d]->Size()-1) {
					Event e = {DATA, p};
					Global_Queue.Add(e, obuf[d], Global_Time+1);
					sent[d] = true;
					ibuf[i]->PopPacket();
				}
				else {
					packet_collision++;
				}
			}
			else {
				// Outgoing buffer full
				packet_collision++;
			}
		}
	}

	// Route packets
	for (int i=0; i < 5; i++)
		ibuf[i]->RoutePacket( addr );

	return ;
}

