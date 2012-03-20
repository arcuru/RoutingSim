#ifndef OUTPUTCHANNEL_H
#define OUTPUTCHANNEL_H

class InputChannel;

class OutputChannel : public VirtualChannel
{
	public:
		OutputChannel ( );
		OutputChannel ( size_t entries );
		virtual ~OutputChannel ( );

		void ProcessEvent ( Event e );
		bool sendFlit ( );
		void setTarget ( InputChannel* target );
		InputChannel* getTarget ( ) const ;
		void setWB ( InputChannel* writeback );
		InputChannel* getWB ( ) const ;
		bool isReady ( ) const ;
		bool isWorking ( ) const ;

	protected:

	private:
		InputChannel* target;   //!< Saved target of this channel
		InputChannel* wb;     //!< Pointer to VirtualChannel we are receiving data from
		size_t flits_sent;      //!< Number of flits sent out for the current packet
		size_t available_space; //!< Available space in the target buffer
};

#endif

