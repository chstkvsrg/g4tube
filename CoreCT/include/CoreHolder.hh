//Detector construction header

#ifndef CoreHolder_h
#define CoreHolder_h 1

#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "globals.hh"
#include "G4Material.hh"


// materials and geometry.

class CoreHolder
{
public:
    CoreHolder(G4LogicalVolume* MotherLogic,
               G4ThreeVector Position,
               G4RotationMatrix* Rotation,
               G4double Holder_length,
               G4double Core_diameter,
               G4double Rubber_thikness,
               G4double OilGap_thikness,
               G4double Coat_thikness,
               G4double Road_dist,
               G4double Road_diameter
               );
    ~CoreHolder();

public:
    void Construct();

private:
	// methods of materials and volumes separate
	//
	void DefineMaterials();
    void DefineVolumes();

    //mother volume
    G4LogicalVolume* fMother;

    //Position
    G4ThreeVector fPosition;

    //Rotation
    G4RotationMatrix* fRotation;

    //size parameters
    G4double fHolder_length;
    G4double fCore_diameter;
    G4double fRubber_thikness;
    G4double fOilGap_thikness;
    G4double fCoat_thikness;
    G4double fRoad_dist;
    G4double fRoad_diameter;
	
	//pointers to material
    G4Material* fEmpty_mat;
    G4Material* fOil_mat;
    G4Material* fSi_mat;
    G4Material* fRubber_mat;
    G4Material* fAl_mat;
    G4Material* fTi_mat;

    G4bool  fCheckOverlaps = true; // option to activate checking of volumes overlaps
}; 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

