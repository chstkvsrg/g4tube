#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcommand.hh"
#include "G4UImanager.hh"
#include "G4SystemOfUnits.hh"

#include <sstream>

DetectorMessenger::DetectorMessenger(DetectorConstruction* detectorConstruction)
  : G4UImessenger(),
    fDetectorConstruction(detectorConstruction)
{
  fDetectorDirectory = new G4UIdirectory("/xtube/detector/");
  fDetectorDirectory->SetGuidance("Detector construction parameters");

  fAddCmd = new G4UIcmdWithAString("/xtube/detector/add", this);
  fAddCmd->SetGuidance("Add a detector strip-panel.\n"
                       "  Usage: /xtube/detector/add <name> <cx> <cy> <cz> "
                       "<sx> <sy> <sz> <nx> <ny> <nz> [<nStrips>]\n"
                       "  name    : detector name (must be unique)\n"
                       "  cx,cy,cz: center position (mm)\n"
                       "  sx,sy,sz: panel dimensions (mm), local Y is split into strips\n"
                       "  nx,ny,nz: direction of the normal to the panel plane\n"
                       "  nStrips : number of strips along local Y (default 6)");
  fAddCmd->SetParameterName("spec", false);
  fAddCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fClearCmd = new G4UIcommand("/xtube/detector/clear", this);
  fClearCmd->SetGuidance("Remove all configured detectors");
  fClearCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fListCmd = new G4UIcommand("/xtube/detector/list", this);
  fListCmd->SetGuidance("Print currently configured detectors");
  fListCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

DetectorMessenger::~DetectorMessenger()
{
  delete fListCmd;
  delete fClearCmd;
  delete fAddCmd;
  delete fDetectorDirectory;
}

void DetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if (command == fAddCmd)
  {
    std::istringstream iss(newValue);
    G4String name;
    G4double cx, cy, cz, sx, sy, sz, nx, ny, nz;
    G4int nStrips = 6;
    if (!(iss >> name >> cx >> cy >> cz >> sx >> sy >> sz >> nx >> ny >> nz))
    {
      G4cerr << "DetectorMessenger: /xtube/detector/add: bad arguments (expected: name cx cy cz sx sy sz nx ny nz [nStrips])"
             << G4endl;
      return;
    }
    iss >> nStrips;   // optional trailing parameter
    if (nStrips < 1) nStrips = 1;

    DetectorConstruction::DetectorParams p;
    p.name    = name;
    p.center  = G4ThreeVector(cx*mm, cy*mm, cz*mm);
    p.size    = G4ThreeVector(sx*mm, sy*mm, sz*mm);
    p.normal  = G4ThreeVector(nx, ny, nz);
    p.nStrips = nStrips;
    fDetectorConstruction->AddDetector(p);
    G4cout << "DetectorMessenger: added detector '" << name
           << "' (" << sx << "x" << sy << "x" << sz << " mm"
           << ", " << nStrips << " strips)" << G4endl;
  }
  else if (command == fClearCmd)
  {
    fDetectorConstruction->ClearDetectors();
    G4cout << "DetectorMessenger: detector list cleared" << G4endl;
  }
  else if (command == fListCmd)
  {
    const auto& v = fDetectorConstruction->GetDetectors();
    if (v.empty())
      G4cout << "DetectorMessenger: no detectors configured" << G4endl;
    for (size_t i = 0; i < v.size(); ++i)
    {
      const auto& d = v[i];
      G4cout << " [" << i << "] " << d.name
             << "  center=(" << d.center.x()/mm << ", " << d.center.y()/mm
             << ", " << d.center.z()/mm << ") mm"
             << "  size=(" << d.size.x()/mm << ", " << d.size.y()/mm
             << ", " << d.size.z()/mm << ") mm"
             << "  normal=(" << d.normal.x() << ", " << d.normal.y()
             << ", " << d.normal.z() << ")"
             << "  nStrips=" << d.nStrips
             << G4endl;
    }
  }
}