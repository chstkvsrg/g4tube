//Detector construction header

#ifndef CoreVoxel_h
#define CoreVoxel_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "globals.hh"
#include "G4Material.hh"
#include "G4VisAttributes.hh"
#include "G4PVParameterised.hh"
#include "G4PhantomParameterisation.hh"
#include "CoreVoxelParameterisation.hh"

#include <vector>
#include <utility>
#include <tuple>

// materials and geometry.

class CoreVoxel
{
public:
    CoreVoxel(G4LogicalVolume* MotherLogic,
               G4ThreeVector Position,
               G4RotationMatrix* Rotation);

    CoreVoxel(G4LogicalVolume* MotherLogic,
               G4ThreeVector Position,
               G4RotationMatrix* Rotation,
               G4double HalfX,
               G4double HalfY,
               G4double HalfZ,
               G4int VoxelsX_number,
               G4int VoxelsY_number,
               G4int VoxelsZ_number,
               G4double Grayscale_overage,
               G4Material* Reference_mat,
               std::vector<G4Material*> Materials,
               size_t* Materials_indexes
               );
    virtual ~CoreVoxel();

public:
    //Set methods
    void SetVoxelsVariables(G4double HalfX, G4double HalfY, G4double HalfZ,
                            G4int VoxelsX_number, G4int VoxelsY_number, G4int VoxelsZ_number);
    void SetVoxelsVariables(G4String Path);

    void SetMaterials(std::vector<G4Material*> Materials, G4Material* Reference_mat, G4double Reference_grayscale);
    void SetMaterials(G4String Path, G4Material* Reference_mat);

    void SetMaterialsIndexes(size_t* Materials_indexes);
    void SetMaterialsIndexes(G4String Path);

    //Construct
    void Construct();

    /*void SetVoxelsVariables(G4Material* CoreMaterial);
    void SetBallsMaterials(std::vector<std::pair<BallMaterial, G4double>> Materials);*/




    /*G4VPhysicalVolume* Construct();*/

private:
	// methods of materials and volumes separate
	//

    //mother volume
    G4LogicalVolume* fMother;

    //Position
    G4ThreeVector fPosition;

    //Rotation
    G4RotationMatrix* fRotation;

    //Voxels array parameters

    G4double fHalfX;
    G4double fHalfY;
    G4double fHalfZ;
    G4int fVoxelsX_number;
    G4int fVoxelsY_number;
    G4int fVoxelsZ_number;
    G4double fGrayscale_overage;
    G4Material* fReference_mat;
    std::vector<G4Material*> fMaterials;
    size_t* fMaterials_indexes;


    //G4bool  fCheckOverlaps; // option to activate checking of volumes overlaps
}; 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

