#include "EKillerSensitiveDetector.hh"

#include "G4RunManager.hh"
#include "G4Step.hh"
#include "G4SystemOfUnits.hh"

#include "HistManager.hh"

EKillerSensitiveDetector::EKillerSensitiveDetector(G4String name): G4VSensitiveDetector(name)
{

}

EKillerSensitiveDetector::~EKillerSensitiveDetector()
{
}


G4bool EKillerSensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory*)
{
	G4Track* track = step -> GetTrack();
	G4String particleName = track->GetDefinition()->GetParticleName();

	if (particleName == "e-")
		track->SetTrackStatus(fStopAndKill);

	return true;
}




