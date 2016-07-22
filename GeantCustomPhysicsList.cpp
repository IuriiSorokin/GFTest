/*
 * GeantPhysicsList.cpp
 *
 *  Created on: Jul 19, 2016
 *      Author: sorokin
 */

#include "GeantCustomPhysicsList.h"

#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Gamma.hh"
#include "G4Proton.hh"
#include "G4GenericIon.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4String.hh"
#include "G4PhotoElectricEffect.hh"
#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4eMultipleScattering.hh"
#include "G4UrbanMscModel.hh"
#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4CoulombScattering.hh"
#include "G4eCoulombScatteringModel.hh"

namespace GFTest {

GeantCustomPhysicsList::GeantCustomPhysicsList()
: G4VUserPhysicsList()
{
}



void GeantCustomPhysicsList::ConstructParticle()
{
    G4Electron::ElectronDefinition();
    G4Positron::PositronDefinition();
    G4Gamma::GammaDefinition();
    G4Proton::ProtonDefinition();
    G4GenericIon::GenericIonDefinition();
}



void GeantCustomPhysicsList::ConstructProcess()
{
    AddTransportation();

    theParticleIterator->reset();

    while( (*theParticleIterator)() ){

        G4ParticleDefinition* particle = theParticleIterator->value();
        G4ProcessManager* pmanager = particle->GetProcessManager();
        G4String particleName = particle->GetParticleName();

        if (particleName == "gamma") {

            pmanager->AddDiscreteProcess(new G4PhotoElectricEffect);
            pmanager->AddDiscreteProcess(new G4ComptonScattering);
            pmanager->AddDiscreteProcess(new G4GammaConversion);

        } else if (particleName == "e-") {
            pmanager->AddProcess(new G4eMultipleScattering, -1, 1, 1);
            pmanager->AddProcess(new G4eIonisation,         -1, 2, 2);
            pmanager->AddProcess(new G4eBremsstrahlung,     -1, 3, 3);
        }
    }
}

} /* namespace GFTest */
