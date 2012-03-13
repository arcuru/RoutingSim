#ifndef PACKET_H
#define PACKET_H

class Packet
{
	public:
		Packet ( Address destination, Address origin, bool head, uint32_t data);
		~Packet ();

		uint8_t GetX ( ) const;
		uint8_t GetY ( ) const;
		uint8_t GetOriginX ( ) const;
		uint8_t GetOriginY ( ) const;
		bool GetHead ( ) const;
		uint32_t GetData ( ) const;

	protected:

	private:
		uint64_t info;     //!< 8 bytes of information that comprise the packet

};


#endif
