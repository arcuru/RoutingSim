#include "Global.h"
#include <cstdlib>
#include <vector>

typedef struct node_t {
  Event payload;  //!< Payload to deliver
  EventTarget *t; //!< Target of event
} node_t;         //!< Node of event queue

typedef struct timedQueue_t {
  std::vector<node_t> q; //!< Queue object
  uint32_t time;         //!< Time  of delivery
} timedQueue_t;          //!< Struct for holding queues

EventQueue::EventQueue() {
  current = new timedQueue_t;
  next = new timedQueue_t;
  current->time = Global_Time;
  next->time = Global_Time + 1;
}

EventQueue::~EventQueue() {
  delete current;
  delete next;
}

/** Add
 *  schedules a new event
 *
 *  @arg d       Data to be delivered
 *  @arg target  Device that will receive the packet
 *  @arg arrival Arrival time step
 */
void EventQueue::Add(Event d, EventTarget *target, uint32_t arrival) {
  assert(target);
  node_t tmp;
  tmp.payload = d;
  tmp.t = target;
  if (arrival == current->time)
    current->q.push_back(tmp);
  else if (arrival == next->time)
    next->q.push_back(tmp);
  else {
    // First time we've had an event with this timestep
    assert(Global_Time != current->time);
    assert(current->q.size() ==
           0); // Checking that we cleared out the 'current' queue
    assert(next->time == Global_Time);

    // Swap pointers
    std::swap(current, next);
    next->time = Global_Time + 1;

    assert(arrival == next->time); // Code will fail here if any event takes 2
    next->q.push_back(tmp);
  }
}

/** Process
 *  deals with all of the events at the current time step
 */
void EventQueue::Process() {
  if (current->time != Global_Time) {
    assert(current->q.size() == 0);
    std::swap(current, next);
    next->time = Global_Time + 1;
  }
  for (node_t tmp : current->q) {
    tmp.t->ProcessEvent(tmp.payload);
  }
  current->q.clear();
}

/** Clear
 *  removes all leftover elements in the queue and creates new ones with
 * global_time of 0
 *
 */
void EventQueue::Clear() {
  delete current;
  delete next;
  current = new timedQueue_t;
  next = new timedQueue_t;
  current->time = 0;
  next->time = 1;
}
