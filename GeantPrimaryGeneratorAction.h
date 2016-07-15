#ifndef GEANTPRIMARYGENERATORACTION_H
#define GEANTPRIMARYGENERATORACTION_H

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include <memory>

namespace GFTest {

class GeantPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    GeantPrimaryGeneratorAction();    
    virtual void GeneratePrimaries( G4Event * );

private:
    std::unique_ptr<G4ParticleGun> fParticleGun;
};

} /* namespace GFTest */

#endif /* GEANTPRIMARYGENERATORACTION_H */
