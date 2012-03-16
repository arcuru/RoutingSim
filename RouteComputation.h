#ifndef ROUTECOMPUTATION_H
#define ROUTECOMPUTATION_H

class RouteComputation : public EventTarget
{
	public:
		RouteComputation ();
		RouteComputation ( Address a );
		~RouteComputation ();

		void setAddr ( Address a );
		void setObuf ( Direction edge, OutputBuffer* ob );
		void Insert ( InputBuffer* ib, Direction d );
		void ProcessEvent ( Event e );
		InputBuffer* getNext ( size_t dir ) const;
		void Remove ( InputBuffer* ib );

	protected:

	private:
		Address addr;          //!< Address of router
		OutputBuffer* obuf[5]; //!< Pointers to all OutputBuffers
		InputBuffer* ibuf[5][5]; //!< Pointer to InputBuffers that desire each OutputBuffer
};


#endif
