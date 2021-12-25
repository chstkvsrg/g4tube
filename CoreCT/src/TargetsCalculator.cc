
#include "TargetsCalculator.hh"

#include "CoreVoxelPrimaryGeneratorAction.hh"
#include "RoundHist.hh"
#include "CoreVoxelDetectorConstruction.hh"

#include "CorePhantomPrimaryGeneratorAction.hh"
#include "CorePhantomHist.hh"
#include "CorePhantomDetectorConstruction.hh"
#include "corect_ggo.h"


#include <iostream>
#include <fstream>

extern args_info_corect args_info;

TargetsCalculator* TargetsCalculator::p_ = nullptr;

TargetsCalculator::TargetsCalculator()
{
}

TargetsCalculator::~TargetsCalculator()
{
}

TargetsCalculator* TargetsCalculator::instance()
{
    if (!p_)
        p_ = new TargetsCalculator();
    return p_;
}

void TargetsCalculator::free()
{
    if (p_) delete p_;
    p_ = nullptr;
}

void TargetsCalculator::SetValues(G4double SourceTarget_dist, G4double TargetDet_dist, G4double Core_radius, G4double Core_heighthalf,
                                  G4double DetX_size, G4double DetY_size, G4int DetX_number, G4int DetY_number,
                                  G4double SourcePhiTanAngle, G4double SourceThetaTanAngle)
{
    fSourceTarget_dist = SourceTarget_dist;
    fTargetDet_dist = TargetDet_dist;
    fCore_radius = Core_radius;
    fCore_heighthalf = Core_heighthalf;

    fDetX_size = DetX_size;
    fDetY_size = DetY_size;
    fDetX_number = DetX_number;
    fDetY_number = DetY_number;

    fSourcePhiTanAngle = SourcePhiTanAngle;
    fSourceThetaTanAngle = SourceThetaTanAngle;

    fSourcePhiAngle = std::atan(fSourcePhiTanAngle);
    fSourceThetaAngle = std::atan(fSourceThetaTanAngle);
}

void TargetsCalculator::SetValuesByTarget(enum_target &target)
{
    switch (target)
    {
    case target_arg_CorePhantom: fSourceTarget_dist = 30*cm; fTargetDet_dist = 30*cm; fCore_radius = 1.5*cm; fCore_heighthalf = 0.25*cm/2.0; fDetX_size = 50*micrometer; fDetY_size = 50*micrometer; break;//fCore_heighthalf = 5*cm/2.0; fDetX_size = 100*micrometer; fDetY_size = 100*micrometer; break;
    case target_arg_CoreVoxel: fSourceTarget_dist = 30*cm; fTargetDet_dist = 30*cm; ImportVoxelData("INPUT/Voxels.ml2g4"); fDetX_size = 1*mm; fDetY_size = 1* mm; break;
    case target__NULL: std::cout << "Target not found, check Calculator or target" << std::endl; break;
    }

    fSourcePhiTanAngle = fCore_radius/std::sqrt(fSourceTarget_dist*fSourceTarget_dist-fCore_radius*fCore_radius);
    fSourceThetaTanAngle = fCore_heighthalf/(fSourceTarget_dist-fCore_radius);

    fSourcePhiAngle = std::atan(fSourcePhiTanAngle);
    fSourceThetaAngle = std::atan(fSourceThetaTanAngle);

    fDetX_number = (G4int) std::ceil(2.*fSourcePhiTanAngle*(fSourceTarget_dist+fTargetDet_dist)/fDetX_size);
    fDetY_number = (G4int) std::ceil(2.*fSourceThetaTanAngle*(fSourceTarget_dist+fTargetDet_dist)/(fDetY_size));
}

void TargetsCalculator::SetCompletedEvents(unsigned long long Events_number)
{
    fEventsCompleted = Events_number;
}

void TargetsCalculator::ImportVoxelData(G4String Path)
{
    G4double HalfX = 0;
    G4double HalfY = 0;
    G4double HalfZ = 0;
    G4int VoxelsX_number = 0;
    G4int VoxelsY_number = 0;
    G4int VoxelsZ_number = 0;

    std::ifstream invox(Path, std::ios::binary|std::ios::in);
    if(!invox)
    {
        std::cout << "Cannot open file!" << std::endl;
        exit(1);
    }
    invox.read((char*) &HalfX, sizeof(HalfX));
    invox.read((char*) &HalfY, sizeof(HalfY));
    invox.read((char*) &HalfZ, sizeof(HalfZ));
    invox.read((char*) &VoxelsX_number, sizeof(VoxelsX_number));
    invox.read((char*) &VoxelsY_number, sizeof(VoxelsY_number));
    invox.read((char*) &VoxelsZ_number, sizeof(VoxelsZ_number));
    invox.close();

    fCore_radius = (((HalfX*VoxelsX_number) >= (HalfY*VoxelsY_number)) ? HalfX*VoxelsX_number*mm : HalfY*VoxelsY_number*mm);
    fCore_heighthalf = HalfZ*VoxelsZ_number*mm;
}

void TargetsCalculator::SaveDetectorParams(G4String Name)
{
    std::ofstream out;
    out.open("OUTPUT/ParamsHeader" + Name);
    if (!out.is_open())
    {
        std::cout << "Can't create file for Det params :(" << std::endl;
    }

    out << "Source to core distance: " << fSourceTarget_dist << std::endl;
    out << "Core to Det distance: " << fTargetDet_dist << std::endl;
    out << "Core radius: " << fCore_radius << std::endl;
    out << "Core height half: " << fCore_heighthalf << std::endl;
    out << "Det X cell size: " << fDetX_size << std::endl;
    out << "Det Y cell size: " << fDetY_size << std::endl;
    out << "Det X number of cells: " << fDetX_number << std::endl;
    out << "Det Y number of cells: " << fDetY_number << std::endl;
    out << "Source tan of phi angle: " << fSourcePhiTanAngle << std::endl;
    out << "Source tan of theta angle: " << fSourceThetaTanAngle << std::endl;
    out << "Number of Events: " << fEventsCompleted << std::endl;

    out.close();
}

