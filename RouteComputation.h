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
		void ProcessEvent ( Event e );
		VirtualChannel* getNext ( size_t dir, size_t c ) const ;
		void Remove ( VirtualChannel* vc  );

	protected:

	private:
		void Insert ( VirtualChannel* vc, Direction d, size_t c );
		void _Remove ( VirtualChannel* vc, VirtualChannel* arr[5][5] );

		Address addr;          //!< Address of router
		VirtualChannel* ovc_xy[5]; //!< Pointers to output XY VC's
		VirtualChannel* ivc_xy[5][5]; //!< Pointer to InputBuffers that desire each OutputBuffer
		VirtualChannel* ovc_ad[5]; //!< Pointers to output VC's for adaptive
		VirtualChannel* ivc_ad[5][5]; //!< Pointer to input VC's that desire each adaptive output VC
};


#endif
