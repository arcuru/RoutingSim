#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

struct timedQueue_t;

class EventQueue
{
	public:
		EventQueue ( );
		~EventQueue ( );

		void Add ( Event e, EventTarget* target, uint32_t arrival );
		void Process ( );
		void Clear ( );

	protected:

	private:
        timedQueue_t* current; //!< Holder for queue of current timestep
        timedQueue_t* next;    //!< Holder for queue of next timestep
};

#endif

