#include "PhysicsList.hh"
#include "G4ProcessManager.hh"
#include "G4ParticleTypes.hh"

#include "QDebug"

using namespace CLHEP;

PhysicsList::PhysicsList():  G4VUserPhysicsList()
{
    defaultCutValue = 0.0001*mm;
}

PhysicsList::~PhysicsList()
{}

void PhysicsList::ConstructParticle()
{
    ConstructBosons();
    ConstructLeptons();

    G4OpticalPhoton::OpticalPhotonDefinition();
}

void PhysicsList::ConstructBosons()
{
    // gamma
    G4Gamma::GammaDefinition();
}

void PhysicsList::ConstructLeptons()
{
    // leptons
    //  e+/-
    G4Electron::ElectronDefinition();
    G4Positron::PositronDefinition();
}

void PhysicsList::ConstructProcess()
{
    AddTransportation();
    ConstructEM();
    //ConstructPenelopeEM();
}

// gamma
#include "G4PhotoElectricEffect.hh"
#include "G4LivermorePhotoElectricModel.hh"

#include "G4ComptonScattering.hh"
#include "G4LivermoreComptonModel.hh"
#include "G4LivermorePolarizedComptonModel.hh" // alternative for polarized photons

#include "G4GammaConversion.hh"
#include "G4LivermoreGammaConversionModel.hh"

#include "G4RayleighScattering.hh"
#include "G4LivermoreRayleighModel.hh"
#include "G4LivermorePolarizedRayleighModel.hh" // alternative for polarized photons

// e-

#include "G4eMultipleScattering.hh"
#include "G4UniversalFluctuation.hh"

#include "G4eIonisation.hh"
#include "G4LivermoreIonisationModel.hh"

#include "G4eBremsstrahlung.hh"
#include "G4LivermoreBremsstrahlungModel.hh"

// e+

#include "G4eplusAnnihilation.hh"
#include "G4PenelopeAnnihilationModel.hh"

//
#include "G4UAtomicDeexcitation.hh"
#include "G4LossTableManager.hh"

void PhysicsList::ConstructEM()
{
    auto theParticleIterator = GetParticleIterator();

    theParticleIterator->reset();
    while( (*theParticleIterator)() ){
        G4ParticleDefinition *particle = theParticleIterator->value();
        G4ProcessManager *pmanager = particle->GetProcessManager();
        G4String particleName = particle->GetParticleName();
        qDebug() << "Particle:" << particleName;
        if (particleName == "gamma") {
            G4PhotoElectricEffect* thePhotoElectricEffect = new G4PhotoElectricEffect();
            G4LivermorePhotoElectricModel* theLivermorePhotoElectricModel = new G4LivermorePhotoElectricModel();
            thePhotoElectricEffect->SetEmModel(theLivermorePhotoElectricModel);
            pmanager->AddDiscreteProcess(thePhotoElectricEffect);

            G4ComptonScattering* theComptonScattering = new G4ComptonScattering();
            G4LivermoreComptonModel* theLivermoreComptonModel = new G4LivermoreComptonModel();
            theComptonScattering->SetEmModel(theLivermoreComptonModel);
            pmanager->AddDiscreteProcess(theComptonScattering);

            G4GammaConversion* theGammaConversion = new G4GammaConversion();
            G4LivermoreGammaConversionModel* theLivermoreGammaConversionModel = new G4LivermoreGammaConversionModel();
            theGammaConversion->SetEmModel(theLivermoreGammaConversionModel);
            pmanager->AddDiscreteProcess(theGammaConversion);

            G4RayleighScattering* theRayleigh = new G4RayleighScattering();
            G4LivermoreRayleighModel* theRayleighModel = new G4LivermoreRayleighModel();
            theRayleigh->SetEmModel(theRayleighModel);
            pmanager->AddDiscreteProcess(theRayleigh);

            //pmanager->AddProcess(new G4StepLimiter(), -1, -1, 5);

        } else if (particleName == "e-") {
            G4eMultipleScattering* msc = new G4eMultipleScattering();
            msc->SetStepLimitType(fUseDistanceToBoundary);
            pmanager->AddProcess(msc, -1, 1, 1);

            // Ionisation
            G4eIonisation* eIoni = new G4eIonisation();
            G4LivermoreIonisationModel* theLivermoreIonisationModel = new G4LivermoreIonisationModel();
            eIoni->SetEmModel(theLivermoreIonisationModel);
            eIoni->SetStepFunction(0.2, 100*um); //
            pmanager->AddProcess(eIoni, -1, 2, 2);

            // Bremsstrahlung
            G4eBremsstrahlung* eBrem = new G4eBremsstrahlung();
            eBrem->SetEmModel(new G4LivermoreBremsstrahlungModel());
            pmanager->AddProcess(eBrem, -1,-3, 3);

            //pmanager->AddProcess(new G4StepLimiter(), -1, -1, 4);


        } else if (particleName == "e+") {
            //positron
            pmanager->AddProcess(new G4eMultipleScattering,-1, 1,1);
            pmanager->AddProcess(new G4eIonisation,       -1, 2,2);
            pmanager->AddProcess(new G4eBremsstrahlung,   -1, 3,3);
            pmanager->AddProcess(new G4eplusAnnihilation,  0,-1,4);

        }
    }

    // Deexcitation
    //
    G4VAtomDeexcitation* de = new G4UAtomicDeexcitation();
    G4LossTableManager::Instance()->SetAtomDeexcitation(de);
    de->SetFluo(true);
    de->SetAuger(true);
    de->SetPIXE(true);
    de->InitialiseForNewRun();

}

