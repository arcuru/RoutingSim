#ifndef INPUTBUFFER_H
#define INPUTBUFFER_H

class RouteComputation;

class InputBuffer : public Buffer
{
	public:
		InputBuffer ();
		InputBuffer ( size_t entries );
		~InputBuffer ();

		void setRC ( RouteComputation* rcomp );
		void WriteBack ( OutputBuffer* write_back );
		void PopFlit ( );
		void ProcessEvent ( Event e );

	protected:

	private:
		OutputBuffer* obuf;  //!< Paired Output Buffer for writeback information
		RouteComputation* RC; //!< Pointer to Route Computation component

};

#endif

