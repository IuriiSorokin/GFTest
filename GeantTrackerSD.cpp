
#include "GeantTrackerSD.h"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"
#include "G4Step.hh"
#include "TString.h"
#include "Options.h"
#include "G4VProcess.hh"

namespace GFTest {

GeantTrackerSD::GeantTrackerSD( G4int planeId )
: G4VSensitiveDetector( Form("TrackerSD%d", planeId) )
{
    // std::cout << "Creating detector " <<  Form("TrackerSD%d", planeId) << std::endl;
    fPos_cm.reset( new TH2F( Form("Pos_cm_%d", planeId), Form("Pos_cm_%d", planeId), 500, -0.05, 0.05, 500, -0.05, 0.05 ) );
    fPosX_cm.reset( new TH1F( Form("PosX_cm_%d", planeId), Form("PosX_cm_%d", planeId), 500, -0.05, 0.05 ) );
    fPosY_cm.reset( new TH1F( Form("PosY_cm_%d", planeId), Form("PosY_cm_%d", planeId), 500, -0.05, 0.05 ) );
    fMomLoss_keV.reset( new TH1F( Form("MomLoss_keV_%d", planeId), Form("MomLoss_keV_%d", planeId), 1000, 0, 5000 ) );
}



G4bool GeantTrackerSD::ProcessHits( G4Step * aStep, G4TouchableHistory * )
{  
    if( aStep->GetTrack()->GetParentID() == 0  ) { // count only the primary particles

//        if( GetName() == "TrackerSD0" ) {
//        // && aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() == "scatPlanePV"
//        std::cout << "HIT" << std::endl;
//        std::cout << "ParentID=" << aStep->GetTrack()->GetParentID() << std::endl;
//        std::cout << "Name=" << aStep->GetTrack()->GetParticleDefinition()->GetParticleName() << std::endl;
//        std::cout << "Prestep at " << aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() << std::endl;
//        std::cout << "poststep at " << aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() << std::endl;
//        }

        G4ThreeVector pos = aStep->GetPreStepPoint()->GetPosition();
        G4double momLoss = gOptions->GetPartPMag_GeVc() * GeV - aStep->GetPreStepPoint()->GetMomentum().mag();

        fPos_cm->Fill( pos.x() / cm, pos.y() / cm );
        fPosX_cm->Fill( pos.x() / cm );
        fPosY_cm->Fill( pos.y() / cm );
        fMomLoss_keV->Fill( momLoss / keV );
        // std::cout << "Det name " << GetName() << "    Entries = " << fMomLoss_keV->GetEntries() << std::endl;

        if( aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() == "DetPlanePV") {
            aStep->GetTrack()->SetTrackStatus( fStopAndKill );
        }
    }

//    std::cout << "Limit by: " << aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() << std::endl;
//    std::cout << "Nonion  : " << aStep->GetNonIonizingEnergyDeposit() << std::endl;
//    std::cout << "Total   : " << aStep->GetTotalEnergyDeposit() << std::endl;

    return true;
}

} /* namespace GFTest */

