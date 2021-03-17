#include "DetectorConstruction.hh"
#include "SensitiveDetector.hh"

#include "G4PhysicalConstants.hh"


#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Trap.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"

#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4NistManager.hh"
#include "globals.hh"
#include "G4VisAttributes.hh" 
#include "G4SDManager.hh"

DetectorConstruction::DetectorConstruction() {}

DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume* DetectorConstruction::Construct()
{  
  G4NistManager* nistMan = G4NistManager::Instance();

  // --- volumes ---
  // создаем геометрию
  G4double saSize = 3*cm;    // размер образца  
  G4double detDiam = 3*cm;   // диаметр детектора
  G4double detThick = 2*cm;  // толщина детектора
  G4double gap1 = 5*mm;      // расстояние от источника до образца
  G4double gap2 = 25*mm;     // расстояние от образца до детектора  

  G4Material* Empty_mat = nistMan->FindOrBuildMaterial("G4_Galactic");  

  // мировой объем в виде параллелепипеда
  G4Box* world_box = new G4Box("world", (saSize + detDiam)/2 + 1*cm, (saSize + detDiam)/2 + 1*cm, gap1 + gap2 + detThick/2 + 1*cm);
  // заполняем его воздухом
  G4LogicalVolume* world_log = new G4LogicalVolume(world_box, Empty_mat, "world");
  // и помещаем в начало координат
  G4VPhysicalVolume* world_phys = new G4PVPlacement(0, G4ThreeVector(), world_log, "world", 0, false, 0);


  anode    = createAnode(AnodeParams(), world_log);
  window   = createWindow(WindowParams(), world_log);
  detector = createDetector(DetectorParams(), world_log);
  filter   = createFilter(FilterParams(), world_log);
  
//  // --- visualisation ---
//  // отключаем отображение мирового объема
//  //world_log->SetVisAttributes(G4VisAttributes::Visible);
//  // а для образца и детектора задаем цвета отображения
//  // (по умолчанию все элементы геометрии окрашены в серый цвет)



  // возвращаем указатель на мировой объем
  return world_phys;
}

G4VPhysicalVolume *DetectorConstruction::createAnode(const DetectorConstruction::AnodeParams &params, G4LogicalVolume *parent)
{
    G4NistManager* nistMan = G4NistManager::Instance();
    G4Material* anodeMaterial = nistMan->FindOrBuildMaterial("G4_W");

    G4Box* anode_box = new G4Box("anode", params.size/2, params.size/2, params.thick/2);

    G4LogicalVolume* anode_log = new G4LogicalVolume(anode_box, anodeMaterial, "anode");
    G4RotationMatrix* pRot = new G4RotationMatrix();

    pRot->rotateX(params.angle);    

    G4VPhysicalVolume* sample_phys = new G4PVPlacement(pRot, params.pos, anode_log, "anode", parent, false, 0);
    anode_log->SetVisAttributes(new G4VisAttributes(G4Colour::Yellow()));

    return sample_phys;
}

G4VPhysicalVolume *DetectorConstruction::createWindow(const DetectorConstruction::WindowParams &params, G4LogicalVolume *parent)
{
    G4NistManager* nistMan = G4NistManager::Instance();
    G4Material* anodeMaterial = nistMan->FindOrBuildMaterial("G4_Be");
    G4Tubs* window_tube = new G4Tubs("window", 0, params.size/2, params.thick, 0, 360*deg);
    G4LogicalVolume* window_log = new G4LogicalVolume(window_tube, anodeMaterial, "window");

    G4RotationMatrix* pRot = new G4RotationMatrix();
    pRot->rotateX(params.angle);
    G4VPhysicalVolume* window_phys = new G4PVPlacement(pRot, params.pos, window_log, "window", parent, false, 0);

    window_log->SetVisAttributes(new G4VisAttributes(G4Colour::White()));
    return window_phys;
}

G4VPhysicalVolume *DetectorConstruction::createFilter(const DetectorConstruction::FilterParams &params, G4LogicalVolume *parent)
{
    G4NistManager* nistMan = G4NistManager::Instance();
    G4Material* anodeMaterial = nistMan->FindOrBuildMaterial("G4_Al");
    G4Tubs* filter_tube = new G4Tubs("filter", 0, params.size/2, params.thick, 0, 360*deg);
    G4LogicalVolume* filter_log = new G4LogicalVolume(filter_tube, anodeMaterial, "filter");

    G4RotationMatrix* pRot = new G4RotationMatrix();
    pRot->rotateX(params.angle);
    G4VPhysicalVolume* window_phys = new G4PVPlacement(pRot, params.pos, filter_log, "filter", parent, false, 0);

    filter_log->SetVisAttributes(new G4VisAttributes(G4Colour::Red()));
    return window_phys;
}

G4VPhysicalVolume *DetectorConstruction::createDetector(const DetectorConstruction::DetectorParams &params, G4LogicalVolume *parent)
{
    G4NistManager* nistMan = G4NistManager::Instance();
    G4Material* detMaterial = nistMan->FindOrBuildMaterial("G4_Galactic");
    // детектор в виде цилиндра
    G4Tubs* det_tube = new G4Tubs("detector", 0, params.size/2, params.thick/2, 0, 360*deg);
    G4LogicalVolume* det_log = new G4LogicalVolume(det_tube, detMaterial, "detector");

    // помещаем его в мировой объем
    G4RotationMatrix* pRot = new G4RotationMatrix();
    pRot->rotateX(params.angle);
    G4VPhysicalVolume* det_phys = new G4PVPlacement(pRot, params.pos, det_log, "detector", parent, false, 0);

    det_log->SetVisAttributes(new G4VisAttributes(G4Colour::Blue()));

    return det_phys;
}

void DetectorConstruction::ConstructSDandField()
{
    // Sensitive detectors

    G4String SensitiveDetector_name = "SensetiveDetectorCore";
    SensitiveDetector* SDCore = new SensitiveDetector(SensitiveDetector_name);
    G4SDManager::GetSDMpointer()->AddNewDetector(SDCore);

    // Setting SD to all logical volumes with the same name
    SetSensitiveDetector("detector", SDCore);
}
