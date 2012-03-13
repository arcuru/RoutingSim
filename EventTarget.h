#ifndef EVENTTARGET_H
#define EVENTTARGET_H

class EventTarget
{
	public:
		EventTarget ()  {}
		~EventTarget () {}

		virtual void ProcessEvent ( Event e ) = 0;

	protected:

	private:

};

#endif

