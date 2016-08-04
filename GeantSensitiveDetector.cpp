
#include "GeantSensitiveDetector.h"

#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"
#include "G4Step.hh"
#include "TString.h"
#include "Options.h"
#include "G4VProcess.hh"

namespace GFTest {

GeantSensitiveDetector::GeantSensitiveDetector( G4int id )
: G4VSensitiveDetector( Form("TrackerSD%d", id) )
{
    fHistTx.reset( new TH1F( Form("fHistTx_%d", id), Form("fHistTx_%d", id), 1000, -1, 1 ) );
    fHistTy.reset( new TH1F( Form("fHistTy_%d", id), Form("fHistTy_%d", id), 1000, -1, 1 ) );
    fHistELoss_keV.reset( new TH1F( Form("HistEnergyLoss_keV_%d", id), Form("HistEnergyLoss_keV_%d", id), 50000, 0, 50000 ) );
}



G4bool GeantSensitiveDetector::ProcessHits( G4Step * aStep, G4TouchableHistory * )
{
    // skip secondaries
    if( aStep->GetTrack()->GetParentID() != 0  ) {
        return true;
    }

    // fEventEnergyDeposit += aStep->GetTotalEnergyDeposit();
    // fEventEnergyLoss += aStep->GetPreStepPoint()->GetKineticEnergy() - aStep->GetPostStepPoint()->GetKineticEnergy();

    //    std::cout << "HIT" << std::endl;
    //    std::cout << "ParentID=" << aStep->GetTrack()->GetParentID() << std::endl;
    //    std::cout << "Name=" << aStep->GetTrack()->GetParticleDefinition()->GetParticleName() << std::endl;
    //    std::cout << "Prestep  in " << aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() << std::endl;
    //    std::cout << "Prestep  at " << aStep->GetPreStepPoint()->GetPosition() << std::endl;
    //    std::cout << "Poststep in " << ( aStep->GetPostStepPoint()->GetPhysicalVolume() ? aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() : G4String("---") ) << std::endl;
    //    std::cout << "Poststep at " << aStep->GetPostStepPoint()->GetPosition() << std::endl;
    //    std::cout << "Limit by: " << aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() << std::endl;
    //    std::cout << "Total   : " << aStep->GetTotalEnergyDeposit() << std::endl;

//    if( aStep->GetNonIonizingEnergyDeposit() > 0 ) {
//        std::cout << "FOUND!!!!!" << std::endl;
//
//        std::cout << "HIT" << std::endl;
//        std::cout << "ParentID=" << aStep->GetTrack()->GetParentID() << std::endl;
//        std::cout << "Name=" << aStep->GetTrack()->GetParticleDefinition()->GetParticleName() << std::endl;
//        std::cout << "Prestep  in " << aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() << std::endl;
//        std::cout << "Prestep  at " << aStep->GetPreStepPoint()->GetPosition() << std::endl;
//        std::cout << "Poststep in " << ( aStep->GetPostStepPoint()->GetPhysicalVolume() ? aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() : G4String("---") ) << std::endl;
//        std::cout << "Poststep at " << aStep->GetPostStepPoint()->GetPosition() << std::endl;
//        std::cout << "Limit by: " << aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() << std::endl;
//        std::cout << "Nonion  : " << aStep->GetNonIonizingEnergyDeposit() << std::endl;
//        std::cout << "Total   : " << aStep->GetTotalEnergyDeposit() << std::endl;
//
//    }


    if( aStep->GetPostStepPoint()->GetPhysicalVolume() == nullptr && aStep->GetPostStepPoint()->GetMomentum().z() > 0 ) {

        assert( aStep->GetPostStepPoint()->GetPosition().z() > 0 );

        G4double ELoss = gOptions->GetEnergy_GeV() * GeV - aStep->GetPostStepPoint()->GetKineticEnergy();
        auto dir = aStep->GetPostStepPoint()->GetMomentumDirection();
        fHistELoss_keV->Fill( ELoss / keV );
        fHistTx->Fill( dir.x() / dir.z() );
        fHistTy->Fill( dir.y() / dir.z() );
    }

    return true;
}


} /* namespace GFTest */

