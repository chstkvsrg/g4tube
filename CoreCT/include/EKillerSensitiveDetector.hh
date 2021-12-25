#pragma once

#include "G4VSensitiveDetector.hh"
class G4Step;
class RunAction;

class HistManager;

class EKillerSensitiveDetector: public G4VSensitiveDetector
{
  public:
	EKillerSensitiveDetector(G4String);
    ~EKillerSensitiveDetector();

    G4bool ProcessHits(G4Step*, G4TouchableHistory*);
    
  private:



};

