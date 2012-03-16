#ifndef BUFFER_H
#define BUFFER_H

class Buffer : public EventTarget 
{
	public:
		Buffer ();
		Buffer ( size_t entries );
		virtual ~Buffer ();

		size_t Size ( ) const;
		virtual void ProcessEvent ( Event e );
		void InsertFlit ( Flit* p );
		void InsertPacket ( Packet* p );
		void PopFlit ( );
		Flit* GetFlit ( ) const;
		size_t FlitsRemaining ( ) const;
		void SetTarget ( Direction t );
		Direction GetTarget ( ) const;
		Packet* GetPacket ( ) const;

	protected:
		Flit** buf;         //!< Pointer to buffer space
		size_t buf_index;   //!< Pointer to next empty buffer space
		size_t buf_valid;   //!< Pointer to oldest valid data
		size_t buf_size;    //!< Size of buffer
		Direction target;   //!< Saved target of current packet
		Packet* current;    //!< Saved pointer to current packet

	private:

};


#endif

