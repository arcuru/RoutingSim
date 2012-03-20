#ifndef ROUTECOMPUTATION_H
#define ROUTECOMPUTATION_H

class RouteComputation : public EventTarget
{
	public:
		RouteComputation ();
		RouteComputation ( Address a );
		~RouteComputation ();

		void setAddr ( Address a );
		void ProcessEvent ( Event e );
		InputChannel* getNext ( size_t dir, size_t c ) const ;
		void Remove ( InputChannel* vc  );

	protected:

	private:
		void Insert ( InputChannel* vc, Direction d, size_t c );
		void _Remove ( InputChannel* vc, InputChannel* arr[5][9] );

		Address addr;          //!< Address of router
		InputChannel* ivc_xy[5][9]; //!< Pointer to InputBuffers that desire each OutputBuffer
		InputChannel* ivc_ad[5][9]; //!< Pointer to input VC's that desire each adaptive output VC
};


#endif
