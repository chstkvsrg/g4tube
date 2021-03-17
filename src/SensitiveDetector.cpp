
#include "SensitiveDetector.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include <iostream>
#include <fstream>
#include "G4UnitsTable.hh"
#include "G4RunManager.hh"
#include "Analysis.hh"

SensitiveDetector::SensitiveDetector(const G4String& name)
: G4VSensitiveDetector(name)
{
}

SensitiveDetector::~SensitiveDetector()
{}

void SensitiveDetector::Initialize(G4HCofThisEvent*)
{
}

G4bool SensitiveDetector::ProcessHits(G4Step* step,
        G4TouchableHistory*)
{
    // energy deposit
    G4double edep = step->GetTrack()->GetKineticEnergy();
    if (edep==0.) return false;

    G4String particle = step->GetTrack()->GetDefinition()->GetParticleName();
    if (particle != "gamma")
    {
        step->GetTrack()->SetTrackStatus(fStopAndKill);
        return false;
    }

    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->FillH1(0, edep);

    step->GetTrack()->SetTrackStatus(fStopAndKill);
    return true;

}

void SensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
    /*if ( verboseLevel>1 ) {
        G4int nofHits = fHitsCollection->entries();
        G4cout << G4endl
                << "-------->Hits Collection: in this event they are " << nofHits
                << " hits : " << G4endl;
        std::ofstream out("/home/user/PET/geant4/first_ver/first_ver_with_sd/data.txt", std::ios::app);
        if (out.is_open())
        {
            for ( G4int i=0; i<nofHits; i++ ) {G4double Edep = (*fHitsCollection)[i]->GetEdep();
            out << G4BestUnit(Edep,"Energy") << std::endl;}
            out << "data finished" << std::endl;
            out.close();};
    }*/
}

