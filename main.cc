#include "Global.h"

#define IND(x,y) (((y) * NInfo.width) + (x))

uint64_t Global_Time;
NetworkInfo NInfo;
EventQueue Global_Queue;
uint64_t packet_injections = 0;
uint64_t packets_blocked = 0;
uint64_t packets_sent = 0;
uint64_t packet_ejections = 0;
uint64_t packet_latency = 0;
uint64_t link_util = 0;
uint64_t aibuf_util = 0;
uint64_t eibuf_util = 0;
uint64_t aobuf_util = 0;
uint64_t eobuf_util = 0;
void RunSimulation( uint32_t simulation_end, double injection_chance );

int main ( int argc, char** argv )
{
	uint32_t simulation_end = END_TIME;
	double injection_chance = 0.6;

	// Initialize default network settings
	NInfo.width = 8;
	NInfo.height = 8;
	NInfo.dest_func = RAND;

	// Generate headers for the output
	cout << "Packet Injections, ";
	cout << "Packet Ejections, ";
	cout << "Offered Load, ";
	cout << "Network Throughput, ";
	cout << "Packet Latency, ";
	cout << "Link Utilization, ";
	cout << "Adaptive Input Buffer Utilization, ";
	cout << "Adaptive Output Buffer Utilization, ";
	cout << "Escape Input Buffer Utilization, ";
	cout << "Escape Output Buffer Utilization, ";
	cout << "Simulation Time, ";
	cout << "Injection Chance" << endl;
	if (argc > 1) {
		NInfo.dest_func = (Destination_Function)atoi(argv[1]);
		if (argc > 2) {
			simulation_end = atoi(argv[2]);
			if (argc > 3) {
				injection_chance = atof(argv[3]);
				RunSimulation( simulation_end, injection_chance );
				return EXIT_SUCCESS;
			}
		}
	}
	// Set number of tests
	uint32_t tests = 100;

	// Iterate and run tests
	for (int i = tests; i >= 0; i--) {
		injection_chance = ((double)i) / tests;
		RunSimulation( simulation_end, injection_chance / 12 );
		Global_Queue.Clear();
	}

	return EXIT_SUCCESS;
}

/** RunSimulation
 *  execute a full simulation of a single router + 4 PacketGens with the
 *  provided parameters. Prints output.
 *
 *  @args simulation_end   Total time to run the simulation
 *  @args injection_chance Chance of each Packet Generator injecting a
 *                          packet each cycle
 */
void RunSimulation( uint32_t simulation_end, double injection_chance )
{
	// Seed random number generator
	srand(4);
	//srand(time(NULL));
	
	// Set injection chance
	NInfo.chance = injection_chance;

	// Create Router and packet generators
	Router* sim = new Router[NInfo.width * NInfo.height];
	for (uint8_t i=0; i < NInfo.width; i++) {
		for (uint8_t j=0; j < NInfo.height; j++) {
			Address addr = {i, j};
			sim[IND(i,j)].SetAddr( addr );
			sim[IND(i,j)].Connect( NORTH, &sim[IND(i,(j+1)%NInfo.height)] );
			sim[IND(i,j)].Connect( EAST, &sim[IND((i+1)%NInfo.width,j)] );
			sim[IND(i,j)].Connect( SOUTH, &sim[IND(i,(j+NInfo.height-1)%NInfo.height)] );
			sim[IND(i,j)].Connect( WEST, &sim[IND((i+NInfo.width-1)%NInfo.width,j)] );
		}
	}

	for (Global_Time = 0; Global_Time < simulation_end; Global_Time++) {
		Global_Queue.Process(); // Process all packet movements in the queue
		for (uint8_t i=0; i < NInfo.width; i++) {
			for (uint8_t j=0; j < NInfo.height; j++) {
				sim[IND(i,j)].Process(); // Process each router for a time step
			}
		}
	}

	cout << packet_injections << ", ";
	cout << packet_ejections << ", ";
	cout << ((double)packet_injections)/simulation_end << ", ";
	cout << ((16 * (double)packet_ejections)/(NInfo.width*NInfo.height))/simulation_end << ", ";
	cout << (double)packet_latency / packet_ejections << ", ";
	cout << (double)link_util / (NInfo.width * NInfo.height * 4 * simulation_end) << ", ";
	cout << (double)aibuf_util / (NInfo.width * NInfo.height * 4 * 4 * simulation_end) << ", ";
	cout << (double)aobuf_util / (NInfo.width * NInfo.height * 4 * 1 * simulation_end) << ", ";
	cout << (double)eibuf_util / (NInfo.width * NInfo.height * 4 * 4 * simulation_end) << ", ";
	cout << (double)eobuf_util / (NInfo.width * NInfo.height * 4 * 1 * simulation_end) << ", ";
	cout << simulation_end << ", ";
	cout << injection_chance << endl;
	packet_injections = 0;
	packet_ejections = 0;
	packets_blocked = 0;
	packets_sent = 0;
	packet_latency = 0;
	link_util = 0;
	aibuf_util = 0;
	eibuf_util = 0;
	aobuf_util = 0;
	eobuf_util = 0;

	// Memory cleanup
	delete [] sim;

	return ;
}
