
#include "GeantSim.h"
#include "G4RunManager.hh"
#include "G4PhysListFactory.hh"
#include "G4StepLimiterPhysics.hh"
#include "GeantDetectorConstruction.h"
#include "GeantPrimaryGeneratorAction.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "Options.h"
#include "TF1.h"
#include "TMath.h"
#include <memory>
#include "G4VModularPhysicsList.hh"
#include "G4EmStandardPhysics_option4.hh"
//#include "FTFP_BERT.hh"
#include "G4PhysListFactory.hh"
#include "G4SystemOfUnits.hh"
// #include "G4StepLimiterPhysics.hh"

namespace GFTest {

namespace GeantSim {

GFTestResult Run()
{
    TH1::StatOverflows( kTRUE ); // Use the counts falling into underflow/overflow to calculate the mean

    std::unique_ptr<G4RunManager> runManager( new G4RunManager );

    // runManager->SetUserInitialization( G4PhysListFactory().GetReferencePhysList( "FTFP_BERT_EMZ" ) );

    runManager->SetUserInitialization( G4PhysListFactory().GetReferencePhysList( "QGSP_BERT_EMZ" ) );

//    G4VModularPhysicsList * physicsList = new G4VModularPhysicsList();
//    physicsList->RegisterPhysics( new G4EmStandardPhysics_option4() );
//    // physicsList->RegisterPhysics( new G4StepLimiterPhysics() );
//    runManager->SetUserInitialization( physicsList );

    runManager->SetUserInitialization( new GeantDetectorConstruction() );
    runManager->SetUserAction( new GeantPrimaryGeneratorAction() );
    // G4EventManager::GetEventManager()->SetVerboseLevel(1);
    runManager->Initialize();
    runManager->BeamOn( gOptions->GetGeantNEvents() );

    // ===== Prepare the result =====
    GFTestResult result;

    const GeantSensitiveDetector * det = dynamic_cast<const GeantDetectorConstruction * >( runManager->GetUserDetectorConstruction())->GetSensitiveDetector();

    TH1F * hELoss_keV = dynamic_cast<TH1F*>( det->GetHistELoss_keV()->Clone("hELoss_keV") );
    TH1F * hTx    = dynamic_cast<TH1F*>( det->GetHistTx()->Clone("hTx") );
    TH1F * hTy    = dynamic_cast<TH1F*>( det->GetHistTy()->Clone("hTy") );

    result.Passed      = hTx->GetEntries()  / gOptions->GetGeantNEvents();
    result.ELossMean   = hELoss_keV->GetMean();
    result.ELossStddev = hELoss_keV->GetRMS();
    result.TxStddev    = hTx->GetRMS();
    result.TyStddev    = hTy->GetRMS();

    hELoss_keV->Fit("gaus", "q");
    result.ELossGausMean  = dynamic_cast<TF1*>(hELoss_keV->GetListOfFunctions()->FindObject("gaus"))->GetParameter("Mean");
    result.ELossGausSigma = dynamic_cast<TF1*>(hELoss_keV->GetListOfFunctions()->FindObject("gaus"))->GetParameter("Sigma");

    hTx->Fit("gaus", "q");
    result.TxGausSigma = dynamic_cast<TF1*>(hTx->GetListOfFunctions()->FindObject("gaus"))->GetParameter("Sigma");

    hTy->Fit("gaus", "q");
    result.TxGausSigma = dynamic_cast<TF1*>(hTy->GetListOfFunctions()->FindObject("gaus"))->GetParameter("Sigma");

    // ===== Store =====

    TFile outFile( gOptions->GetGeantOutFile().c_str(), "recreate");
    hELoss_keV->Write();
    hTx->Write();
    hTy->Write();
    outFile.Close();

    // ===== Draw =====

    if( GFTest::gOptions->GetGeantDraw() ) {
        new TCanvas("ELoss");
        hELoss_keV->Clone("Eloss")->Draw();
        new TCanvas("Tx");
        hTx->Clone("Tx")->Draw();
        new TCanvas("Ty");
        hTy->Clone("Ty")->Draw();
    }

    return result;
}



} /* namespace GFTest::GeantSim */

} /* namespace GFTest */

