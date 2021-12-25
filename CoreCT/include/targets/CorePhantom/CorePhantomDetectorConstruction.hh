#pragma once

#include "DetectorConstruction.hh"
#include "G4Material.hh"
#include "defines.hh"

class G4ParticleGun;


class CorePhantomDetectorConstruction: public DetectorConstruction
{
public:
    CorePhantomDetectorConstruction();
    ~CorePhantomDetectorConstruction();

protected:
    virtual void ConstructCore(G4LogicalVolume* Mother, G4RotationMatrix* Rotation, G4ThreeVector Position);
    virtual void ConstructDetector(G4LogicalVolume *Mother, G4RotationMatrix *Rotation, G4ThreeVector Position);

private:
    G4Material* fSi_mat = Mat("G4_Si");
    G4Material* fAir_mat = Mat("G4_AIR");

};




