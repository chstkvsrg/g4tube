#pragma once

#include "G4VUserPrimaryGeneratorAction.hh"

class G4ParticleGun;


class RoundPrimaryGeneratorAction: public G4VUserPrimaryGeneratorAction
{
public:
	RoundPrimaryGeneratorAction(const std::vector<float>& vecSpectrum);
    ~RoundPrimaryGeneratorAction();

    void GeneratePrimaries(G4Event* anEvent);


private:
    G4ParticleGun* particleGun_;
    std::vector<float> vecSpectrum_;
};




