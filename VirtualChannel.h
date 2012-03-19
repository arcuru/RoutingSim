#ifndef BUFFER_H
#define BUFFER_H

typedef enum VC_t {
	VC_OUT,
	VC_IN,
	VC_INJ,
	VC_EJ
} VC_t;

class RouteComputation;

class VirtualChannel : public EventTarget 
{
	public:
		VirtualChannel ();
		VirtualChannel ( size_t entries );
		virtual ~VirtualChannel ();

		void setID ( size_t id );
		size_t getID ( ) const;
		void setSize ( size_t entries );
		void setRC ( RouteComputation* rcomp );
		size_t Size ( ) const;
		virtual void ProcessEvent ( Event e );
		void InsertFlit ( Flit* p );
		void InsertPacket ( Packet* p );
		void PopFlit ( );
		Flit* GetFlit ( ) const;
		void sendFlit ( );
		size_t FlitsRemaining ( ) const;
		void SetTarget ( VirtualChannel* t );
		void setType ( VC_t t );
		void setWB ( VirtualChannel* writeback );
		VirtualChannel* GetTarget ( ) const;
		Packet* GetPacket ( ) const;
		VirtualChannel* getWB ( ) const;
		size_t getSpace ( ) const;

	protected:
		Flit** buf;         //!< Pointer to buffer space
		size_t buf_index;   //!< Pointer to next empty buffer space
		size_t buf_valid;   //!< Pointer to oldest valid data
		size_t buf_size;    //!< Size of buffer
		size_t flits_sent;         //!< Keeps track of the number of flits sent
		uint32_t available_space;  //!< Keeping track of how many available spaces are left in corresponding VC
		VirtualChannel* target;   //!< Saved target of current packet
		Packet* current;    //!< Saved pointer to current packet
		VirtualChannel* wb; //!< Writeback connection
		VC_t type;          //!< Saved type of this VC
		RouteComputation* RC; //!< Pointer to Route Computation component
		size_t channel_id;  //!< the saved channel id of this VC

	private:

};


#endif

