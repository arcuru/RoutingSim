#ifndef EVENTTARGET_H
#define EVENTTARGET_H

class EventTarget {
public:
  EventTarget() {}
  virtual ~EventTarget() {}

  virtual void ProcessEvent(Event e) = 0;

protected:
private:
};

#endif
