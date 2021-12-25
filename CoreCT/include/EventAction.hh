#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
class G4Event;

class EventAction: public G4UserEventAction
{
  public:
    EventAction();
   ~EventAction();

    void EndOfEventAction(const G4Event*);

private:
    int eventsProcessed_;
};

#endif

