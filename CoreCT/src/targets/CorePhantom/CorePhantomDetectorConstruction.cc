#include <G4SDManager.hh>
#include <G4SystemOfUnits.hh>

#include "CorePhantomDetectorConstruction.hh"
#include "CorePhantomPhys.hh"
#include "SensitiveDetector.hh"
#include "Detector.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4Tubs.hh"
#include "TargetsManager.hh"
#include "TargetsCalculator.hh"

#include <cmath>


CorePhantomDetectorConstruction::CorePhantomDetectorConstruction()
{

}

CorePhantomDetectorConstruction::~CorePhantomDetectorConstruction()
{
}

void CorePhantomDetectorConstruction::ConstructCore(G4LogicalVolume *Mother, G4RotationMatrix *Rotation, G4ThreeVector Position)
{
    CorePhantomPhys* Core = new CorePhantomPhys(Mother, Rotation, Position);

    Core->SetCore(TargetsCalculator::instance()->GetCoreRadius(), TargetsCalculator::instance()->GetCoreHeightHalf()*2, fSi_mat);
    /*Core->SetZeroBall(50*micrometer, G4ThreeVector(0.75*cm, 0, -1.875*cm), fSi_mat);

    std::vector<G4double> Balls_radius = {50*micrometer, 100*micrometer,
                                          200*micrometer, 300*micrometer, 500*micrometer};

    Core->SetSegments(Balls_radius);

    std::vector<G4double> BallsCoof_material = {0.99, 0.98, 0.95};

    Core->SetLayers(BallsCoof_material, 1.25*cm);
    Core->AppendLayer(fAir_mat);
    std::vector<G4double> Balls_grayscale = {0.505, 0.51, 0.525,1};

    Core->SetGrayScale(Balls_grayscale);*/

    /*Core->SetZeroBall(50*micrometer, G4ThreeVector(0.75*cm, 0, 0*cm), fSi_mat);

    std::vector<G4double> Balls_radius = {50*micrometer, 100*micrometer,
                                          200*micrometer, 300*micrometer, 500*micrometer};

    Core->SetSegments(Balls_radius);

    std::vector<G4double> BallsCoof_material = {0.99};

    Core->SetLayers(BallsCoof_material, 0*cm);
    //Core->AppendLayer(fAir_mat);
    std::vector<G4double> Balls_grayscale = {0.505};

    Core->SetGrayScale(Balls_grayscale);*/

    Core->SetZeroBall(50*micrometer, G4ThreeVector(0.75*cm, 0, 0*cm), fSi_mat);

    std::vector<G4double> Balls_radius = /*{50*micrometer, 100*micrometer,
                                          200*micrometer, 300*micrometer, 500*micrometer};*/
    {50*micrometer, 100*micrometer,
            200*micrometer, 300*micrometer, 500*micrometer};

    Core->SetSegments(Balls_radius);

    std::vector<G4double> BallsCoof_material = {0.95};

    Core->SetLayers(BallsCoof_material, 0*cm);
    //Core->AppendLayer(fAir_mat);
    std::vector<G4double> Balls_grayscale = {1};

    Core->SetGrayScale(Balls_grayscale);

    Core->Construct();

}

void CorePhantomDetectorConstruction::ConstructDetector(G4LogicalVolume *Mother, G4RotationMatrix *Rotation, G4ThreeVector Position)
{
    //the sizes in calculator
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





