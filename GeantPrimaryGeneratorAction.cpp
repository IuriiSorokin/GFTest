
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
    fParticleGun->SetParticleDefinition( G4ParticleTable::GetParticleTable()->FindParticle( gOptions->GetParticle() ) );
    fParticleGun->SetParticlePosition( G4ThreeVector( 0, 0, - gOptions->GetThickness_um() * um / 2 ) );
    fParticleGun->SetParticleMomentumDirection( G4ParticleMomentum(0,0,1) );
    fParticleGun->SetParticleEnergy( gOptions->GetEnergy_GeV() * GeV );
}



void GeantPrimaryGeneratorAction::GeneratePrimaries( G4Event * anEvent )
{
    // G4cout << " ============================ GeantPrimaryGeneratorAction::GeneratePrimaries ===============================" << G4endl;
    fParticleGun->GeneratePrimaryVertex( anEvent );
}

} /* namespace GFTest */
