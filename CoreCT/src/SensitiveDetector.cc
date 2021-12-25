#include "SensitiveDetector.hh"

#include "G4RunManager.hh"
#include "G4Step.hh"
#include "G4SystemOfUnits.hh"
#include "TargetsCalculator.hh"

#include "HistManager.hh"

SensitiveDetector::SensitiveDetector(G4String name): G4VSensitiveDetector(name)
{
	histMan_ = HistManager::instance();
	histMan_->addContext("my", this);
}

SensitiveDetector::~SensitiveDetector()
{
}


void SensitiveDetector::Initialize(G4HCofThisEvent*)
{
}

G4bool SensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory*)
{
	const G4TouchableHandle touchable = step->GetPreStepPoint()->GetTouchableHandle();
	G4Track* track = step->GetTrack();

	G4String particleName = track->GetDefinition()->GetParticleName();

	if (particleName == "gamma")
	{
		if (track->GetDynamicParticle()->GetTotalEnergy() > 10*keV)
		{
			std::vector<int> index(1);
            index[0] = touchable->GetCopyNumber(1);

            /*std::cout << "SD HERE!!!!!!!!" << std::endl;
            std::cout << "X - "<< touchable->GetCopyNumber(0) << "; Y - " << touchable->GetCopyNumber(1) << std::endl;*/

            histMan_->addValue("my", this, index, touchable->GetCopyNumber(0));
		}
	}

	track->SetTrackStatus(fStopAndKill);

	return true;
}

void SensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{

}



