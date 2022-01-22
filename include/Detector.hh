//Detector construction header

#ifndef Detector_h
#define Detector_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolume.hh"
#include "globals.hh"
#include "G4Material.hh"


// materials and geometry.

class Detector : public G4VUserDetectorConstruction
{
public:
    Detector(G4LogicalVolume* MotherLogic,
               G4ThreeVector Position,
               G4RotationMatrix* Rotation,
               G4double Detector_length,
               G4double DetectorCell_sizeX,
               G4double DetectorCell_sizeY,
               G4double DetectorCell_numberX,
               G4double DetectorCell_numberY
               );
    virtual ~Detector();

public:
    virtual G4VPhysicalVolume* Construct();

private:
	// methods of materials and volumes separate
	//
	void DefineMaterials();
	G4VPhysicalVolume* DefineVolumes();

    //mother volume
    G4LogicalVolume* fMother;

    //Position
    G4ThreeVector fPosition;

    //Rotation
    G4RotationMatrix* fRotation;

    //size parameters
    G4double fDetector_length;
    G4double fDetectorCell_sizeX;
    G4double fDetectorCell_sizeY;
    G4double fDetectorCell_numberX;
    G4double fDetectorCell_numberY;
	
	//pointers to material
    G4Material* fEmpty_mat;

	G4bool  fCheckOverlaps; // option to activate checking of volumes overlaps
}; 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

