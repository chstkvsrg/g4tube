//Prticles generator include

#ifndef CorePhantomPrimaryGeneratorAction_h
#define CorePhantomPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"

class G4ParticleGun;
class G4Event;

/// The primary generator action class with particle gum.
///

class CorePhantomPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  CorePhantomPrimaryGeneratorAction();
  virtual ~CorePhantomPrimaryGeneratorAction();

  virtual void GeneratePrimaries(G4Event* event);

  // set methods


private:
  G4ParticleGun*  fParticleGun; // G4 particle gun

  std::vector<std::pair<G4double, G4double>> fSpectrum = {};

  void SetSpectrum(G4String Path);
  void SetSpectrum(const std::vector<std::pair<G4double, G4double>>& Spectrum);

  G4double ComputeEnergy();
  G4ThreeVector ComputeDirection();

  G4double fDeltaphi = 0.;
  G4double fDeltatheta = 0.;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif



