#ifndef PACKET_H
#define PACKET_H

class Packet
{
	public:
		Packet ( Address destination, Address origin, bool head, uint32_t data);
		~Packet ();

		uint8_t GetX ( );
		uint8_t GetY ( );
		uint8_t GetOriginX ( );
		uint8_t GetOriginY ( );
		bool GetHead ( );
		uint32_t GetData ( );

	protected:

	private:
		uint64_t info;     //!< 8 bytes of information that comprise the packet

};


#endif
