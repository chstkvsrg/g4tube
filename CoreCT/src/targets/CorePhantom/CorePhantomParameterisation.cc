//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file CorePhantomParameterisationColour.cc
/// \brief Implementation of the CorePhantomParameterisationColour class
///
#include "CorePhantomParameterisation.hh"

#include "globals.hh"
#include "G4VisAttributes.hh"
#include "G4Material.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4SystemOfUnits.hh"


#include "G4VisAttributes.hh"
#include "G4VVisManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CorePhantomParameterisation::CorePhantomParameterisation(CorePhantomPhys* Core_phys)
    : G4VPVParameterisation(),
      fCore_phys(Core_phys)
{
    fMaterial_array = fCore_phys->GetBallsMaterial();
    fRadius_array = fCore_phys->GetBallsRadius();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
CorePhantomParameterisation::~CorePhantomParameterisation()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4int CorePhantomParameterisation::Layer(G4int coNum) const
{
    return coNum/(fCore_phys->GetSegmentsNumber());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int CorePhantomParameterisation::Segment(G4int coNum) const
{
    return coNum%(fCore_phys->GetSegmentsNumber());
}

G4Material* CorePhantomParameterisation::ComputeMaterial(const G4int copyNo, G4VPhysicalVolume *physVol ,
                                                  const G4VTouchable *)
{
    G4int copyLayer = Layer(copyNo);

    G4VisAttributes* vis = fCore_phys->GetGrayScale(copyLayer);

    physVol->GetLogicalVolume()->SetVisAttributes(vis);

    return fMaterial_array[copyLayer];
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CorePhantomParameterisation::ComputeDimensions(G4Sphere& Sphere_solid, const G4int repNo, const G4VPhysicalVolume*) const
{
    G4int copySegment = Segment(repNo);

    G4double Current_radius = fRadius_array[copySegment];

    Sphere_solid.SetOuterRadius(Current_radius);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CorePhantomParameterisation::ComputeTransformation(const G4int copyNo, G4VPhysicalVolume* physVol) const
{
    G4ThreeVector Zero_pose = fCore_phys->GetZeroBall().pose;
    G4int Current_segment = Segment(copyNo);
    G4int Current_layer = Layer(copyNo);

    G4ThreeVector Delta_pose = Zero_pose.rotateZ(2*M_PI*Current_segment/(fCore_phys->GetSegmentsNumber())) + G4ThreeVector(0,0,Current_layer*(fCore_phys->GetBallStepHeight()));

    physVol->SetRotation(0);
    physVol->SetTranslation(Delta_pose);
    G4String name = std::string("Ball") + std::string("_") + std::to_string(Current_layer) + std::string("_") + std::to_string(Current_segment);
    physVol->SetName(name);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
