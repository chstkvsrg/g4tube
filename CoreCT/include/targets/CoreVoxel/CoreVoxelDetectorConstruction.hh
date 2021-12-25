#pragma once

#include "DetectorConstruction.hh"

class G4ParticleGun;


class CoreVoxelDetectorConstruction: public DetectorConstruction
{
public:
    CoreVoxelDetectorConstruction();
    ~CoreVoxelDetectorConstruction();

protected:
    virtual void ConstructCore(G4LogicalVolume* Mother, G4RotationMatrix* Rotation, G4ThreeVector Position);
    virtual void ConstructDetector(G4LogicalVolume* Mother, G4RotationMatrix* Rotation, G4ThreeVector Position);
    //void ConstructSDandField();

private:




};




