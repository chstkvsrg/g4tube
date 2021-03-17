// ������������ ����� ��� ������� ������� �� ����������� ����
// ��� ����� ����� � ����������� include/
#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "ActionInitialization.hh"

// ������������ ����� ��� ������� �� ���������� Geant4
//#include "G4RunManager.hh"

#include "G4MTRunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "Randomize.hh"
// ���� � �������� ��� ������ � ��������� ��������
#include <ctime>

int main(int argc, char** argv)
{
  // ����� ���������� ��������� �����
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  // � ������������� ���������� ������� ��������� �������
  // ��������� ����� ���������� ����� ���� �������� ������������� �� ����� ��������� ��������� 
  CLHEP::HepRandom::setTheSeed(time(NULL));


  // Detect interactive mode (if no arguments) and define UI session
  //
  G4UIExecutive* ui = 0;
  if ( argc == 1 ) {
      ui = new G4UIExecutive(argc, argv);
  }

  // �������� ������ ��� ���������� ��������������
  //G4RunManager* runManager = new G4RunManager;
  G4MTRunManager* runManager = new G4MTRunManager;
  runManager->SetNumberOfThreads(6);

  // ����������� ������������ �������: �������� ������, ���������, ��������� � ���������  
  runManager->SetUserInitialization(new DetectorConstruction());
  runManager->SetUserInitialization(new PhysicsList);
  runManager->SetUserInitialization(new ActionInitialization());

  // ����������� �������������� �������: �����/���������� ����������
  //runManager->SetUserAction(new RunAction);

  // �������� � ��������� ������ ��� ���������� �������������
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();
    
  // ������ ����������������� �����
  G4UImanager* UI = G4UImanager::GetUIpointer();
  // � argv[1] ���������� ������ �������� ��������� ������ (��� ����������������� �����)
  //UI->ExecuteMacroFile(argv[1]);

  // Process macro or start UI session
  //
  if ( ! ui ) {
      // batch mode
      G4String command = "/control/execute ";
      G4String fileName = argv[1];
      UI->ApplyCommand(command+fileName);
  }
  else {
      // interactive mode
      UI->ApplyCommand("/control/execute init_vis.mac");
      ui->SessionStart();
      delete ui;
  }

  // ������������ ������
  delete visManager;
  delete runManager;
  // � �����
  return 0;
}
