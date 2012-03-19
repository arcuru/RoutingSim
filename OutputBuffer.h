#ifndef OUTPUTBUFFER_H
#define OUTPUTBUFFER_H

class InputBuffer;

class OutputBuffer
{
	public:
		OutputBuffer ();
		OutputBuffer ( size_t entries );
		~OutputBuffer ();

		void ProcessBuffer ( );
		void Connect ( InputBuffer* target );
		//void ProcessEvent ( Event e );
		//InputBuffer* GetPrev ( ) const;
		//Packet* GetP ( ) const;
		VirtualChannel* getVC ( size_t channel ) const ;

	protected:

	private:
		//EventTarget* t;            //!< Corresponding Input Buffer for packet destination
		//uint32_t available_space;  //!< Keeping track of how many available spaces are left in corresponding input buffer
		//InputBuffer* ib;           //!< Pointer to InputBuffer we are currently receiving from
		//size_t flits_sent;         //!< Keeps track of the number of flits sent
		//Packet* p;                 //!< Pointer to packet we are currently holding
		VirtualChannel* vc;        //!< Pointer to virtual channels
		size_t channel_count;      //!< Total number of virtual channels

};

#endif
