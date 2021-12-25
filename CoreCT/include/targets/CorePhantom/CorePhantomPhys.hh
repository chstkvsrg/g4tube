//Detector construction header

#ifndef CorePhantomPhys_h
#define CorePhantomPhys_h 1

#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "globals.hh"
#include "G4Material.hh"



// materials and geometry.

typedef struct
{
    G4double radius = 0.;
    G4ThreeVector pose = G4ThreeVector();
    G4Material* mat = nullptr;

} Ball;

class CorePhantomPhys
{
public:
    CorePhantomPhys();
    CorePhantomPhys(G4LogicalVolume* Mother_logic, G4RotationMatrix* Rotation, G4ThreeVector Position);
    ~CorePhantomPhys();

public:

    //set methods

    void SetLogRotPos(G4LogicalVolume* Mother_logic, G4RotationMatrix* Rotation, G4ThreeVector Position);

    void SetCore(G4double Core_radius, G4double Core_height, G4Material* Core_mat);

    void SetZeroBall(G4double Ball_radius, G4ThreeVector Ball_pose, G4Material* Ball_mat);

    void SetSegments(G4int Segments_number, G4double BallStep_radius);                          //constant step
    void SetSegments(std::vector<G4double> Balls_radius);                //user step

    void SetLayers(G4int Layers_number, G4double BallStep_density, G4double BallStep_height);
    void SetLayers(std::vector<G4double> BallsCoof_density, G4double BallStep_height);
    void SetLayers(std::vector<G4Material*> Balls_mat, G4double BallStep_height);

    void SetGrayScale(G4double Reference_grayscale, G4double Delta_grayscale);
    void SetGrayScale(std::vector<G4double> Balls_grayscale);

    //Append methods

    void AppendSegment(G4double Ball_radius);
    void AppendLayer(G4double BallCoof_density);
    void AppendLayer(G4Material* Ball_material);

    //get methods

    Ball GetZeroBall();
    G4int GetSegmentsNumber();
    std::vector<G4double> GetBallsRadius();
    G4int GetLayersNumber();
    std::vector<G4Material*> GetBallsMaterial();

    G4double GetBallStepHeight();
    G4VisAttributes* GetGrayScale(G4int Layer);

    void Construct();

private:

    G4LogicalVolume* fMother;
    G4RotationMatrix* fRotation;
    G4ThreeVector fPosition;

    //Core
    G4double fCore_radius = 0.;
    G4double fCore_heigth = 0.;
    G4Material* fCore_mat = nullptr;

    //ZeroBall
    Ball fZeroBall;

    //Segment

    G4int fSegments_number = 0;
    std::vector<G4double> fBalls_radius = {};

    //Layer

    G4int fLayers_number = 0;
    std::vector<G4Material*> fBalls_material = {};
    G4double fBallStep_height = 0.;

    //Visual

    std::vector<G4VisAttributes*> fBalls_vis = {};

    //Parametrization

    G4bool fCheckOverlaps = true; // option to activate checking of volumes overlaps
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

