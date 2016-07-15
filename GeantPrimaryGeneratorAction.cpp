
#include "GeantPrimaryGeneratorAction.h"

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Options.h"

namespace GFTest {

GeantPrimaryGeneratorAction::GeantPrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction()
{
    fParticleGun.reset( new G4ParticleGun( 1 ) );
    fParticleGun->SetParticleDefinition( G4ParticleTable::GetParticleTable()->FindParticle( gOptions->GetPartType() ) );
    fParticleGun->SetParticlePosition( G4ThreeVector( gOptions->GetPartX_cm() * cm, gOptions->GetPartY_cm() * cm, gOptions->GetPartZ_cm() * cm ) );
    fParticleGun->SetParticleMomentum( G4ParticleMomentum( gOptions->GetPartPx_GeVc() * GeV, gOptions->GetPartPy_GeVc() * GeV, gOptions->GetPartPz_GeVc() * GeV ) );
}



void GeantPrimaryGeneratorAction::GeneratePrimaries( G4Event * anEvent )
{
    fParticleGun->GeneratePrimaryVertex( anEvent );
}

} /* namespace GFTest */
