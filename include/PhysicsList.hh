#ifndef PhysicsList_h
#define PhysicsList_h 1

#include "G4VUserPhysicsList.hh"

class PhysicsList: public G4VUserPhysicsList
{
public:
    PhysicsList();
    ~PhysicsList();

protected:
    void ConstructParticle();
    void ConstructProcess();
    void SetCuts();

protected:
    void ConstructBosons();
    void ConstructLeptons();

protected:
//    void ConstructPenelopeEM();
    void ConstructEM();
};

#endif


