#ifndef ROUTER_H
#define ROUTER_H

class Router
{
	public:
		Router ( Address setAddress );
		~Router ();

		void Connect( Direction edge, PacketGen* target);
		EventTarget* GetTarget ( Direction edge );
		uint32_t GetCollisions ( );
		void Process ( );

	protected:

	private:
		InputBuffer ibuf[5];       //!< Input buffers situated in this router
		OutputBuffer obuf[5];      //!< Output buffers situated in this router
		Address addr;              //!< Address for this router
		uint32_t packet_collision; //!< Count of packet collisions

};

#endif

