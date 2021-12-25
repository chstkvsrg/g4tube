#include <G4Event.hh>
#include <G4ParticleGun.hh>
#include <G4ParticleTable.hh>
#include <G4ParticleDefinition.hh>
#include <G4RandomDirection.hh>
#include <G4SystemOfUnits.hh>
#include "G4Tubs.hh"
#include "TargetsManager.hh"
#include "TargetsCalculator.hh"

#include "Randomize.hh"
#include <iostream>
#include <fstream>


#include "CorePhantomPrimaryGeneratorAction.hh"



CorePhantomPrimaryGeneratorAction::CorePhantomPrimaryGeneratorAction():
    G4VUserPrimaryGeneratorAction(),
    fParticleGun(nullptr)
{
    fParticleGun = new G4ParticleGun(1);
    fParticleGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle("gamma"));
    fParticleGun->SetParticlePosition(G4ThreeVector(0.,0.,TargetsCalculator::instance()->GetSourceTargetDist()));
    //SetSpectrum("/home/xinady/data/Geant4/core/sources/W160Spectrum.txt");
    SetSpectrum("W160Spectrum.txt");

    TargetsCalculator* calc = TargetsCalculator::instance();
    fDeltaphi = calc->GetSourcePhiAngle();
    fDeltatheta = calc->GetSourceThetaAngle();
}

CorePhantomPrimaryGeneratorAction::~CorePhantomPrimaryGeneratorAction()
{
    delete fParticleGun;
}

void CorePhantomPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{

    fParticleGun->SetParticleEnergy(ComputeEnergy());

    fParticleGun->SetParticleMomentumDirection(ComputeDirection());

    fParticleGun->GeneratePrimaryVertex(event);
}

void CorePhantomPrimaryGeneratorAction::SetSpectrum(const std::vector<std::pair<G4double, G4double> > &Spectrum)
{
    fSpectrum = Spectrum;
}

void CorePhantomPrimaryGeneratorAction::SetSpectrum(G4String Path)
{
    std::ifstream in(Path);

    if (!in.is_open())
    {
        std::cout << "Watch here!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        std::cout << "Can't open spectrum txt file!" << std::endl;
        exit(4);
    }

    G4int Bins_number = 0;
    in >> Bins_number;
    std::pair<G4double, G4double> Bin (0., 0.); //bin include upper_bound and not include lower bound

    fSpectrum.push_back(Bin);

    for (G4int Index = 0; Index < Bins_number; Index++)
    {
        in >> Bin.first >> Bin.second;
        fSpectrum.push_back(Bin);
    }
    in.close();
}

G4double CorePhantomPrimaryGeneratorAction::ComputeEnergy()
{
    G4double Random = static_cast <G4double>(std::rand()) / static_cast <G4double> (RAND_MAX);

    G4double Sum = 0;
    unsigned bin = 0;
    for (bin = 1; bin < fSpectrum.size(); bin++)
    {
        Sum += fSpectrum[bin].second;
        if (Sum > Random) break;
    }

    G4double Upper_bound = fSpectrum[bin].first;
    G4double Lower_bound = fSpectrum[bin-1].first;

    return (Lower_bound + static_cast <G4double> (rand()) / ( static_cast <G4double> (RAND_MAX/(Upper_bound-Lower_bound))))*MeV;
}

G4ThreeVector CorePhantomPrimaryGeneratorAction::ComputeDirection()
{
    G4double SourcePhiAngle = fDeltaphi*(-1 + 2.*(static_cast <G4double>(std::rand()) / static_cast <G4double> (RAND_MAX)));
    G4double SourceThetaAngle = fDeltatheta*(-1 + 2.*(static_cast <G4double>(std::rand()) / static_cast <G4double> (RAND_MAX)));

    G4double SourcePhiTanAngle = std::tan(SourcePhiAngle);
    G4double SourceThetaTanAngle = std::tan(SourceThetaAngle);

    return G4ThreeVector(SourcePhiTanAngle, SourceThetaTanAngle, -1.);
}