//#include "G4eMultipleScattering.hh"
//#include "G4PenelopePhotoElectricModel.hh"
//#include "G4PenelopeComptonModel.hh"
//#include "G4PenelopeGammaConversionModel.hh"
//#include "G4PenelopeRayleighModel.hh"
//#include "G4PenelopeIonisationModel.hh"
//#include "G4PenelopeBremsstrahlungModel.hh"

//void PhysicsList::ConstructPenelopeEM()
//{
//    auto theParticleIterator = GetParticleIterator();

//    theParticleIterator->reset();
//    while( (*theParticleIterator)() ){
//        G4ParticleDefinition *particle = theParticleIterator->value();
//        G4ProcessManager *pmanager = particle->GetProcessManager();
//        G4String particleName = particle->GetParticleName();

//        if (particleName == "gamma") {
//            G4PhotoElectricEffect* thePhotoElectricEffect = new G4PhotoElectricEffect();
//            G4PenelopePhotoElectricModel* thePenelopePhotoElectricModel = new G4PenelopePhotoElectricModel();
//            thePhotoElectricEffect->SetEmModel(thePenelopePhotoElectricModel);
//            pmanager->AddDiscreteProcess(thePhotoElectricEffect);

//            G4ComptonScattering* theComptonScattering = new G4ComptonScattering();
//            G4PenelopeComptonModel* thePenelopeComptonModel = new G4PenelopeComptonModel();
//            theComptonScattering->SetEmModel(thePenelopeComptonModel);
//            pmanager->AddDiscreteProcess(theComptonScattering);

//            G4GammaConversion* theGammaConversion = new G4GammaConversion();
//            G4PenelopeGammaConversionModel* thePenelopeGammaConversionModel = new G4PenelopeGammaConversionModel();
//            theGammaConversion->SetEmModel(thePenelopeGammaConversionModel);
//            pmanager->AddDiscreteProcess(theGammaConversion);

//            G4RayleighScattering* theRayleigh = new G4RayleighScattering();
//            G4PenelopeRayleighModel* thePenelopeRayleighModel = new G4PenelopeRayleighModel();
//            theRayleigh->SetEmModel(thePenelopeRayleighModel);
//            pmanager->AddDiscreteProcess(theRayleigh);

//            //pmanager->AddProcess(new G4StepLimiter(), -1, -1, 5);

//        } else if (particleName == "e-") {
//            G4eMultipleScattering* msc = new G4eMultipleScattering();
//            msc->SetStepLimitType(fUseDistanceToBoundary);
//            pmanager->AddProcess(msc, -1, 1, 1);

//            // Ionisation
//            G4eIonisation* eIoni = new G4eIonisation();
//            G4PenelopeIonisationModel* thePenelopeIonisationModel = new G4PenelopeIonisationModel();
//            eIoni->SetEmModel(thePenelopeIonisationModel);
//            eIoni->SetStepFunction(0.2, 100*um); //
//            pmanager->AddProcess(eIoni, -1, 2, 2);

//            // Bremsstrahlung
//            G4eBremsstrahlung* eBrem = new G4eBremsstrahlung();
//            eBrem->SetEmModel(new G4PenelopeBremsstrahlungModel());
//            //eBrem->SetStepFunction(0.2, 2*um);
//            pmanager->AddProcess(eBrem, -1,-3, 3);

//            //pmanager->AddProcess(new G4StepLimiter(), -1, -1, 4);


//        } else if (particleName == "e+") {
//            //positron
//            pmanager->AddProcess(new G4eMultipleScattering,-1, 1,1);
//            pmanager->AddProcess(new G4eIonisation,       -1, 2,2);
//            pmanager->AddProcess(new G4eBremsstrahlung,   -1, 3,3);
//            pmanager->AddProcess(new G4eplusAnnihilation,  0,-1,4);

//        }
//    }

//    // Deexcitation
//    //
//    G4VAtomDeexcitation* de = new G4UAtomicDeexcitation();
//    G4LossTableManager::Instance()->SetAtomDeexcitation(de);
//    de->SetFluo(true);
//    de->SetAuger(true);
//    de->SetPIXE(true);
//    de->InitialiseForNewRun();

//}

void PhysicsList::SetCuts()
{
    SetCutsWithDefault();
}
