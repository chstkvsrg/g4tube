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
/// \file CoreVoxelParameterisationColour.cc
/// \brief Implementation of the CoreVoxelParameterisationColour class
///
#include "CoreVoxelParameterisation.hh"

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

CoreVoxelParameterisation::CoreVoxelParameterisation()
    : G4PhantomParameterisation(),
      fReference_density(0.), fReference_grayscale(0.)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CoreVoxelParameterisation::CoreVoxelParameterisation(G4double Reference_density, G4double Reference_grayscale)
    : G4PhantomParameterisation(),
      fReference_density(Reference_density), fReference_grayscale(Reference_grayscale)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
CoreVoxelParameterisation::~CoreVoxelParameterisation()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CoreVoxelParameterisation::SetReferenceVariables(G4double Reference_density, G4double Reference_grayscale)
{
    fReference_density = Reference_density;
    fReference_grayscale = Reference_grayscale;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4Material* CoreVoxelParameterisation::ComputeMaterial(const G4int copyNo, G4VPhysicalVolume * physVol,
                const G4VTouchable *)
{
    G4Material* mate = G4PhantomParameterisation::ComputeMaterial(
        copyNo, physVol, 0 );

    if(G4VVisManager::GetConcreteInstance() && physVol)
    {
        G4double Materials_density = mate->GetDensity();
        G4double Material_grayscale = fReference_density*fReference_grayscale/Materials_density;
        G4VisAttributes* Voxel_vis = new G4VisAttributes(G4Color(Material_grayscale, Material_grayscale, Material_grayscale));
        physVol->GetLogicalVolume()->SetVisAttributes(Voxel_vis);
    }

    return mate;
}
