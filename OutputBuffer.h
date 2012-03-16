#ifndef OUTPUTBUFFER_H
#define OUTPUTBUFFER_H

class InputBuffer;

class OutputBuffer : public Buffer
{
	public:
		OutputBuffer ();
		OutputBuffer ( size_t entries );
		~OutputBuffer ();

		void ProcessBuffer ( );
		void Connect ( EventTarget* target );
		void ProcessEvent ( Event e );
		InputBuffer* GetPrev ( ) const;
		Packet* GetP ( ) const;

	protected:

	private:
		EventTarget* t;            //!< Corresponding Input Buffer for packet destination
		uint32_t available_space;  //!< Keeping track of how many available spaces are left in corresponding input buffer
		InputBuffer* ib;           //!< Pointer to InputBuffer we are currently receiving from
		size_t flits_sent;         //!< Keeps track of the number of flits sent
		Packet* p;                 //!< Pointer to packet we are currently holding

};

#endif
