#include "RunAction.hh"

#include "G4RunManager.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "G4Run.hh"
#include "Randomize.hh"
#include "Analysis.hh"

RunAction::RunAction() {

    auto analysisManager = G4AnalysisManager::Instance();
    G4cout << "Using " << analysisManager->GetType() << G4endl;
    analysisManager->SetVerboseLevel(0);
    analysisManager->CreateH1("Energy","Kinetic energy", 1000, 0., 225*keV,"keV");
}

RunAction::~RunAction() {
    delete G4AnalysisManager::Instance();
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

