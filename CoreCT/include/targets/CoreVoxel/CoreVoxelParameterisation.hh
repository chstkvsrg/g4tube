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
/// \file medical/DICOM/include/CoreVoxelParameterisationColour.hh
/// \brief Definition of the CoreVoxelParameterisationColour class
//

#ifndef CoreVoxelParameterisation_HH
#define CoreVoxelParameterisation_HH

#include "G4PhantomParameterisation.hh"
#include "G4Material.hh"
class G4VisAttributes;

// *********************************************************************

class CoreVoxelParameterisation : public G4PhantomParameterisation
{

public:  // with description
    CoreVoxelParameterisation();
    CoreVoxelParameterisation(G4double Reference_density, G4double Reference_grayscale);
    ~CoreVoxelParameterisation();

    virtual G4Material* ComputeMaterial(const G4int repNo,
                                        G4VPhysicalVolume *currentVol,
                                        const G4VTouchable *parentTouch=0);

    void SetReferenceVariables(G4double Reference_density, G4double Reference_grayscale);

private:

    G4double fReference_density;
    G4double fReference_grayscale;
};


#endif
