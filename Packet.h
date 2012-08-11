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
		uint8_t GetHash ( ) const;
		uint32_t GetCreated ( ) const;
		void SetCreated ( uint32_t creation );

	protected:

	private:
		uint8_t CalcHash ( ) const;

		Flit* flits;       //!< Array of flits that comprise this packet
		size_t flit_count; //!< Number of flits contained in packet
		uint32_t created;
	
#ifndef NDEBUG
	public:
		void AddRouter ( Address addr );
	
	private:
		Address route[20];
		size_t route_pointer;
#endif

};


#endif
