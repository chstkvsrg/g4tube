#ifndef DetectorConstruction_H
#define DetectorConstruction_H 1

#include "G4VUserDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"

class DetectorConstruction : public G4VUserDetectorConstruction
{                 
  public:
    DetectorConstruction();
    ~DetectorConstruction();

    G4VPhysicalVolume* Construct();

    virtual void ConstructSDandField();

    struct AnodeParams {
        G4double size  = 10*mm;  // ширина анода
        G4double thick = 5*mm;  // толщина анода
        //G4double angle = 11*deg; // угол наклона анода
        G4double angle = 11*deg; // угол наклона анода
        G4ThreeVector pos = G4ThreeVector(0, (thick/2)*sin(angle), 20*mm + thick/2); // позиция относительно парента
    };

    struct WindowParams {
        G4double size  = 20*mm;  // радиус окна
        G4double thick = 0.8*mm; // толщина окна
        G4double angle = 90*deg; // угол поворота
        G4ThreeVector pos = G4ThreeVector(0, -7*mm, 20*mm); // позиция относительно парента
    };

    struct FilterParams {
        G4double size  = 35*mm;  // радиус окна
        G4double thick = 10.0*mm; // толщина окна
        G4double angle = 90*deg; // угол поворота
        G4ThreeVector pos = G4ThreeVector(0, -17.8*mm, 20*mm); // позиция относительно парента
    };

    struct DetectorParams {
        G4double size  = 35*mm;  // радиус
        G4double thick = 0.8*mm; // толщина
        G4double angle = 90*deg; // угол поворота
        //G4ThreeVector pos = G4ThreeVector(0, -12.2*mm, 20*mm); // позиция относительно парента 2 mm alum
        //G4ThreeVector pos = G4ThreeVector(0, -8.2*mm, 20*mm); // позиция относительно парента  0 mm alum
        G4ThreeVector pos = G4ThreeVector(0, -28.2*mm, 20*mm); // позиция относительно парента
    };

    struct DetectorParams2 {
        G4String name = "detector2";
        G4double height  = 70*mm;  // высота
        G4double width  = 3*mm;  // ширина
        G4double thick = 0.8*mm; // толщина
        G4double angle = 90*deg; // угол поворота       
        G4ThreeVector pos = G4ThreeVector(0, -28.2*mm, 20*mm + tan(10*deg)*28.2 - width/2); // позиция относительно парента
    };

    G4VPhysicalVolume*  createAnode(const AnodeParams &params, G4LogicalVolume *parent);
    G4VPhysicalVolume*  createWindow(const WindowParams &params, G4LogicalVolume *parent);
    G4VPhysicalVolume*  createFilter(const FilterParams &params, G4LogicalVolume *parent);
    G4VPhysicalVolume*  createDetector(const DetectorParams &params, G4LogicalVolume *parent);
    G4VPhysicalVolume*  createDetector2(const DetectorParams2 &params, G4LogicalVolume *parent);

    G4VPhysicalVolume* anode;
    G4VPhysicalVolume* window;
    G4VPhysicalVolume* detector;
    G4VPhysicalVolume* filter;
};

#endif

