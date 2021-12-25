#include "EventAction.hh"
#include "RunAction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"

EventAction::EventAction() : eventsProcessed_(0)
{
}

EventAction::~EventAction()
{
}


void EventAction::EndOfEventAction(const G4Event* event)
{
	eventsProcessed_++;

	//if (!(eventsProcessed_ % 1000))
		//std::cout << eventsProcessed_ << " events processed" << std::endl;
}

