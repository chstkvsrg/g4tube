#include "FilterMessenger.hh"

#include "DetectorConstruction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UImanager.hh"

FilterMessenger::FilterMessenger(DetectorConstruction* detectorConstruction)
  : G4UImessenger(),
    fDetectorConstruction(detectorConstruction)
{
  fFilterDirectory = new G4UIdirectory("/xtube/filter/");
  fFilterDirectory->SetGuidance("Filter (circular plate) construction parameters");

  fSetMaterialCmd = new G4UIcmdWithAString("/xtube/filter/setMaterial", this);
  fSetMaterialCmd->SetGuidance("Set filter material (G4 NIST name, e.g. G4_Al)");
  fSetMaterialCmd->SetParameterName("material", false);
  fSetMaterialCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fSetThicknessCmd = new G4UIcmdWithADoubleAndUnit("/xtube/filter/setThickness", this);
  fSetThicknessCmd->SetGuidance("Set filter plate thickness");
  fSetThicknessCmd->SetParameterName("thickness", false);
  fSetThicknessCmd->SetRange("thickness > 0");
  fSetThicknessCmd->SetDefaultUnit("mm");
  fSetThicknessCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fSetDiameterCmd = new G4UIcmdWithADoubleAndUnit("/xtube/filter/setDiameter", this);
  fSetDiameterCmd->SetGuidance("Set filter plate diameter");
  fSetDiameterCmd->SetParameterName("diameter", false);
  fSetDiameterCmd->SetRange("diameter > 0");
  fSetDiameterCmd->SetDefaultUnit("mm");
  fSetDiameterCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fSetPositionCmd = new G4UIcmdWith3VectorAndUnit("/xtube/filter/setPosition", this);
  fSetPositionCmd->SetGuidance("Set filter center position");
  fSetPositionCmd->SetParameterName("x", "y", "z", false);
  fSetPositionCmd->SetDefaultUnit("mm");
  fSetPositionCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fSetNormalCmd = new G4UIcmdWith3Vector("/xtube/filter/setNormal", this);
  fSetNormalCmd->SetGuidance("Set normal direction to the filter plane (unit vector)");
  fSetNormalCmd->SetParameterName("nx", "ny", "nz", false);
  fSetNormalCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

FilterMessenger::~FilterMessenger()
{
  delete fSetNormalCmd;
  delete fSetPositionCmd;
  delete fSetDiameterCmd;
  delete fSetThicknessCmd;
  delete fSetMaterialCmd;
  delete fFilterDirectory;
}

void FilterMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if (command == fSetMaterialCmd)
    fDetectorConstruction->SetFilterMaterial(newValue);
  else if (command == fSetThicknessCmd)
    fDetectorConstruction->SetFilterThickness(fSetThicknessCmd->GetNewDoubleValue(newValue));
  else if (command == fSetDiameterCmd)
    fDetectorConstruction->SetFilterDiameter(fSetDiameterCmd->GetNewDoubleValue(newValue));
  else if (command == fSetPositionCmd)
    fDetectorConstruction->SetFilterPosition(fSetPositionCmd->GetNew3VectorValue(newValue));
  else if (command == fSetNormalCmd)
    fDetectorConstruction->SetFilterNormal(fSetNormalCmd->GetNew3VectorValue(newValue));
}