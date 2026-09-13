#include "AnodeMessenger.hh"

#include "DetectorConstruction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UImanager.hh"

AnodeMessenger::AnodeMessenger(DetectorConstruction* detectorConstruction)
  : G4UImessenger(),
    fDetectorConstruction(detectorConstruction)
{
  fAnodeDirectory = new G4UIdirectory("/xtube/anode/");
  fAnodeDirectory->SetGuidance("Anode construction parameters");

  fSetMaterialCmd = new G4UIcmdWithAString("/xtube/anode/setMaterial", this);
  fSetMaterialCmd->SetGuidance("Set anode material (G4 NIST name, e.g. G4_W)");
  fSetMaterialCmd->SetParameterName("material", false);
  fSetMaterialCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fSetThicknessCmd = new G4UIcmdWithADoubleAndUnit("/xtube/anode/setThickness", this);
  fSetThicknessCmd->SetGuidance("Set anode plate thickness");
  fSetThicknessCmd->SetParameterName("thickness", false);
  fSetThicknessCmd->SetRange("thickness > 0");
  fSetThicknessCmd->SetDefaultUnit("mm");
  fSetThicknessCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fSetAngleCmd = new G4UIcmdWithADoubleAndUnit("/xtube/anode/setAngle", this);
  fSetAngleCmd->SetGuidance("Set anode plate tilt angle");
  fSetAngleCmd->SetParameterName("angle", false);
  fSetAngleCmd->SetRange("angle > 0 && angle < 90");
  fSetAngleCmd->SetDefaultUnit("deg");
  fSetAngleCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

AnodeMessenger::~AnodeMessenger()
{
  delete fSetAngleCmd;
  delete fSetThicknessCmd;
  delete fSetMaterialCmd;
  delete fAnodeDirectory;
}

void AnodeMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if (command == fSetMaterialCmd)
    fDetectorConstruction->SetAnodeMaterial(newValue);
  else if (command == fSetThicknessCmd)
    fDetectorConstruction->SetAnodeThickness(fSetThicknessCmd->GetNewDoubleValue(newValue));
  else if (command == fSetAngleCmd)
    fDetectorConstruction->SetAnodeAngle(fSetAngleCmd->GetNewDoubleValue(newValue));
}