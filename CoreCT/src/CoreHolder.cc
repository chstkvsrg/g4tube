//Detector construction source

#include "CoreHolder.hh"
#include "defines.hh"

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

CoreHolder::CoreHolder(G4LogicalVolume* MotherLogic,
                       G4ThreeVector Position,
                       G4RotationMatrix* Rotation,
                       G4double Holder_length,
                       G4double Core_diameter,
                       G4double Rubber_thikness,
                       G4double OilGap_thikness,
                       G4double Coat_thikness,
                       G4double Road_dist,
                       G4double Road_diameter)
    :
      fMother(MotherLogic),
      fPosition(Position), fRotation(Rotation),
      fHolder_length(Holder_length), fCore_diameter(Core_diameter), fRubber_thikness(Rubber_thikness),
      fOilGap_thikness(OilGap_thikness), fCoat_thikness(Coat_thikness), fRoad_dist(Road_dist),fRoad_diameter(Road_diameter),
      fEmpty_mat(nullptr),fOil_mat(nullptr), fSi_mat(nullptr), fRubber_mat(nullptr), fAl_mat(nullptr), fTi_mat(nullptr)
{
    if (fRotation == 0 || fRotation == NULL || fRotation == nullptr)
    {
        fRotation = new G4RotationMatrix();
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CoreHolder::~CoreHolder()
{ 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CoreHolder::Construct()
{
    // materials
    DefineMaterials();

    // volumes
    return DefineVolumes();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CoreHolder::DefineMaterials()
{

    /////////////////////////// Materials creation//////////////////////////////////

    //First includings

    /*G4NistManager* nist = G4NistManager::Instance();

    //First includings
    
    G4Element* C = Elem("C");
    G4Element* H = Elem("H");
    G4Element* O = Elem("O");
    G4Element* N = Elem("N");
    
    // ULTEM = Polyetherimide (PEI)
    G4Material* ultem = new G4Material("ULTEM", 1.27*g/cm3, 4, kStateSolid);
    ultem->AddElement(C, 37); //natoms
    ultem->AddElement(H, 24);
    ultem->AddElement(O, 6);
    ultem->AddElement(N, 2);*/
    
    ////////////////////////Materials choose////////////////////////////////////

    fEmpty_mat = Mat("G4_AIR");
    fOil_mat = new G4Material("OIL", 0.8*g/cm3, Mat("G4_PARAFFIN"), kStateLiquid);
    fSi_mat = Mat("G4_Si");;  		// pointer for G4_galactic - for all intermediate volumes
    fRubber_mat = Mat("G4_RUBBER_NATURAL");            // Oil material with optic
    fAl_mat = Mat("G4_Al");		// pointer for scintillator material
    fTi_mat = Mat("G4_Ti"); 		// pointer for light guide material

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void CoreHolder::DefineVolumes()
{
    //
    //sizes
    //

    //holder

    G4double CoreHolderX = 0;

    if ((2*fRoad_dist+fRoad_diameter*std::sqrt(2))/(std::sqrt(2)) >= 2*(fCore_diameter/2+fRubber_thikness+fOilGap_thikness+fCoat_thikness))
    {
        CoreHolderX = (2*fRoad_dist+fRoad_diameter*std::sqrt(2))/(std::sqrt(2));
    }
    else
    {
        CoreHolderX = 2*(fCore_diameter/2+fRubber_thikness+fOilGap_thikness+fCoat_thikness);
    }

    G4double CoreHolderY = CoreHolderX;
    G4double CoreHolderZ = fHolder_length;

    //Coat
    G4double Coat_Rmin = fCore_diameter/2+fRubber_thikness+fOilGap_thikness;                                              // its mother valume so minimal radius not need
    G4double Coat_Rmax = Coat_Rmin+fCoat_thikness;
    G4double Coat_Length = fHolder_length;

    //Gap
    G4double OilGap_Rmin = fCore_diameter/2+fRubber_thikness;
    G4double OilGap_Rmax = OilGap_Rmin+fOilGap_thikness;
    G4double OilGap_Length = fHolder_length;

    //Rubber
    G4double Rubber_Rmin = fCore_diameter/2;
    G4double Rubber_Rmax = Rubber_Rmin+fRubber_thikness;
    G4double Rubber_Length = fHolder_length;

    //Core

    G4double Core_Rmin = 0;
    G4double Core_Rmax = fCore_diameter/2;
    G4double Core_Length = fHolder_length;

    //Roads
    G4double Road_Rmin = 0;
    G4double Road_Rmax = fRoad_diameter/2;
    G4double Road_Length = fHolder_length;

    //
    //volume build
    //

    // CoreHolder

    G4Box* CoreHolder_plus
            = new G4Box("CoreHolderPlus",           // its name
                        CoreHolderX/2, CoreHolderY/2, CoreHolderZ/2); // its size

    G4Tubs* CoreHolder_minus
            = new G4Tubs("CoreHolderMinus",           // its name
                         Core_Rmin,
                         Core_Rmax,
                         Core_Length/2+0.1*mm,
                         0,
                         2*M_PI); // its size

    G4VSolid* CoreHolder_solid = new G4SubtractionSolid("CoreHolder",
                                                        CoreHolder_plus,
                                                        CoreHolder_minus);

    G4LogicalVolume* CoreHolder_logic
            = new G4LogicalVolume(
                CoreHolder_solid,           // its solid
                fEmpty_mat,  // its material
                "CoreHolder");         // its name

    new G4PVPlacement(
                G4Transform3D(*fRotation,                // no rotation
                              fPosition),                     // position
                CoreHolder_logic,          // its logical volume
                "CoreHolder",          // its name
                fMother,                // its mother  volume
                false,            // no boolean operation
                0,                // copy number
                fCheckOverlaps);  // checking overlaps

    //Coat

    G4Tubs* Coat_solid
            = new G4Tubs("Coat",           // its name
                         Coat_Rmin,
                         Coat_Rmax,
                         Coat_Length/2,
                         0,
                         2*M_PI); // its size

    G4LogicalVolume* Coat_logic
            = new G4LogicalVolume(
                Coat_solid,           // its solid
                fAl_mat,  // its material
                "Coat");         // its name

    new G4PVPlacement(
                0,                // no rotation
                G4ThreeVector(),  // at (0,0,0)
                Coat_logic,          // its logical volume
                "Coat",          // its name
                CoreHolder_logic,                // its mother  volume
                false,            // no boolean operation
                0,                // copy number
                fCheckOverlaps);  // checking overlaps

    //OilGap

    G4Tubs* OilGap_solid
            = new G4Tubs("OilGap",           // its name
                         OilGap_Rmin,
                         OilGap_Rmax,
                         OilGap_Length/2,
                         0,
                         2*M_PI); // its size

    G4LogicalVolume* OilGap_logic
            = new G4LogicalVolume(
                OilGap_solid,           // its solid
                fOil_mat,  // its material
                "OilGap");         // its name

    new G4PVPlacement(
                0,                // no rotation
                G4ThreeVector(),  // a
                OilGap_logic,          // its logical volume
                "OilGap",          // its name
                CoreHolder_logic,                // its mother  volume
                false,            // no boolean operation
                0,                // copy number
                fCheckOverlaps);  // checking overlaps

    //Rubber

    G4Tubs* Rubber_solid
            = new G4Tubs("Rubber",           // its name
                         Rubber_Rmin,
                         Rubber_Rmax,
                         Rubber_Length/2,
                         0,
                         2*M_PI); // its size

    G4LogicalVolume* Rubber_logic
            = new G4LogicalVolume(
                Rubber_solid,           // its solid
                fRubber_mat,  // its material
                "Rubber");         // its name

    new G4PVPlacement(
                0,                // no rotation
                G4ThreeVector(),  // at (0,0,0)-ScintDepth/2
                Rubber_logic,          // its logical volume
                "Rubber",          // its name
                CoreHolder_logic,                // its mother  volume
                false,            // no boolean operation
                0,                // copy number
                fCheckOverlaps);  // checking overlaps

    //Roads

    G4Tubs* Road_solid = nullptr;
    G4LogicalVolume* Road_logic = nullptr;
    G4ThreeVector Road_pose = G4ThreeVector();

    for (G4int Road_index = 0; Road_index <= 3; Road_index++)
    {
        G4String Index = std::to_string(Road_index);
        Road_pose = G4ThreeVector(fRoad_dist*std::cos(M_PI/4+M_PI/2*Road_index),fRoad_dist*std::sin(M_PI/4+M_PI/2*Road_index),0);
        Road_solid
                = new G4Tubs("Road"+Index,           // its name
                             Road_Rmin,
                             Road_Rmax,
                             Road_Length/2,
                             0,
                             2*M_PI); // its size

        Road_logic
                = new G4LogicalVolume(
                    Road_solid,           // its solid
                    fTi_mat,  // its material
                    "Road"+Index);         // its name

        new G4PVPlacement(
                    0,                // no rotation
                    Road_pose,  //
                    Road_logic,          // its logical volume
                    "Road"+Index,          // its name
                    CoreHolder_logic,                // its mother  volume
                    false,            // no boolean operation
                    0,                // copy number
                    fCheckOverlaps);  // checking overlaps

        auto visAttributes = new G4VisAttributes(G4Colour(0.0, 1.0, 1.0));
        visAttributes->SetVisibility(true);
        Road_logic->SetVisAttributes(visAttributes);
    }

    //Visulisation parameters detector//

    auto visAttributes = new G4VisAttributes(G4Colour(0.45,0.25,0.0));
    visAttributes->SetVisibility(true);
    Rubber_logic->SetVisAttributes(visAttributes);

    visAttributes = new G4VisAttributes(G4Colour(0.0, 1.0, 1.0));
    visAttributes->SetVisibility(true);
    OilGap_logic->SetVisAttributes(visAttributes);

    visAttributes = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0));
    visAttributes->SetVisibility(true);
    Coat_logic->SetVisAttributes(visAttributes);

    visAttributes = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0, 0));
    visAttributes->SetVisibility(true);
    CoreHolder_logic->SetVisAttributes(visAttributes);


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
