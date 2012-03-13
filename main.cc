#include "Global.h"

#define IND(x,y) (((y) * NetworkInfo.width) + (x))

uint32_t Global_Time;
NInfo NetworkInfo;
EventQueue Global_Queue;
uint32_t packet_injections = 0;
uint32_t packet_ejections = 0;
void RunSimulation( uint32_t simulation_end, double injection_chance );

int main ( int argc, char** argv )
{
	uint32_t simulation_end = END_TIME;
	double injection_chance = 0.6;

	// Generate lots of output
	cout << "Packet Injections, ";
	cout << "Packet Ejections, ";
	cout << "Offered Load, ";
	cout << "Switch Throughput, ";
	cout << "Packet Collisions, ";
	cout << "Simulation Time, ";
	cout << "Injection Chance" << endl;
	if (argc > 1) {
		simulation_end = atoi(argv[1]);
		if (argc > 2) {
			injection_chance = atof(argv[2]);
		}
		RunSimulation( simulation_end, injection_chance );
	}
	else {
		// Set number of tests
		uint32_t tests = 100;

		// Iterate and run tests
		for (int i = tests; i >= 0; i--) {
			injection_chance = ((double)i) / tests;
			RunSimulation( simulation_end, injection_chance );
			Global_Queue.Clear();
		}
	}

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
	//srand(4);
	srand(time(NULL));

	// Initialize network settings
	NetworkInfo.width = 2;
	NetworkInfo.height = 2;
	NetworkInfo.chance = injection_chance;

	// Create Router and packet generators
	Router* sim = new Router[NetworkInfo.width * NetworkInfo.height];
	for (uint8_t i=0; i < NetworkInfo.width; i++) {
		for (uint8_t j=0; j < NetworkInfo.height; j++) {
			Address addr = {i, j};
			sim[IND(i,j)].SetAddr( addr );
			if ( j+1 < NetworkInfo.height )
				sim[IND(i,j)].Connect( NORTH, &sim[IND(i,j+1)] );
			if ( i+1 < NetworkInfo.width )
				sim[IND(i,j)].Connect( EAST, &sim[IND(i+1,j)] );
			if ( j > 0 )
				sim[IND(i,j)].Connect( SOUTH, &sim[IND(i,j-1)] );
			if ( i > 0 )
				sim[IND(i,j)].Connect( WEST, &sim[IND(i-1,j)] );
		}
	}

	for (Global_Time = 0; Global_Time < simulation_end; Global_Time++) {
		Global_Queue.Process(); // Process all packet movements in the queue
		for (uint8_t i=0; i < NetworkInfo.width; i++) {
			for (uint8_t j=0; j < NetworkInfo.height; j++) {
				sim[IND(i,j)].Process(); // Process each router for a time step
			}
		}
	}

	cout << packet_injections << ", ";
	cout << packet_ejections << ", ";
	cout << ((double)packet_injections)/simulation_end << ", ";
	cout << ((double)packet_ejections)/simulation_end << ", ";
	//cout << sim.GetCollisions() << ", ";
	cout << simulation_end << ", ";
	cout << injection_chance << endl;
	packet_injections = 0;
	packet_ejections = 0;

	return ;
}
