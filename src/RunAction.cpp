#include "RunAction.hh"

#include "G4RunManager.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "G4Run.hh"
#include "Randomize.hh"
#include "Analysis.hh"
#include "DetectorConstruction.hh"

#include <cstdio>

RunAction::RunAction() {

    auto analysisManager = G4AnalysisManager::Instance();
    G4cout << "Using " << analysisManager->GetType() << G4endl;
    analysisManager->SetVerboseLevel(0);

    // число гистограмм = числу полос детектора (по умолчанию 6)
    G4int nStrips = 6;
    auto* runManager = G4RunManager::GetRunManager();
    auto* det = dynamic_cast<const DetectorConstruction*>(runManager->GetUserDetectorConstruction());
    if ( det && !det->GetDetectors().empty() )
        nStrips = det->GetDetectors().front().nStrips;

    char name[32];
    for (G4int i = 0; i < nStrips; ++i) {
        std::snprintf(name, sizeof(name), "Energy%d", i);
        analysisManager->CreateH1(name, "Kinetic energy", 1000, 0., 225*keV, "keV");
    }
}

RunAction::~RunAction() {
}

void RunAction::BeginOfRunAction(const G4Run*)
{  
  stampBegin_ = QDateTime::currentDateTime().toString("yyyyMMdd-hh:mm:ss");
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);
}

void RunAction::EndOfRunAction(const G4Run* )
{
  // сохраняем гистограмму в файл
  // второй параметр - первая строка файла
  stampFinish_ = QDateTime::currentDateTime().toString("yyyyMMdd-hh:mm:ss");

  QString name = QString("h1_%1_%2.csv").arg(stampBegin_).arg(stampFinish_);

  auto analysisManager = G4AnalysisManager::Instance();
  G4String fileName = name.toStdString();
  if (analysisManager->OpenFile(fileName)) {
      analysisManager->Write();
      analysisManager->CloseFile();
  }
}

