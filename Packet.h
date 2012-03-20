#ifndef PACKET_H
#define PACKET_H

class Packet
{
	public:
		Packet ( Address destination, Address origin, size_t packet_size );
		~Packet ();

		Flit* GetFlit ( size_t index ) const;
		size_t GetSize ( ) const;
		uint8_t GetX ( ) const;
		uint8_t GetY ( ) const;
		uint32_t GetOriginX ( ) const;
		uint32_t GetOriginY ( ) const;
		bool GetHead ( ) const;
		uint32_t GetData ( ) const;
		uint32_t GetCreated ( ) const;
		void AddRouter ( Address addr );

	protected:

	private:
		Flit* flits;       //!< Array of flits that comprise this packet
		size_t flit_count; //!< Number of flits contained in packet
		uint64_t info;     //!< 8 bytes of information that comprise the packet
		uint32_t created;
		Address route[16];
		size_t route_pointer;
};


#endif
