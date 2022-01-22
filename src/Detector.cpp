//Detector construction source

#include "Detector.hh"

#include "G4NistManager.hh"
#include "G4SDManager.hh"

#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4AnyType.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4SubtractionSolid.hh"
#include "G4PVReplica.hh"
#include "G4ReflectionFactory.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"

#include "G4LogicalBorderSurface.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalSkinSurface.hh"

#include <cmath>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Detector::Detector(G4LogicalVolume* MotherLogic,
                       G4ThreeVector Position,
                       G4RotationMatrix* Rotation,
                       G4double Detector_length,
                       G4double DetectorCell_sizeX,
                       G4double DetectorCell_sizeY,
                       G4double DetectorCell_numberX,
                       G4double DetectorCell_numberY)
    : G4VUserDetectorConstruction(),
      fMother(MotherLogic),
      fPosition(Position), fRotation(Rotation),
      fDetector_length(Detector_length), fDetectorCell_sizeX(DetectorCell_sizeX), fDetectorCell_sizeY(DetectorCell_sizeY),
      fDetectorCell_numberX(DetectorCell_numberX), fDetectorCell_numberY(DetectorCell_numberY),
      fEmpty_mat(nullptr)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Detector::~Detector()
{ 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* Detector::Construct()
{
    // materials
    DefineMaterials();

    // volumes
    return DefineVolumes();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Detector::DefineMaterials()
{


    /////////////////////////// Materials creation//////////////////////////////////

    //First includings

    G4NistManager* nist = G4NistManager::Instance();

    ////////////////////////Materials choose////////////////////////////////////

    fEmpty_mat = nist->FindOrBuildMaterial("G4_Galactic");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


G4VPhysicalVolume* Detector::DefineVolumes()
{
    //
    //Sizes
    //

    //Detector

    G4double DetectorX = fDetectorCell_numberX*fDetectorCell_sizeX;
    G4double DetectorY = fDetectorCell_numberY*fDetectorCell_sizeY;
    G4double DetectorZ = fDetector_length;

    //Detector strip
    G4double DetectorStripX = fDetectorCell_numberX*fDetectorCell_sizeX;
    G4double DetectorStripY = fDetectorCell_sizeY;
    G4double DetectorStripZ = fDetector_length;

    //Detector Cell
    G4double DetectorCellX = fDetectorCell_sizeX;
    G4double DetectorCellY = fDetectorCell_sizeY;
    G4double DetectorCellZ = fDetector_length;

    //
    //Volume build
    //

    // Detector

    G4Box* Det_solid
            = new G4Box("Detector",           // its name
                        DetectorX/2, DetectorY/2, DetectorZ/2); // its size

    G4LogicalVolume* Det_logic
            = new G4LogicalVolume(
                Det_solid,           // its solid
                fEmpty_mat,  // its material
                "Detector");         // its name

    G4VPhysicalVolume* Det_phys = nullptr;

    if (fRotation == 0)
    {
        Det_phys
                = new G4PVPlacement(
                    0,//G4Transform3D(*fRotation,                // no rotation
                    fPosition,//),                     // position
                    Det_logic,          // its logical volume
                    "Detector",          // its name
                    fMother,                // its mother  volume
                    false,            // no boolean operation
                    0,                // copy number
                    fCheckOverlaps);  // checking overlaps
    }
    else
    {
        Det_phys
                = new G4PVPlacement(
                    G4Transform3D(*fRotation,                // no rotation
                    fPosition),                     // position
                    Det_logic,          // its logical volume
                    "Detector",          // its name
                    fMother,                // its mother  volume
                    false,            // no boolean operation
                    0,                // copy number
                    fCheckOverlaps);  // checking overlaps
    }

    //DetectorStrip

    G4Box* DetStrip_solid
            = new G4Box("DetectorStrip",           // its name
                        DetectorStripX/2, DetectorStripY/2, DetectorStripZ/2); // its size

    G4LogicalVolume* DetStrip_logic
            = new G4LogicalVolume(
                DetStrip_solid,           // its solid
                fEmpty_mat,  // its material
                "DetectorStrip");         // its name

    new G4PVReplica("DetectorStrip",
                        DetStrip_logic,     //its logic
                        Det_logic,            //its mother logic
                        kYAxis,                  //Axis of replica
                        fDetectorCell_numberY,               // Number of repicas
                        fDetectorCell_sizeY,           // distns betwenn replecated logics
                        0);                      // offset=0

    //DetectorCell

    G4Box* DetCell_solid
            = new G4Box("DetectorCell",           // its name
                        DetectorCellX/2, DetectorCellY/2, DetectorCellZ/2); // its size

    G4LogicalVolume* DetCell_logic
            = new G4LogicalVolume(
                DetCell_solid,           // its solid
                fEmpty_mat,  // its material
                "DetectorCell");         // its name

    new G4PVReplica("DetectorCell",
                        DetCell_logic,     //its logic
                        DetStrip_logic,            //its mother logic
                        kXAxis,                  //Axis of replica
                        fDetectorCell_numberX,               // Number of repicas
                        fDetectorCell_sizeX,           // distns betwenn replecated logics
                        0);                      // offset=0

    //Visulisation parameters detector//

    auto visAttributes = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0));
    visAttributes->SetVisibility(true);
    //visAttributes->SetDaughtersInvisible(false);
    Det_logic->SetVisAttributes(visAttributes);

    visAttributes = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0));
    visAttributes->SetVisibility(true);
    DetStrip_logic->SetVisAttributes(visAttributes);

    visAttributes = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0));
    visAttributes->SetVisibility(true);
    DetCell_logic->SetVisAttributes(visAttributes);

    /*visAttributes = new G4VisAttributes(G4Colour(1.0,1.0,1.0));
    visAttributes->SetVisibility(true);
    Scint_logic->SetVisAttributes(visAttributes);

    visAttributes = new G4VisAttributes(G4Colour(1.0,1.0,1.0));
    visAttributes->SetVisibility(true);
    LightPipe_logic->SetVisAttributes(visAttributes);

    visAttributes = new G4VisAttributes(G4Colour(1.0,1.0,1.0));
    visAttributes->SetVisibility(false);
    visAttributes->SetDaughtersInvisible(true);
    SiPM_logic->SetVisAttributes(visAttributes);*/

    return Det_phys;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
