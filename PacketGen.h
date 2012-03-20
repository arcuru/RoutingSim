#ifndef PACKETGEN_H
#define PACKETGEN_H

class PacketGen
{
	public:
		PacketGen ( );
		PacketGen ( Address setAddress );
		~PacketGen ();

		void SetAddr ( Address setAddress );
		void SetDirection ( Direction edge );
		InputBuffer* GetInjection ( ) const;
		OutputBuffer* GetEjection ( ) const;
		void GenPacket ( );
		void RandomGenPacket ( double chances );
		void Process ( );


	protected:

	private:
		InputBuffer* ibuf;       //!< Ejection queue 
		OutputBuffer* obuf;      //!< Injection queue
		Direction dir;           //!< Direction this generator is pointing
		Address addr;            //!< Address of this generator
		uint32_t packets_out;    //!< Total packets that arrived in the ejection queue
		uint32_t packets_sent;   //!< Total packets sent injected into the network
		uint32_t packets_blocked;//!< Total packets blocked from being inserted
		uint32_t flits_received; //!< Number of flits received in the current packet

};

#endif

