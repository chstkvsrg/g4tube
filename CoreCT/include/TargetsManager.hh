#pragma once

#include "HistManager.hh"
#include "corect_ggo.h"

class ITargetHist;
class G4VUserDetectorConstruction;
class G4VUserPrimaryGeneratorAction;

class TargetsManager
{
public:
	static TargetsManager* instance();
	static void free();

	enum_target target();
	void setTarget(enum_target & target);

	G4VUserDetectorConstruction* createDetectorConstruction();
    G4VUserPrimaryGeneratorAction* createPrimaryGeneratorAction();

    G4double GetSourceTargetDist() {return fSourceTarget_dist;}
    G4double GetTargetDetDist() {return fTargetDet_dist;}

	void initHistogram();
	void saveHisto();

private:

	static TargetsManager* p_;
	ITargetHist *hist_;
	enum_target target_;

    G4double fSourceTarget_dist = 0.;
    G4double fTargetDet_dist = 0.;
    G4double fCore_radius = 0.;
    G4double fCore_heighthalf = 0.;

    G4double fDetX_size = 0.;
    G4double fDetY_size = 0.;
    G4int fDetX_number = 0;
    G4int fDetY_number = 0;

    void ComputeCoreRadius();

	TargetsManager();
    ~TargetsManager();



};




