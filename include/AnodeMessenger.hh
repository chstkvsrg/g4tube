#ifndef AnodeMessenger_H
#define AnodeMessenger_H 1

#include "globals.hh"
#include "G4UImessenger.hh"

class DetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;

class AnodeMessenger: public G4UImessenger
{
  public:
    AnodeMessenger(DetectorConstruction* detectorConstruction);
    ~AnodeMessenger();

    void SetNewValue(G4UIcommand* command, G4String newValue);

  private:
    DetectorConstruction* fDetectorConstruction;

    G4UIdirectory*          fAnodeDirectory;
    G4UIcmdWithAString*     fSetMaterialCmd;
    G4UIcmdWithADoubleAndUnit* fSetThicknessCmd;
    G4UIcmdWithADoubleAndUnit* fSetAngleCmd;
};

#endif