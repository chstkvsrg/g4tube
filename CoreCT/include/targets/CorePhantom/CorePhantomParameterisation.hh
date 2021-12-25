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
/// \file medical/DICOM/include/CorePhantomParameterisationColour.hh
/// \brief Definition of the CorePhantomParameterisationColour class
//

#ifndef CorePhantomParameterisation_HH
#define CorePhantomParameterisation_HH

#include "G4VPVParameterisation.hh"
#include "G4Material.hh"
#include "CorePhantomPhys.hh"
#include "G4Sphere.hh"

#include <vector>

class G4VisAttributes;

// *********************************************************************

class CorePhantomParameterisation : public G4VPVParameterisation
{

public:  // with description
    CorePhantomParameterisation(CorePhantomPhys* Core_phys);
    ~CorePhantomParameterisation();

    virtual G4Material* ComputeMaterial(const G4int repNo,
                                        G4VPhysicalVolume *physVol,
                                        const G4VTouchable *parentTouch = 0);

    void ComputeDimensions(G4Sphere& Sphere_solid,
                                   const G4int repNo,
                                   const G4VPhysicalVolume * = 0) const;

    void ComputeTransformation(const G4int copyNo,
                               G4VPhysicalVolume* physVol) const;

private:

    CorePhantomPhys* fCore_phys;

    G4int Layer(G4int coNum) const;
    G4int Segment(G4int coNum) const;

    G4VisAttributes* GetGrayScale(G4int layer);

    std::vector<G4Material*> fMaterial_array = {};
    std::vector<G4double> fRadius_array = {};

    void ComputeDimensions (G4Box&,const G4int,
                            const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Trd&,const G4int,
                            const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Trap&,const G4int,
                            const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Cons&,const G4int,
                            const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Tubs&,const G4int,
                            const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Orb&,const G4int,
                            const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Ellipsoid&,const G4int,
                            const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Torus&,const G4int,
                            const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Para&,const G4int,
                            const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Hype&,const G4int,
                            const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Polycone&,const G4int,
                            const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Polyhedra&,const G4int,
                            const G4VPhysicalVolume*) const {}

};


#endif
