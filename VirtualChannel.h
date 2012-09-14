#ifndef VIRTUALCHANNEL_H
#define VIRTUALCHANNEL_H

class VirtualChannel : public EventTarget 
{
	public:
		VirtualChannel ();
		VirtualChannel ( uint8_t entries );
		virtual ~VirtualChannel ();

		void setSize ( uint8_t entries );
		uint8_t Size ( ) const;
		virtual void ProcessEvent ( Event e ) = 0;
		void InsertFlit ( Flit* p );
		void InsertPacket ( Packet* p );
		void PopFlit ( );
		Flit* GetFlit ( ) const;
		uint8_t FlitsRemaining ( ) const;
		Packet* GetPacket ( ) const;

	protected:
		Flit** buf;         //!< Pointer to buffer space
		uint8_t buf_index;   //!< Pointer to next empty buffer space
		uint8_t buf_valid;   //!< Pointer to oldest valid data
		uint8_t buf_size;    //!< Size of buffer
		Packet* cur_packet;    //!< Saved pointer to current packet

	private:

};


#endif

