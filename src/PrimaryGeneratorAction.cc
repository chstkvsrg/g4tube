#include "PrimaryGeneratorAction.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4RandomDirection.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
  // создаем источник частиц
  // источник испускает по одной частице
  particleGun = new G4ParticleGun(1);

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle = particleTable->FindParticle("e-");
  // устанавливаем тип и энергию частиц, координаты положения источника
  if (particle->GetParticleName() == "e-" ) {
  particleGun->SetParticleDefinition(particle);
  particleGun->SetParticleEnergy(160*keV);
  particleGun->SetParticlePosition(G4ThreeVector(0, 0, 0));
  }
}


PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete particleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  particleGun->SetParticleMomentumDirection( G4ThreeVector(0,0,1) );
  // источник испускает одну частицу
  particleGun->GeneratePrimaryVertex(event);
}

