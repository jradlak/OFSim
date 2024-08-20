#pragma once

#include "EventDispatcher.h"

#include <thread>

namespace ofsim_infrastructure
{
	class SMEventsListener
	{
	public:
		SMEventsListener(EventDispatcher &_eventDispatcher) 
			: eventDispatcher(_eventDispatcher)
		{
			smeThread = std::make_unique<std::thread>(&SMEventsListener::recieveEvents, this);
		}

		void stop() { shouldListen = false; }
		StateEvent lastEvent() { return _lastEvent; }
		int numberOfEventsRecieved() { return _numberOfEventsRecieved; }

		~SMEventsListener() { shouldListen = false; }

	private:
		bool shouldListen = true;

		EventDispatcher& eventDispatcher;
		std::unique_ptr<std::thread> smeThread;

		StateEvent _lastEvent;
		int _numberOfEventsRecieved = 0;

		void recieveEvents()
		{
			while (shouldListen)
			{
				_numberOfEventsRecieved++;
				_lastEvent = eventDispatcher.recieveSMEvent();				
			}
		}
	};
}