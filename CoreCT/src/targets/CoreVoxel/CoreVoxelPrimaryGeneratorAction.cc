#include <G4Event.hh>
#include <G4ParticleGun.hh>
#include <G4ParticleTable.hh>
#include <G4ParticleDefinition.hh>
#include <G4RandomDirection.hh>
#include <G4SystemOfUnits.hh>
#include "G4LogicalVolumeStore.hh"
#include "G4Box.hh"
#include "TargetsManager.hh"

#include "Randomize.hh"
#include <iostream>
#include <fstream>


#include "CoreVoxelPrimaryGeneratorAction.hh"



CoreVoxelPrimaryGeneratorAction::CoreVoxelPrimaryGeneratorAction():
    G4VUserPrimaryGeneratorAction(),
    fParticleGun(nullptr)
{
    fParticleGun = new G4ParticleGun(1);
    fParticleGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle("gamma"));
    fParticleGun->SetParticlePosition(G4ThreeVector(0.,0.,30*cm));
    SetSpectrum("INPUT/Spectrum.txt");
}

CoreVoxelPrimaryGeneratorAction::~CoreVoxelPrimaryGeneratorAction()
{
    delete fParticleGun;
}

void CoreVoxelPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{

    fParticleGun->SetParticleEnergy(ComputeEnergy());

    fParticleGun->SetParticleMomentumDirection(ComputeDirection());

    fParticleGun->GeneratePrimaryVertex(event);
}

void CoreVoxelPrimaryGeneratorAction::SetSpectrum(const std::vector<std::pair<G4double, G4double> > &Spectrum)
{
    fSpectrum = Spectrum;
}

void CoreVoxelPrimaryGeneratorAction::SetSpectrum(G4String Path)
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

G4double CoreVoxelPrimaryGeneratorAction::ComputeEnergy()
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

G4ThreeVector CoreVoxelPrimaryGeneratorAction::ComputeDirection()
{
    G4LogicalVolumeStore* store = G4LogicalVolumeStore::GetInstance();

    G4Box* Core_solid = static_cast<G4Box*>(store->GetVolume("VoxelsContainer")->GetSolid());

    if(Core_solid == 0)
    {
        std::cout << "Watch here!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        std::cout << "Check CoreVoxelDetectorConstruction" << " Core is not container :(" << std:: endl;
        exit(3);
    }

    G4double Core_radius = ((Core_solid->GetXHalfLength() >= Core_solid->GetYHalfLength()) ? Core_solid->GetXHalfLength() : Core_solid->GetYHalfLength());

    G4double Core_heighthalf = Core_solid->GetZHalfLength();

    //Distances

    G4double SourceCore_dist = TargetsManager::instance()->GetSourceTargetDist();

    G4double SourcePhiTanAngle = Core_radius/std::sqrt(SourceCore_dist*SourceCore_dist-Core_radius*Core_radius)*(-1 + 2.*(static_cast <G4double>(std::rand()) / static_cast <G4double> (RAND_MAX)));
    G4double SourceThetaTanAngle = Core_heighthalf/(SourceCore_dist-Core_radius)*(-1 + 2.*(static_cast <G4double>(std::rand()) / static_cast <G4double> (RAND_MAX)));

    return G4ThreeVector(SourcePhiTanAngle, SourceThetaTanAngle, -1.);
}




