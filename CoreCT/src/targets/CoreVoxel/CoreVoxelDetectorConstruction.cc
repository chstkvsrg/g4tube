#include <G4SDManager.hh>
#include <G4SystemOfUnits.hh>

#include "CoreVoxelDetectorConstruction.hh"
#include "CoreVoxel.hh"
#include "Detector.hh"
#include "G4Box.hh"
#include "TargetsManager.hh"
#include "TargetsCalculator.hh"
#include "G4LogicalVolumeStore.hh"
//#include "CoreVoxelSensitiveDetector.hh"

#include "defines.hh"



CoreVoxelDetectorConstruction::CoreVoxelDetectorConstruction()
{
}

CoreVoxelDetectorConstruction::~CoreVoxelDetectorConstruction()
{
}

void CoreVoxelDetectorConstruction::ConstructCore(G4LogicalVolume *Mother, G4RotationMatrix *Rotation, G4ThreeVector Position)
{
    CoreVoxel* vox  = new CoreVoxel(Mother, Position, Rotation);

    vox->SetVoxelsVariables("/home/xinady/data/Geant4/core/geom/CoreVoxel/Voxels.ml2g4");
    vox->SetMaterials("/home/xinady/data/Geant4/core/geom/CoreVoxel/Densities.ml2g4", Mat("G4_CONCRETE"));
    vox->SetMaterialsIndexes("/home/xinady/data/Geant4/core/geom/CoreVoxel/Indexes.ml2g4");
    vox->Construct();
}

void CoreVoxelDetectorConstruction::ConstructDetector(G4LogicalVolume *Mother, G4RotationMatrix *Rotation, G4ThreeVector Position)
{

    TargetsCalculator* calc = TargetsCalculator::instance();
   //Distances

   Detector* Det =
           new Detector(Mother,                       //MotherLogic,
                        Position,       //Position,
                        Rotation,                                 //Rotation,
                        1*cm,                              //Detector_length,
                        calc->GetDetXSize(),                               //DetectorCell_sizeX,
                        calc->GetDetYSize(),                               //DetectorCell_sizeY,
                        calc->GetDetXNumber(),                                  //DetectorCell_numberX,
                        calc->GetDetYNumber()                                  //DetectorCell_numberY
                        );

    Det->Construct();
}

/*void CoreVoxelDetectorConstruction::ConstructSDandField()
{
	//return;

	G4SDManager* manager = G4SDManager::GetSDMpointer();

    CorePhantomSensitiveDetector* sd = new CorePhantomSensitiveDetector("Sensitive");
	SetSensitiveDetector("LogicMonitor1", sd);

	manager->AddNewDetector(sd);
}*/





