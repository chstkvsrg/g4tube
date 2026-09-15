#ifndef FilterMessenger_H
#define FilterMessenger_H 1

#include "globals.hh"
#include "G4UImessenger.hh"

class DetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWith3Vector;

class FilterMessenger: public G4UImessenger
{
  public:
    FilterMessenger(DetectorConstruction* detectorConstruction);
    ~FilterMessenger();

    void SetNewValue(G4UIcommand* command, G4String newValue);

  private:
    DetectorConstruction* fDetectorConstruction;

    G4UIdirectory*                fFilterDirectory;
    G4UIcmdWithAString*           fSetMaterialCmd;
    G4UIcmdWithADoubleAndUnit*    fSetThicknessCmd;
    G4UIcmdWithADoubleAndUnit*    fSetDiameterCmd;
    G4UIcmdWith3VectorAndUnit*    fSetPositionCmd;
    G4UIcmdWith3Vector*           fSetNormalCmd;
};

#endif