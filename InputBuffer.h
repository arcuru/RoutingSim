#ifndef INPUTBUFFER_H
#define INPUTBUFFER_H

class RouteComputation;

class InputBuffer
{
	public:
		InputBuffer ();
		InputBuffer ( size_t entries );
		~InputBuffer ();

		void setRC ( RouteComputation* rcomp );
		void WriteBack ( OutputBuffer* write_back );
		//void PopFlit ( );
		//void ProcessEvent ( Event e );
		VirtualChannel* getVC ( size_t channel ) const ;

	protected:

	private:
		//OutputBuffer* obuf;  //!< Paired Output Buffer for writeback information
		//RouteComputation* RC; //!< Pointer to Route Computation component
		VirtualChannel* vc;   //!< Pointer to virtual channels
		size_t channel_count; //!< Total number of virtual channels
};

#endif

