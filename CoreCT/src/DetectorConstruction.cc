#include <stdio.h>

#include <G4SDManager.hh>
#include <G4SystemOfUnits.hh>
#include <G4UnionSolid.hh>
#include <G4RunManager.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4PVPlacement.hh>
#include <G4RotationMatrix.hh>
#include <G4NistManager.hh>
#include <G4VisAttributes.hh>
#include <G4SubtractionSolid.hh>
#include <G4UnionSolid.hh>
#include <G4CutTubs.hh>
#include <G4PVReplica.hh>
#include <G4ReplicatedSlice.hh>

#include <G4Ellipsoid.hh>

#include "DetectorConstruction.hh"
#include "SensitiveDetector.hh"
#include "TargetsManager.hh"
#include "TargetsCalculator.hh"
#include "MessagesHandler.hh"
#include "Detector.hh"
#include "defines.hh"
#include "CoreHolder.hh"

extern args_info_corect args_info;

DetectorConstruction::DetectorConstruction()
{

}

DetectorConstruction::~DetectorConstruction()
{

}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    // Histogram initlization
    TargetsManager::instance()->initHistogram();

    //Sizes and positions
    //World
    G4double WorldX = 32*cm*2;
    G4double WorldY = 32*cm*2;
    G4double WorldZ = 32*cm*2;

    //Detector
    G4ThreeVector Det_pose = G4ThreeVector(0.,0.,-30.*cm);

    //Core+Holder

    G4RotationMatrix* Core_rotation = new G4RotationMatrix();
    Core_rotation->rotateX(M_PI/2);
    Core_rotation->rotateY(args_info.angle_arg/180*M_PI);

    //
    //CONSTRUCTION PHASE
    //

    //World

    G4Box* World_solid = new G4Box("World", WorldX/2, WorldY/2, WorldZ/2);

    G4LogicalVolume* World_logic = new G4LogicalVolume(World_solid,
                                                       Mat("G4_Galactic"),
                                                       "World");

    G4VPhysicalVolume* World_phys = new G4PVPlacement(0,
                                                      G4ThreeVector(),
                                                      World_logic,
                                                      "World",
                                                      0,
                                                      false,
                                                      0,
                                                      true);

    //Core

    //ConstructCore(World_logic, Core_rotation, G4ThreeVector());

    //CoreHolder

    if (args_info.holder_flag)
    {
        if(args_info.target_arg == target_arg_CoreVoxel)
        {
            std::cout << "Watch here!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
            std::cout << "You turn on Holder(-h) And Voxel(target), now it's impossible" << std::endl;
            std::cout << "Be more attentive pls" << std::endl;
            exit(2);
        }

        TargetsCalculator* calc = TargetsCalculator::instance();

        CoreHolder* Holder =
                new CoreHolder(World_logic,       //MotherLogic,
                               G4ThreeVector(),   //position in mother
                               Core_rotation,                 //rotation in mother
                               calc->GetCoreHeightHalf()*2.,              //Holder_length,
                               calc->GetCoreRadius()*2.,              //Core_diameter,
                               1*cm,              //Rubber_thikness,
                               0.5*cm,            //AirGap_thikness,
                               0.5*cm,            //Coat_thikness,
                               6*cm,              //Road_dist,
                               0.8*cm);           //Road_diameter

        Holder->Construct();
    }

    //Detector

    ConstructDetector(World_logic, 0, Det_pose);

    G4VisAttributes* World_vis = new G4VisAttributes(G4Color(0.,0.,0.,0.));

    World_logic->SetVisAttributes(World_vis);

    return World_phys;
}

void DetectorConstruction::ConstructCore(G4LogicalVolume *Mother, G4RotationMatrix *Rotation, G4ThreeVector Position)
{
    G4Tubs* Core_solid  = new G4Tubs("Core",
                                     0, 1.5*cm,
                                     5*cm,
                                     0, 2*M_PI);
    G4LogicalVolume* Core_logic = new G4LogicalVolume(Core_solid,
                                                      Mat("G4_Galactic"),
                                                      "Core");
    new G4PVPlacement(G4Transform3D(*Rotation,
                                    Position),
                      Core_logic,
                      "Core",
                      Mother,
                      false,
                      0,
                      true);
}

void DetectorConstruction::ConstructDetector(G4LogicalVolume *Mother, G4RotationMatrix *Rotation, G4ThreeVector Position)
{
    G4Box* Detector_solid  = new G4Box("Detector", 1*mm, 1*mm, 1*mm);

    G4LogicalVolume* Detector_logic = new G4LogicalVolume(Detector_solid,
                                                          Mat("G4_Galactic"),
                                                          "Detector");

    new G4PVPlacement(G4Transform3D(*Rotation,
                                    Position),
                      Detector_logic,
                      "Detector",
                      Mother,
                      false,
                      0,
                      true);
}

void DetectorConstruction::ConstructSDandField()
{
    G4String DetectorSD_name = "DetectorSD";

    SensitiveDetector* DetectorSD = new SensitiveDetector(DetectorSD_name);

    G4SDManager::GetSDMpointer()->AddNewDetector(DetectorSD);
    SetSensitiveDetector("DetectorCell" , DetectorSD);
}
