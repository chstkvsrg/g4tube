
#include "TargetsManager.hh"
#include "TargetsCalculator.hh"

#include "CoreVoxelPrimaryGeneratorAction.hh"
#include "RoundHist.hh"
#include "CoreVoxelDetectorConstruction.hh"

#include "CorePhantomPrimaryGeneratorAction.hh"
#include "CorePhantomHist.hh"
#include "CorePhantomDetectorConstruction.hh"


TargetsManager* TargetsManager::p_ = nullptr;

TargetsManager::TargetsManager() :
    hist_(nullptr), target_(target__NULL)
{
}

TargetsManager::~TargetsManager()
{
    if (hist_) delete hist_;
}

TargetsManager* TargetsManager::instance()
{
    if (!p_)
        p_ = new TargetsManager();
    return p_;
}

void TargetsManager::free()
{
    if (p_) delete p_;
    p_ = nullptr;
}

enum_target TargetsManager::target()
{
    return target_;
}

void TargetsManager::setTarget(enum_target & target)
{
    target_ = target;

    if (hist_) delete hist_;
    switch (target_)
    {
        case target_arg_CorePhantom: hist_ = new CorePhantomHist(); break;
        case target_arg_CoreVoxel: hist_ = new CorePhantomHist(); break;

        case target__NULL: break;
    }

    TargetsCalculator::instance()->SetValuesByTarget(target_);

}

G4VUserDetectorConstruction* TargetsManager::createDetectorConstruction()
{
    switch (target_)
    {
    case target_arg_CorePhantom: return new CorePhantomDetectorConstruction();
    case target_arg_CoreVoxel: return new CoreVoxelDetectorConstruction();
    case target__NULL: break;
    }

    return nullptr;
}

G4VUserPrimaryGeneratorAction* TargetsManager::createPrimaryGeneratorAction()
{
    switch (target_)
    {
    case target_arg_CorePhantom: return new CorePhantomPrimaryGeneratorAction();
    case target_arg_CoreVoxel: return new CoreVoxelPrimaryGeneratorAction();
    case target__NULL: break;
    }

    return nullptr;
}

void TargetsManager::initHistogram()
{
    hist_->initHistogram();
}

void TargetsManager::saveHisto()
{
    hist_->saveHisto();
}



