
#include "GeantTrackerSD.h"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"
#include "G4Step.hh"
#include "TString.h"
#include "Options.h"

namespace GFTest {

GeantTrackerSD::GeantTrackerSD( G4int planeId )
: G4VSensitiveDetector( Form("TrackerSD%d", planeId) )
{
    fPos_cm.reset( new TH2F( Form("Pos_cm_%d", planeId), Form("Pos_cm_%d", planeId), 100, -1, 1, 100, -1, 1 ) );
    fPosX_cm.reset( new TH1F( Form("PosX_cm_%d", planeId), Form("PosX_cm_%d", planeId), 100, -1, 1 ) );
    fPosY_cm.reset( new TH1F( Form("PosY_cm_%d", planeId), Form("PosY_cm_%d", planeId), 100, -1, 1 ) );
    fMomLoss_keV.reset( new TH1F( Form("MomLoss_keV_%d", planeId), Form("MomLoss_keV_%d", planeId), 100, 0, 500 ) );
}



G4bool GeantTrackerSD::ProcessHits( G4Step * aStep, G4TouchableHistory * )
{  
    if( aStep->GetTrack()->GetParentID() == 0 ) { // count only the primary particles
        G4ThreeVector pos = aStep->GetPreStepPoint()->GetPosition();
        G4double momLoss = gOptions->GetPartPMag_GeVc() * GeV - aStep->GetPreStepPoint()->GetMomentum().mag();

        fPos_cm->Fill( pos.x() / cm, pos.y() / cm );
        fPosX_cm->Fill( pos.x() / cm );
        fPosY_cm->Fill( pos.y() / cm );
        fMomLoss_keV->Fill( momLoss / keV );
    }

    return true;
}

} /* namespace GFTest */

