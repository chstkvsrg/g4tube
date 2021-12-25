#pragma once

#include "HistManager.hh"
#include "corect_ggo.h"

class ITargetHist;
class G4VUserDetectorConstruction;
class G4VUserPrimaryGeneratorAction;

class TargetsCalculator
{
public:
    static TargetsCalculator* instance();
	static void free();

    //Set Methods

    void SetValues(G4double SourceTarget_dist, G4double TargetDet_dist, G4double Core_radius, G4double Core_heighthalf,
                   G4double DetX_size, G4double DetY_size, G4int DetX_number, G4int DetY_number,
                   G4double SourcePhiTanAngle, G4double SourceThetaTanAngle);  //not recomended, only for pro geometry boss
    void SetCompletedEvents(unsigned long long Events_number);
    void SetValuesByTarget(enum_target & target);

    //Get Methods

    G4double GetSourceTargetDist() {return fSourceTarget_dist;}
    G4double GetTargetDetDist() {return fTargetDet_dist;}
    G4double GetCoreRadius() {return fCore_radius;}
    G4double GetCoreHeightHalf() {return fCore_heighthalf;}
    G4double GetDetXSize() {return fDetX_size;}
    G4double GetDetYSize() {return fDetY_size;}
    G4double GetDetXNumber() {return fDetX_number;}
    G4double GetDetYNumber() {return fDetY_number;}
    G4double GetSourcePhiTanAngle() {return fSourcePhiTanAngle;}
    G4double GetSourceThetaTanAngle() {return fSourceThetaTanAngle;}
    G4double GetSourcePhiAngle() {return fSourcePhiAngle;}
    G4double GetSourceThetaAngle() {return fSourceThetaAngle;}
    void SaveDetectorParams(G4String Name);

private:

    static TargetsCalculator* p_;

    G4double fSourceTarget_dist = 0.;
    G4double fTargetDet_dist = 0.;
    G4double fCore_radius = 0.;
    G4double fCore_heighthalf = 0.;

    G4double fDetX_size = 0.;
    G4double fDetY_size = 0.;
    G4int fDetX_number = 0;
    G4int fDetY_number = 0;

    G4double fSourcePhiTanAngle = 0.;
    G4double fSourceThetaTanAngle = 0.;
    G4double fSourcePhiAngle = 0.;
    G4double fSourceThetaAngle = 0.;
    G4double fEventsCompleted = 0.;

    void ImportVoxelData(G4String Path);

    TargetsCalculator();
    ~TargetsCalculator();



};




