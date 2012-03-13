#include "Global.h"


Router::Router ( Address setAddress )
{
	addr = setAddress;
	packet_collision = 0;
}

Router::~Router ()
{
}

/** GetTarget
 *  Returns the inputbuffer found at the specified edge
 *
 *  @arg edge Edge of router to return
 *  @return   Target input buffer
 */
EventTarget* Router::GetTarget ( Direction edge )
{
	return &ibuf[edge];
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
 *  and target routers input buffer
 *
 *  @arg edge   Edge of this router we are connecting
 *  @arg target Target packet generator
 */
void Router::Connect( Direction edge, PacketGen* target)
{
	obuf[edge].Connect(target->GetTarget());
	((InputBuffer*)target->GetTarget())->WriteBack(&obuf[edge]);
	return ;
}

/** Process
 *  handles a single cycle of router operation
 *
 */
void Router::Process ( )
{
	// Process all the output buffers
	for (int i=0; i < 4; i++)
		obuf[i].ProcessBuffer();
	if ( obuf[HERE].PacketsRemaining() != 0 ) {
		obuf[HERE].PopPacket();
		assert(obuf[HERE].PacketsRemaining() == 0);
	}

	// Traverse packets through the switch
	bool sent[5];
	for (int i=0; i < 5; i++)
		sent[i] = false;

	// Start sending from a random side
	int start = rand() % 5;
	for (int k=start; k < 5+start; k++) {
		int i = k % 5;
		if (ibuf[i].Routed() > 0) {
			// Valid packets are routed
			Packet* p = ibuf[i].GetPacket();
			Direction d = ibuf[i].GetRoute();
			if ( false == sent[d] ) {
				if (obuf[d].PacketsRemaining() < obuf[d].Size()-1) {
					Event e = {DATA, p};
					Global_Queue.Add(e, &obuf[d], Global_Time+1);
					sent[d] = true;
					ibuf[i].PopPacket();
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
		ibuf[i].RoutePacket( addr );

	return ;
}

