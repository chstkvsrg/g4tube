#pragma once

#include <G4VUserDetectorConstruction.hh>
#include <G4ThreeVector.hh>
#include <G4RotationMatrix.hh>
#include <G4SystemOfUnits.hh>

#include <vector>

class SensitiveDetector;
class G4VSolid;

class G4Material;



class DetectorConstruction : public G4VUserDetectorConstruction
{                 
public:
    DetectorConstruction();
    ~DetectorConstruction();

    G4VPhysicalVolume* Construct();
    virtual void ConstructSDandField();

protected:

    //virtual function for choose

    virtual void ConstructCore(G4LogicalVolume* Mother, G4RotationMatrix* Rotation, G4ThreeVector Position);
    virtual void ConstructDetector(G4LogicalVolume* Mother, G4RotationMatrix* Rotation, G4ThreeVector Position);
};



