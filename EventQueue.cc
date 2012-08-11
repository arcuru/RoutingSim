#include "Global.h"

typedef struct sll_t {
	Event payload;    //!< Payload to deliver
	EventTarget* t;   //!< Target of event
	uint32_t arrival; //!< Arrival time
	sll_t* next;      //!< Next node in linked list
} sll_t; //!< Node of event queue

EventQueue::EventQueue ()
{
	head = nullptr;
}

EventQueue::~EventQueue ()
{
	Clear();
}

/** Add
 *  schedules a new event
 *
 *  @arg d       Data to be delivered
 *  @arg target  Device that will receive the packet
 *  @arg arrival Arrival time step
 */
void EventQueue::Add ( Event d, EventTarget* target, uint32_t arrival )
{
	assert(target);
	sll_t* n = (sll_t*) malloc(sizeof(sll_t));
	n->payload = d;
	n->t = target;
	n->arrival = arrival;
	n->next = nullptr;
	if (head) {
		if (n->arrival < head->arrival) {
			n->next = head;
			head = n;
		}
		else {
			sll_t* cur = head;
			while (cur->next && cur->next->arrival < n->arrival)
				cur = cur->next;
			n->next = cur->next;
			cur->next = n;
		}
	}
	else
		head = n;
	return ;
}

/** Process
 *  deals with all of the events at the current time step
 */
void EventQueue::Process ( )
{
	while (head && head->arrival <= Global_Time) {
		assert(head->arrival == Global_Time);
		assert(head->t);
		head->t->ProcessEvent(head->payload);
		sll_t* tmp = head->next;
		free(head);
		head = tmp;
	}
	return ;
}

/** Clear
 *  removes all leftover elements in the queue
 *
 */
void EventQueue::Clear ( )
{
	while (head) {
		sll_t* tmp = head->next;
		if ( head->payload.t == CREDIT )
			delete (size_t*)head->payload.d;
		free(head);
		head = tmp;
	}
}

