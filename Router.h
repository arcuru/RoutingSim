#ifndef ROUTER_H
#define ROUTER_H

class Router
{
	public:
		Router ( );
		Router ( Address setAddress );
		~Router ();

		void SetAddr ( Address newAddress );
		void Connect( Direction edge, Router* target);
		InputBuffer* GetTarget ( Direction edge );
		void Process ( );

	protected:

	private:
		void InitBuffers ( );

		InputBuffer** ibuf;        //!< Input buffers situated in this router
		OutputBuffer** obuf;       //!< Output buffers situated in this router
		PacketGen* pgen;           //!< Packet generator to handle I/O for whole network
		Address addr;              //!< Address for this router
		RouteComputation* RC;      //!< Pointer to Route Computation component

};

#endif

