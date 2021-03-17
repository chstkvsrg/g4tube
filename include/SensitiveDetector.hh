#ifndef SENSITIVEDETECTOR_H
#define SENSITIVEDETECTOR_H

#include "G4VSensitiveDetector.hh"
#include <vector>

class G4Step;
class G4HCofThisEvent;

/// The hits are accounted in hits in ProcessHits() function which is called
/// by Geant4 kernel at each step. A hit is created with each step with non zero
/// energy deposit.

class SensitiveDetector : public G4VSensitiveDetector
{

public:

    SensitiveDetector(const G4String& name);
    virtual ~SensitiveDetector();

    // methods from base class
    virtual void   Initialize(G4HCofThisEvent*);
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* );
    virtual void   EndOfEvent(G4HCofThisEvent*);

private:

};


#endif // SENSITIVEDETECTOR_H
