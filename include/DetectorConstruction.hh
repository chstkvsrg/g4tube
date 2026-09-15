#ifndef DetectorConstruction_H
#define DetectorConstruction_H 1

#include "G4VUserDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"

#include <vector>

class AnodeMessenger;
class DetectorMessenger;
class FilterMessenger;
class SensitiveDetector;

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction();
    ~DetectorConstruction();

    G4VPhysicalVolume* Construct();

    virtual void ConstructSDandField();

    void SetAnodeMaterial(const G4String &material) { fAnodeParams.material = material; }
    void SetAnodeThickness(G4double thickness)      { fAnodeParams.thick = thickness; }
    void SetAnodeAngle(G4double angle)              { fAnodeParams.angle = angle; }

    void SetFilterMaterial(const G4String &material) { fFilterParams.material = material; }
    void SetFilterThickness(G4double thickness)      { fFilterParams.thick = thickness; }
    void SetFilterDiameter(G4double diameter)        { fFilterParams.size = diameter; }
    void SetFilterPosition(const G4ThreeVector &pos) { fFilterParams.pos = pos; }
    void SetFilterNormal(const G4ThreeVector &normal) { fFilterParams.normal = normal; }

    struct AnodeParams {
        G4String material = "G4_W";
        G4double size  = 10*mm;
        G4double thick = 5*mm;
        G4double angle = 11*deg;
    };

    const AnodeParams &GetAnodeParams() const { return fAnodeParams; }

    struct WindowParams {
        G4double size  = 20*mm;
        G4double thick = 0.8*mm;
        G4double angle = 90*deg;
        G4ThreeVector pos = G4ThreeVector(0, -7*mm, 20*mm);
    };

    struct FilterParams {
        G4String      material = "G4_Al";
        G4double      size     = 35*mm;                       // диаметр пластины
        G4double      thick    = 10.0*mm;                     // толщина пластины
        G4ThreeVector normal   = G4ThreeVector(0, -1, 0);     // нормаль к плоскости
        G4ThreeVector pos      = G4ThreeVector(0, -17.8*mm, 20*mm);
    };

    const FilterParams &GetFilterParams() const { return fFilterParams; }

    // детектор: панель из N чувствительных полос; локальная ось Y разбивается на полосы
    struct DetectorParams {
        G4String      name;
        G4ThreeVector center;   // позиция центра панели (mm)
        G4ThreeVector size;     // полные размеры панели (mm), полосы вдоль local Y
        G4ThreeVector normal;   // нормаль к плоскости панели (любой вектор, нормируется)
        G4int         nStrips;  // число полос по local Y
    };

    void AddDetector(const DetectorParams& params) { fDetectorParamsVec.push_back(params); }
    void ClearDetectors()                          { fDetectorParamsVec.clear(); }
    const std::vector<DetectorParams>& GetDetectors() const { return fDetectorParamsVec; }

    G4VPhysicalVolume* createAnode(const AnodeParams &params, G4LogicalVolume *parent);
    G4VPhysicalVolume* createWindow(const WindowParams &params, G4LogicalVolume *parent);
    G4VPhysicalVolume* createFilter(const FilterParams &params, G4LogicalVolume *parent);
    G4VPhysicalVolume* createDetectorPanel(const DetectorParams &params, G4LogicalVolume *parent);

    G4VPhysicalVolume* anode;
    G4VPhysicalVolume* window;
    G4VPhysicalVolume* detector;
    G4VPhysicalVolume* filter;

  private:
    AnodeParams                  fAnodeParams;
    AnodeMessenger*              fAnodeMessenger;
    DetectorMessenger*           fDetectorMessenger;
    FilterMessenger*             fFilterMessenger;
    FilterParams                 fFilterParams;
    G4VPhysicalVolume*           fWorldPhys;
    SensitiveDetector*           fSDCore;
    std::vector<DetectorParams>  fDetectorParamsVec;
    std::vector<G4LogicalVolume*> fSensitiveStripLogics;
};

#endif