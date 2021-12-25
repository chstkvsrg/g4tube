//Detector construction source

#include "CorePhantomPhys.hh"
#include "CorePhantomParameterisation.hh"
#include "G4PVParameterised.hh"


#include "G4NistManager.hh"
#include "G4SDManager.hh"


#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"


#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4SubtractionSolid.hh"
#include "G4PVReplica.hh"
#include "G4ReflectionFactory.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"
#include "defines.hh"


#include <cmath>
#include <iostream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CorePhantomPhys::CorePhantomPhys() :
    fMother(nullptr), fRotation(nullptr), fPosition(G4ThreeVector())
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CorePhantomPhys::CorePhantomPhys(G4LogicalVolume *Mother_logic, G4RotationMatrix *Rotation, G4ThreeVector Position) :
    fMother(Mother_logic), fRotation(Rotation), fPosition(Position)
{
    if (Rotation == 0 || Rotation == nullptr || Rotation == NULL)
    {
        fRotation = new G4RotationMatrix();
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CorePhantomPhys::~CorePhantomPhys()
{ 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CorePhantomPhys::SetLogRotPos(G4LogicalVolume* Mother_logic, G4RotationMatrix* Rotation, G4ThreeVector Position)
{
    fMother = Mother_logic;
    if (Rotation == 0 || Rotation == nullptr || Rotation == NULL)
    {
        fRotation = new G4RotationMatrix();
    }

    fRotation = Rotation;
    fPosition = Position;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CorePhantomPhys::SetCore(G4double Core_radius, G4double Core_height, G4Material* Core_mat)
{
    fCore_radius = Core_radius;
    fCore_heigth = Core_height;
    fCore_mat = Core_mat;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CorePhantomPhys::SetZeroBall(G4double Ball_radius, G4ThreeVector Ball_pose, G4Material* Ball_mat)
{
    fZeroBall.radius = Ball_radius;
    fZeroBall.pose = Ball_pose;
    fZeroBall.mat = Ball_mat;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CorePhantomPhys::SetSegments(G4int Segments_number, G4double BallStep_radius)
{
    fSegments_number = Segments_number;

    for(G4int Index = 0; Index < fSegments_number; Index++)
    {
        G4double Ball_radius = fZeroBall.radius + BallStep_radius*Index;
        fBalls_radius.push_back(Ball_radius);
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CorePhantomPhys::SetSegments(std::vector<G4double> Balls_radius)
{
    fSegments_number = (G4int) Balls_radius.size();
    fBalls_radius = Balls_radius;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CorePhantomPhys::SetLayers(G4int Layers_number, G4double BallStep_density, G4double BallStep_height)
{
    fLayers_number = Layers_number;
    fBallStep_height = BallStep_height;

    fBalls_material.push_back(fZeroBall.mat);

    for(G4int Index = 1; Index < fLayers_number; Index++)
    {
        G4double Ball_density = fZeroBall.mat->GetDensity() + BallStep_density*Index;

        G4Material* Ball_mat = new G4Material(fZeroBall.mat->GetName() + (G4String) "_" + std::to_string(Index),
                                              Ball_density,
                                              fZeroBall.mat);
        fBalls_material.push_back(Ball_mat);
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CorePhantomPhys::SetLayers(std::vector<G4double> BallsCoof_density, G4double BallStep_height)
{
    fLayers_number = (G4int) BallsCoof_density.size();
    fBallStep_height = BallStep_height;

    for (G4double ball_coofdensity : BallsCoof_density)
    {
        G4double Ball_density = fZeroBall.mat->GetDensity()*ball_coofdensity;

        G4Material* Ball_mat = new G4Material(fZeroBall.mat->GetName() + (G4String) "_" + std::to_string(ball_coofdensity),
                                              Ball_density,
                                              fZeroBall.mat);
        fBalls_material.push_back(Ball_mat);
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CorePhantomPhys::SetLayers(std::vector<G4Material*> Balls_mat, G4double BallStep_height)
{
    fLayers_number = (G4int) Balls_mat.size();
    fBallStep_height = BallStep_height;

    fBalls_material = Balls_mat;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CorePhantomPhys::SetGrayScale(G4double Reference_grayscale, G4double Delta_grayscale)
{
   for (G4int Index = 0 ; Index < fLayers_number; Index++)
   {
       G4double Grayscale = Reference_grayscale + Delta_grayscale*Index;

       G4VisAttributes* vis = new G4VisAttributes(G4Color(Grayscale, Grayscale, Grayscale));
       vis->SetVisibility(true);
       fBalls_vis.push_back(vis);
   }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CorePhantomPhys::SetGrayScale(std::vector<G4double> Balls_grayscale)
{
   if ((G4int) Balls_grayscale.size() != fLayers_number)
   {
       std::cout << "CorePhantomPhys ERROR, check sizes of GrayScale and Layers" << std::endl;
       std::cout << "Grayscale size is - " << Balls_grayscale.size() << std::endl;
       std::cout << "Layers number is - " << fLayers_number << std::endl;

       exit(2);
   }

   for (G4double ball_vis : Balls_grayscale)
   {
       G4VisAttributes* vis = new G4VisAttributes(G4Color(ball_vis, ball_vis, ball_vis));
       vis->SetVisibility(true);
       fBalls_vis.push_back(vis);
   }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CorePhantomPhys::AppendSegment(G4double Ball_radius)
{
   fBalls_radius.push_back(Ball_radius);
   fSegments_number++;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CorePhantomPhys::AppendLayer(G4double BallCoof_density)
{
   fLayers_number++;
   G4Material* material = new G4Material(fZeroBall.mat->GetName() + std::to_string(fLayers_number-1),
                                    fZeroBall.mat->GetDensity()*BallCoof_density,
                                    fZeroBall.mat);

   fBalls_material.push_back(material);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CorePhantomPhys::AppendLayer(G4Material* Ball_material)
{
   fLayers_number++;
   fBalls_material.push_back(Ball_material);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



Ball CorePhantomPhys::GetZeroBall()
{
    return fZeroBall;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int CorePhantomPhys::GetSegmentsNumber()
{
    return fSegments_number;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

std::vector<G4double> CorePhantomPhys::GetBallsRadius()
{
    return fBalls_radius;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int CorePhantomPhys::GetLayersNumber()
{
    return fLayers_number;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

std::vector<G4Material*> CorePhantomPhys::GetBallsMaterial()
{
    return fBalls_material;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double CorePhantomPhys::GetBallStepHeight()
{
    return fBallStep_height;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VisAttributes* CorePhantomPhys::GetGrayScale(G4int Layer)
{
    return fBalls_vis[Layer];
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CorePhantomPhys::Construct()
{

    G4Tubs* Core_solid = new G4Tubs("Core",
                                    0., fCore_radius,
                                    fCore_heigth/2.,
                                    0.,2*M_PI);

    G4LogicalVolume* Core_logic = new G4LogicalVolume(Core_solid,
                                                      fCore_mat,
                                                      "Core");

    /*G4PVPlacement* Core_phys =*/ new G4PVPlacement(G4Transform3D(
                                                         *fRotation,
                                                         fPosition),
                                                     Core_logic,
                                                     "Core",
                                                     fMother,
                                                     false,
                                                     0,
                                                     true
                                                     );

    G4VisAttributes* Core_vis = new G4VisAttributes(G4Colour(80./255, 80./255, 80./255, 0.5));
    Core_logic->SetVisAttributes(Core_vis);

    G4Sphere* Ball_solid = new G4Sphere("Zero_ball",
                                        0., fZeroBall.radius,
                                        0, 2*M_PI,
                                        0, M_PI);

    G4LogicalVolume* Ball_logic = new G4LogicalVolume(Ball_solid,
                                                      fZeroBall.mat,
                                                      "Zero_ball");


    CorePhantomParameterisation* param = new CorePhantomParameterisation(this);

    new G4PVParameterised("Balls",
                          Ball_logic,
                          Core_logic,
                          kUndefined,
                          fLayers_number*fSegments_number,
                          param,
                          true);


    //return Core_phys;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



