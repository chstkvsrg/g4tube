//Detector construction source

#include "CoreVoxel.hh"
#include "CoreVoxelParameterisation.hh"

#include "G4NistManager.hh"
#include "G4SDManager.hh"

#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4AnyType.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4SubtractionSolid.hh"
#include "G4PVReplica.hh"
#include "G4ReflectionFactory.hh"
#include "G4PhantomParameterisation.hh"
#include "G4PVParameterised.hh"
#include "G4VVisManager.hh"

#include "G4IntersectionSolid.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4VisModelManager.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalVolumeModel.hh"

#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"
#include "G4LogicalVolume.hh"

#include "G4LogicalBorderSurface.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4VNestedParameterisation.hh"

#include <cmath>
#include <vector>
#include "Randomize.hh"
#include <iostream>
#include <fstream>
#include <G4ios.hh>


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CoreVoxel::CoreVoxel(G4LogicalVolume* MotherLogic,
                         G4ThreeVector Position,
                         G4RotationMatrix* Rotation
                         )
    :
      fMother(MotherLogic),
      fPosition(Position), fRotation(Rotation),
      fHalfX(0.), fHalfY(0.), fHalfZ(0.),
      fVoxelsX_number(-1), fVoxelsY_number(-1), fVoxelsZ_number(-1),
      fGrayscale_overage(0.), fReference_mat(nullptr), fMaterials(), fMaterials_indexes(nullptr)
{
    if(fRotation==nullptr ||fRotation==0||fRotation== NULL)
    {
        fRotation = new G4RotationMatrix;
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CoreVoxel::CoreVoxel(G4LogicalVolume* MotherLogic,
                         G4ThreeVector Position,
                         G4RotationMatrix* Rotation,
                         G4double HalfX, G4double HalfY, G4double HalfZ,
                         G4int VoxelsX_number, G4int VoxelsY_number, G4int VoxelsZ_number,
                         G4double Grayscale_overage, G4Material *Reference_mat, std::vector<G4Material*> Materials, size_t *Materials_indexes):
    fMother(MotherLogic),
    fPosition(Position), fRotation(Rotation),
    fHalfX(HalfX), fHalfY(HalfY), fHalfZ(HalfZ),
    fVoxelsX_number(VoxelsX_number), fVoxelsY_number(VoxelsY_number), fVoxelsZ_number(VoxelsZ_number),
    fGrayscale_overage(Grayscale_overage), fReference_mat(Reference_mat), fMaterials(Materials),fMaterials_indexes(Materials_indexes)
{
    if(fRotation==nullptr ||fRotation==0||fRotation== NULL)
    {
        fRotation = new G4RotationMatrix;
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


CoreVoxel::~CoreVoxel()
{ 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void CoreVoxel::SetVoxelsVariables(G4double HalfX, G4double HalfY, G4double HalfZ,
                                     G4int VoxelsX_number, G4int VoxelsY_number, G4int VoxelsZ_number)
{
    fHalfX = HalfX;
    fHalfY = HalfY;
    fHalfZ = HalfZ;
    fVoxelsX_number = VoxelsX_number;
    fVoxelsY_number = VoxelsY_number;
    fVoxelsZ_number = VoxelsZ_number;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CoreVoxel::SetVoxelsVariables(G4String Path)
{
    std::ifstream invox(Path, std::ios::binary|std::ios::in);
    if(!invox)
    {
        std::cout << "Watch here!!!!!!!!!!!!!!" << std::endl;
        std::cout << "Vox variables error" << std::endl;
        std::cout << "Cannot open file!" << std::endl;
        exit(1);
    }
    invox.read((char*) &fHalfX, sizeof(fHalfX));
    invox.read((char*) &fHalfY, sizeof(fHalfY));
    invox.read((char*) &fHalfZ, sizeof(fHalfZ));
    invox.read((char*) &fVoxelsX_number, sizeof(fVoxelsX_number));
    invox.read((char*) &fVoxelsY_number, sizeof(fVoxelsY_number));
    invox.read((char*) &fVoxelsZ_number, sizeof(fVoxelsZ_number));
    invox.close();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CoreVoxel::SetMaterials(std::vector<G4Material*> Materials, G4Material *Reference_mat, G4double Reference_grayscale)
{
    fReference_mat = Reference_mat;
    fMaterials = Materials;
    fGrayscale_overage = Reference_grayscale;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CoreVoxel::SetMaterials(G4String Path, G4Material* Reference_mat)
{
    fReference_mat = Reference_mat;

    G4int Materials_quantity = 0;

    std::ifstream inmat(Path, std::ios::binary|std::ios::in);
    if(!inmat)
    {
        std::cout << "Watch here!!!!!!!!!!!!!!" << std::endl;
        std::cout << "Vox materials error" << std::endl;
        std::cout << "Cannot open file!" << std::endl;
        exit(1);
    }

    inmat.read((char*) &fGrayscale_overage, sizeof(fGrayscale_overage));

    inmat.read((char*) &Materials_quantity, sizeof(Materials_quantity));

    for (G4int Index=0; Index < Materials_quantity; Index++)
    {
        G4double Material_grayscale = 0;
        G4String Material_name = Reference_mat->GetName() + "_" + std::to_string(Index);

        inmat.read((char*) &Material_grayscale, sizeof(Material_grayscale));

        G4double Material_density = (Reference_mat->GetDensity()*fGrayscale_overage/Material_grayscale);

        G4Material* Material = new G4Material(Material_name, Material_density, Reference_mat);
        fMaterials.push_back(Material);
    }

    inmat.close();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CoreVoxel::SetMaterialsIndexes(size_t* Materials_indexes)
{
    fMaterials_indexes = Materials_indexes;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CoreVoxel::SetMaterialsIndexes(G4String Path)
{
    fMaterials_indexes = new size_t[fVoxelsX_number*fVoxelsY_number*fVoxelsZ_number]{};

    std::ifstream inind(Path, std::ios::binary|std::ios::in);
    if(!inind)
    {
        std::cout << "Watch here!!!!!!!!!!!!!!" << std::endl;
        std::cout << "Vox materials indexes error" << std::endl;
        std::cout << "Cannot open file!" << std::endl;
        exit(1);
    }

    for (G4int Index=0; Index < fVoxelsX_number*fVoxelsY_number*fVoxelsZ_number; Index++)
    {
        G4int Material_index = 0;
        inind.read((char*) &Material_index, sizeof(Material_index));
        fMaterials_indexes[Index] = (size_t) Material_index;
    }
    inind.close();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void CoreVoxel::Construct()
{
    CoreVoxelParameterisation* Paramet = new CoreVoxelParameterisation(fReference_mat->GetDensity(), fGrayscale_overage);

    Paramet->SetVoxelDimensions(fHalfX*mm, fHalfY*mm, fHalfZ*mm);
    Paramet->SetNoVoxel(fVoxelsX_number, fVoxelsY_number, fVoxelsZ_number);
    Paramet->SetMaterials(fMaterials);
    Paramet->SetMaterialIndices(fMaterials_indexes);
    Paramet->SetSkipEqualMaterials(true);

    G4Box* Container_solid = new G4Box("VoxelsContainer", fVoxelsX_number*fHalfX, fVoxelsY_number*fHalfY, fVoxelsZ_number*fHalfZ);

    G4LogicalVolume* Container_logic =
            new G4LogicalVolume(Container_solid,
                                G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),
                                "VoxelsContainer");

    G4VPhysicalVolume* Container_phys =
            new G4PVPlacement(G4Transform3D(*fRotation,                // no rotation
                                            fPosition),                     // position
                              Container_logic,
                              "VoxelsContainer",
                              fMother,
                              false,
                              0);

    G4VisAttributes* Container_vis = new G4VisAttributes(G4Color(0,0,0,0));
    Container_logic->SetVisAttributes(Container_vis);

    Paramet->BuildContainerSolid(Container_phys);
    Paramet->CheckVoxelsFillContainer(Container_solid->GetXHalfLength(), Container_solid->GetYHalfLength(), Container_solid->GetZHalfLength());


    G4Box* Voxel_solid = new G4Box("Voxel", fHalfX, fHalfY, fHalfZ);

    G4LogicalVolume* Voxel_logic =
            new G4LogicalVolume(Voxel_solid,
                                G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),
                                "Voxel");

    G4PVParameterised* Voxel_phys =
            new G4PVParameterised("Voxell",
                                  Voxel_logic,
                                  // logical volume
                                  Container_logic,
                                  // mother volume
                                  kUndefined,
                                  // optimisation hint
                                  fVoxelsX_number
                                  *
                                  fVoxelsY_number
                                  *
                                  fVoxelsZ_number,
                                  // number of voxels
                                  Paramet);

    Voxel_phys->SetRegularStructureId(1);

}

