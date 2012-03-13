#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

struct sll_t;

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
		sll_t* head;  //!< Pointer to head of linked list

};

#endif

