#ifndef DetectorMessenger_H
#define DetectorMessenger_H 1

#include "globals.hh"
#include "G4UImessenger.hh"

class DetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcommand;

class DetectorMessenger: public G4UImessenger
{
  public:
    DetectorMessenger(DetectorConstruction* detectorConstruction);
    ~DetectorMessenger();

    void SetNewValue(G4UIcommand* command, G4String newValue);

  private:
    DetectorConstruction* fDetectorConstruction;

    G4UIdirectory*      fDetectorDirectory;
    G4UIcmdWithAString* fAddCmd;
    G4UIcommand*        fClearCmd;
    G4UIcommand*        fListCmd;
};

#endif