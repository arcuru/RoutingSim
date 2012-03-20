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
		InputChannel* getIC ( size_t channel ) const ;

	protected:

	private:
		InputChannel* ic;   //!< Pointer to input channels
		size_t channel_count; //!< Total number of virtual channels
};

#endif

