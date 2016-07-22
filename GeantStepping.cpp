/*
 * GeantStepping.cpp
 *
 *  Created on: Jul 19, 2016
 *      Author: sorokin
 */

#include "GeantStepping.h"
#include "G4Step.hh"
#include "G4SystemOfUnits.hh"
#include "G4VProcess.hh"
#include "Options.h"

namespace GFTest {

GeantStepping::GeantStepping()
{
}

GeantStepping::~GeantStepping()
{
}

void GeantStepping::UserSteppingAction( const G4Step* step)
{
    if( gOptions->GetGeantTrace() ) {
        if( step->GetPreStepPoint()->GetPhysicalVolume()->GetName() == "scatPlanePV" ) {
            G4cout << "Step  ";
            G4cout << "From " << (step->GetPreStepPoint()->GetPhysicalVolume() ? step->GetPreStepPoint()->GetPhysicalVolume()->GetName().c_str() : "----" ) << "  into  " << (step->GetPostStepPoint()->GetPhysicalVolume() ? step->GetPostStepPoint()->GetPhysicalVolume()->GetName().c_str() : "----" ) << G4endl;
            G4cout << "particle          = " << step->GetTrack()->GetParticleDefinition()->GetParticleName() << G4endl;
            G4cout << "total edep   / keV = " << step->GetTotalEnergyDeposit() / keV  << G4endl;
            G4cout << "nonion edep  / keV = " << step->GetNonIonizingEnergyDeposit() / keV << G4endl;
            G4cout << "momentum change / keV = " << (step->GetPostStepPoint()->GetMomentum().mag() - step->GetPreStepPoint()->GetMomentum().mag() ) / keV << G4endl;
            G4cout << "proc defined step  = " << step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() << G4endl;
        }
    }
}

} /* namespace GFTest */
