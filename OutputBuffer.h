#ifndef OUTPUTBUFFER_H
#define OUTPUTBUFFER_H

class InputBuffer;

class OutputBuffer
{
	public:
		OutputBuffer ();
		explicit OutputBuffer ( size_t entries );
		~OutputBuffer ();

		void ProcessBuffer ( );
		void Connect ( InputBuffer* target );
		OutputChannel* getOC ( size_t channel ) const ;

	protected:

	private:
		OutputChannel* oc;        //!< Pointer to virtual channels
		size_t channel_count;      //!< Total number of virtual channels
		size_t last_sent;         //!< Saves the channel that was most recently sent

};

#endif
