// заголовочные файлы для классов которые мы реализовали сами
// эти файлы лежат в подкаталоге include/
#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "ActionInitialization.hh"

// заголовочные файлы для классов из библиотеки Geant4
//#include "G4RunManager.hh"

#include "G4MTRunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "Randomize.hh"
// файл с фунциями для работы с системным временем
#include <ctime>

int main(int argc, char** argv)
{
  // выбор генератора случайных чисел
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  // и инициализация генератора текущим значением времени
  // благодаря этому результаты любых двух запусков моделирования не будут полностью одинаковы 
  CLHEP::HepRandom::setTheSeed(time(NULL));


  // Detect interactive mode (if no arguments) and define UI session
  //
  G4UIExecutive* ui = 0;
  if ( argc == 1 ) {
      ui = new G4UIExecutive(argc, argv);
  }

  // создание класса для управления моделированием
  //G4RunManager* runManager = new G4RunManager;
  G4MTRunManager* runManager = new G4MTRunManager;
  runManager->SetNumberOfThreads(24);

  // подключение обязательных классов: описание частиц, процессов, геометрии и источника  
  runManager->SetUserInitialization(new DetectorConstruction());
  runManager->SetUserInitialization(new PhysicsList);
  runManager->SetUserInitialization(new ActionInitialization());

  // подключение дополнительных классов: набор/сохранение гистограмм
  //runManager->SetUserAction(new RunAction);

  // создание и настройка класса для управления визуализацией
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();
    
  // запуск конфигурационного файла
  G4UImanager* UI = G4UImanager::GetUIpointer();
  // в argv[1] содержится первый параметр командной строки (имя конфигурационного файла)
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

  // освобождение памяти
  delete visManager;
  delete runManager;
  // и выход
  return 0;
}
