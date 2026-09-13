#include "DetectorConstruction.hh"
#include "SensitiveDetector.hh"

#include "G4PhysicalConstants.hh"


#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Trap.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"

#include "G4PVReplica.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4NistManager.hh"
#include "globals.hh"
#include "G4VisAttributes.hh" 
#include "G4SDManager.hh"

#include "AnodeMessenger.hh"
#include "DetectorMessenger.hh"

#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4GeometryManager.hh"

#include <cmath>

DetectorConstruction::DetectorConstruction()
{
    fAnodeMessenger     = new AnodeMessenger(this);
    fDetectorMessenger  = new DetectorMessenger(this);
    fWorldPhys = nullptr;
    fSDCore    = nullptr;
}

DetectorConstruction::~DetectorConstruction()
{
    delete fDetectorMessenger;
    delete fAnodeMessenger;
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{  
  // на пересборке геометрии (reinitializeGeometry) удаляем старую
  if ( fWorldPhys ) {
    anode = nullptr;
    window = nullptr;
    detector = nullptr;
    filter = nullptr;
    fSensitiveStripLogics.clear();

    G4GeometryManager::GetInstance()->OpenGeometry(fWorldPhys);
    G4PhysicalVolumeStore::GetInstance()->Clean();
    G4LogicalVolumeStore::GetInstance()->Clean();
    G4SolidStore::GetInstance()->Clean();

    fWorldPhys = nullptr;
  }

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


  anode    = createAnode(fAnodeParams, world_log);
  window   = createWindow(WindowParams(), world_log);

  // детекторы создаются из макро-конфигурации (команды /xtube/detector/add)
  for (const auto &params : fDetectorParamsVec)
      createDetectorPanel(params, world_log);

  // возвращаем указатель на мировой объем
  fWorldPhys = world_phys;
  return fWorldPhys;
}

G4VPhysicalVolume *DetectorConstruction::createAnode(const DetectorConstruction::AnodeParams &params, G4LogicalVolume *parent)
{
    G4NistManager* nistMan = G4NistManager::Instance();
    G4Material* anodeMaterial = nistMan->FindOrBuildMaterial(params.material);

    G4Box* anode_box = new G4Box("anode", params.size/2, params.size/2, params.thick/2);

    G4LogicalVolume* anode_log = new G4LogicalVolume(anode_box, anodeMaterial, "anode");
    G4RotationMatrix* pRot = new G4RotationMatrix();

    pRot->rotateX(params.angle);

    G4ThreeVector anodePos(0, (params.thick/2)*sin(params.angle), 20*mm + params.thick/2);

    G4VPhysicalVolume* sample_phys = new G4PVPlacement(pRot, anodePos, anode_log, "anode", parent, false, 0);
    anode_log->SetVisAttributes(new G4VisAttributes(G4Colour::Yellow()));

    return sample_phys;
}

G4VPhysicalVolume *DetectorConstruction::createWindow(const DetectorConstruction::WindowParams &params, G4LogicalVolume *parent)
{
    G4NistManager* nistMan = G4NistManager::Instance();
    G4Material* anodeMaterial = nistMan->FindOrBuildMaterial("G4_Be");
    G4Tubs* window_tube = new G4Tubs("window", 0, params.size/2, params.thick/2, 0, 360*deg);
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
    G4Tubs* filter_tube = new G4Tubs("filter", 0, params.size/2, params.thick/2, 0, 360*deg);
    G4LogicalVolume* filter_log = new G4LogicalVolume(filter_tube, anodeMaterial, "filter");

    G4RotationMatrix* pRot = new G4RotationMatrix();
    pRot->rotateX(params.angle);
    G4VPhysicalVolume* window_phys = new G4PVPlacement(pRot, params.pos, filter_log, "filter", parent, false, 0);

    filter_log->SetVisAttributes(new G4VisAttributes(G4Colour::Red()));
    return window_phys;
}

G4VPhysicalVolume *DetectorConstruction::createDetectorPanel(const DetectorConstruction::DetectorParams &params, G4LogicalVolume *parent)
{
    G4NistManager* nistMan = G4NistManager::Instance();
    G4Material* emptyMat = nistMan->FindOrBuildMaterial("G4_Galactic");

    G4double panelX = params.size.x();
    G4double panelY = params.size.y();
    G4double panelZ = params.size.z();
    G4double stripY = panelY / params.nStrips;

    // поворот панели: локальная ось Z (нормаль) совмещается с заданным вектором нормали
    G4ThreeVector n = params.normal.unit();
    G4RotationMatrix* pRot = new G4RotationMatrix();
    if (params.normal.mag2() > 0)
    {
        G4ThreeVector zAxis(0, 0, 1);
        G4double cosA = zAxis.dot(n);
        if (cosA >  0.999999) { /* нормаль уже совпадает с +Z */ }
        else if (cosA < -0.999999) { pRot->rotateX(180*deg); }
        else
        {
            G4double ang = std::acos(cosA);
            G4ThreeVector axis = zAxis.cross(n);
            pRot->rotate(ang, axis);
        }
    }

    // оболочка панели ("большой детектор")
    G4Box* panel_solid = new G4Box(params.name, panelX/2, panelY/2, panelZ/2);
    G4LogicalVolume* panel_log = new G4LogicalVolume(panel_solid, emptyMat, params.name);
    G4VPhysicalVolume* panel_phys = new G4PVPlacement(pRot, params.center, panel_log, params.name, parent, false, 0);

    // полосы: сплошные чувствительные объёмы, реплики вдоль локальной оси Y
    G4String stripName = "DetectorStrip_" + params.name;
    G4Box* strip_solid = new G4Box(stripName, panelX/2, stripY/2, panelZ/2);
    G4LogicalVolume* strip_log = new G4LogicalVolume(strip_solid, emptyMat, stripName);
    new G4PVReplica(stripName, strip_log, panel_log, kYAxis, params.nStrips, stripY, 0);

    fSensitiveStripLogics.push_back(strip_log);

    panel_log->SetVisAttributes(new G4VisAttributes(G4Colour::Blue()));
    strip_log->SetVisAttributes(new G4VisAttributes(G4Colour::Blue()));

    return panel_phys;
}

void DetectorConstruction::ConstructSDandField()
{
    // Sensitive detectors
    if ( fSDCore == nullptr ) {
        G4String SensitiveDetector_name = "SensetiveDetectorCore";
        fSDCore = new SensitiveDetector(SensitiveDetector_name);
        G4SDManager::GetSDMpointer()->AddNewDetector(fSDCore);
    }

    // крепим SD к каждой полосе каждого детектора
    for ( auto* strip_log : fSensitiveStripLogics )
        SetSensitiveDetector(strip_log, fSDCore);
}