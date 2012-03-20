#ifndef INPUTCHANNEL_H
#define INPUTCHANNEL_H

class RouteComputation;

class InputChannel : public VirtualChannel
{
	public:
		InputChannel ( );
		InputChannel ( size_t entries );
		~InputChannel ( );

		void setRC ( RouteComputation* rcomp );
		void ProcessEvent ( Event e );
		void sendFlit ( );
		void setTarget ( OutputChannel* target );
		void setWB ( OutputChannel* writeback );
		void schedRC ( );

	protected:

	private:
		RouteComputation* RC; //!< Pointer to Route Computation component
		OutputChannel* target;   //!< Saved target of current packet
		OutputChannel* wb;   //!< Saved write back of this channel
};

#endif

