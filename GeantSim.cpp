//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file electromagnetic/TestEm18/TestEm18.cc
/// \brief Main program of the electromagnetic/TestEm18 example
//
//
// $Id: TestEm18.cc 66241 2012-12-13 18:34:42Z gunter $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "GeantSim.h"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "Randomize.hh"

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "SteppingVerbose.hh"
#include "StackingAction.hh"
#include "G4Run.hh"
#include "Options.h"
#include "G4SystemOfUnits.hh"
// #include "HistoManager.hh"

// #include <boost/algorithm/string/predicate.hpp>


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

namespace GFTest {

namespace GeantSim {

GFTestResult Run()
{
    //choose the Random engine
    CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);

    //my Verbose output class
    G4VSteppingVerbose::SetInstance(new SteppingVerbose);

    // Construct the default run manager
    G4RunManager * runManager = new G4RunManager;

    // set mandatory initialization classes
    DetectorConstruction* detector;
    detector = new DetectorConstruction;
    runManager->SetUserInitialization(detector);
    runManager->SetUserInitialization(new PhysicsList());

    // set user action classes
    //
    //primaryGenerator
    PrimaryGeneratorAction* primary = new PrimaryGeneratorAction(detector);
    runManager->SetUserAction(primary);

    //runAction
    RunAction* runaction = new RunAction(detector,primary);
    runManager->SetUserAction(runaction);

    //eventAction
    EventAction* eventaction = new EventAction(runaction);
    runManager->SetUserAction(eventaction);

    //stepAction
    SteppingAction* steppingaction = new SteppingAction(runaction, eventaction);
    runManager->SetUserAction(steppingaction);

    //stackAction
    StackingAction* stackingaction = new StackingAction(runaction, eventaction);
    runManager->SetUserAction(stackingaction);

    // get the pointer to the User Interface manager
    G4UImanager* UI = G4UImanager::GetUIpointer();

    UI->ApplyCommand( G4String("/testem/det/setMat G4_") + gOptions->GetMaterial() );
    UI->ApplyCommand( G4String("/testem/det/setSize ") + std::to_string( gOptions->GetThickness_um() ) + " um" );
    UI->ApplyCommand( G4String("/testem/phys/addPhysics standard") );
    UI->ApplyCommand( G4String("/run/setCut 1 um") );
    UI->ApplyCommand( G4String("/run/initialize") );
    UI->ApplyCommand( G4String("/gun/particle ") + gOptions->GetParticle() );
    UI->ApplyCommand( G4String("/gun/energy ") + std::to_string( gOptions->GetEnergy_GeV() ) + " GeV" );
    UI->ApplyCommand( G4String("/analysis/setFileName ") + gOptions->GetGeantOutFile() );
    UI->ApplyCommand( G4String("/analysis/h1/set 1 1000 0 1 MeV") );   // continuous eLoss
    UI->ApplyCommand( G4String("/analysis/h1/set 2 1000 0 1 MeV") );   // secondary  eLoss
    UI->ApplyCommand( G4String("/analysis/h1/set 3 1000 0 1 MeV") );   // total eLoss
    UI->ApplyCommand( G4String("/analysis/h1/set 4 1000 0 1 MeV") );   // e- energy spectrum
    UI->ApplyCommand( G4String("/analysis/h1/set 5 1000 0 1 MeV") );   // gamma energy spectrum
    UI->ApplyCommand( G4String("/analysis/h1/set 6 1000 0 500 um") );  // step size
    UI->ApplyCommand( G4String("/run/printProgress 100000") );
    UI->ApplyCommand( G4String("/run/beamOn " + std::to_string( gOptions->GetGeantNEvents() ) ) );

    GFTestResult result;

    double nEvents = G4RunManager::GetRunManager()->GetCurrentRun()->GetNumberOfEvent();
    result.ELossMean   = runaction->GetTotalLoss() / keV / nEvents;
    result.ELossStddev = sqrt( runaction->GetTotalLoss2() / pow(keV,2) / nEvents - pow( result.ELossMean.get(), 2 ) );

    // G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    // TH1D * hTotalELoss = dynamic_cast<TH1D *>(analysisManager->GetH1( 3 ));

    // std::cout << "HISTOGRAM 3 " << analysisManager->GetH1( 3 )->mean() << " RMS: " << analysisManager->GetH1( 3 )->rms() << std::endl;

    // job termination
    //
    delete runManager;


    return result;
}

} /* namespace GFTest::GeantSim */

} /* namespace GFTest */

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......





























//
//#include "GeantSim.h"
//#include "G4RunManager.hh"
//#include "G4PhysListFactory.hh"
//#include "G4StepLimiterPhysics.hh"
//#include "GeantDetectorConstruction.h"
//#include "GeantPrimaryGeneratorAction.h"
//#include "TFile.h"
//#include "TCanvas.h"
//#include "TROOT.h"
//
//#include "TF1.h"
//#include "TMath.h"
//#include <memory>
//#include "G4VModularPhysicsList.hh"
//#include "G4EmStandardPhysics_option4.hh"
////#include "FTFP_BERT.hh"
//#include "G4PhysListFactory.hh"
//#include "G4SystemOfUnits.hh"
//// #include "G4StepLimiterPhysics.hh"
//
//namespace GFTest {
//
//namespace GeantSim {
//
//GFTestResult Run()
//{
//    TH1::StatOverflows( kTRUE ); // Use the counts falling into underflow/overflow to calculate the mean
//
//    std::unique_ptr<G4RunManager> runManager( new G4RunManager );
//
//    // runManager->SetUserInitialization( G4PhysListFactory().GetReferencePhysList( "FTFP_BERT_EMZ" ) );
//
//    runManager->SetUserInitialization( G4PhysListFactory().GetReferencePhysList( "QGSP_BERT_EMZ" ) );
//
//    //    G4VModularPhysicsList * physicsList = new G4VModularPhysicsList();
//    //    physicsList->RegisterPhysics( new G4EmStandardPhysics_option4() );
//    //    // physicsList->RegisterPhysics( new G4StepLimiterPhysics() );
//    //    runManager->SetUserInitialization( physicsList );
//
//    runManager->SetUserInitialization( new GeantDetectorConstruction() );
//    runManager->SetUserAction( new GeantPrimaryGeneratorAction() );
//    // G4EventManager::GetEventManager()->SetVerboseLevel(1);
//    runManager->Initialize();
//    runManager->BeamOn( gOptions->GetGeantNEvents() );
//
//    // ===== Prepare the result =====
//    GFTestResult result;
//
//    const GeantSensitiveDetector * det = dynamic_cast<const GeantDetectorConstruction * >( runManager->GetUserDetectorConstruction())->GetSensitiveDetector();
//
//    TH1F * hELoss_keV = dynamic_cast<TH1F*>( det->GetHistELoss_keV()->Clone("hELoss_keV") );
//    TH1F * hTx    = dynamic_cast<TH1F*>( det->GetHistTx()->Clone("hTx") );
//    TH1F * hTy    = dynamic_cast<TH1F*>( det->GetHistTy()->Clone("hTy") );
//
//    result.Passed      = hTx->GetEntries()  / gOptions->GetGeantNEvents();
//    result.ELossMean   = hELoss_keV->GetMean();
//    result.ELossStddev = hELoss_keV->GetRMS();
//    result.TxStddev    = hTx->GetRMS();
//    result.TyStddev    = hTy->GetRMS();
//
//    hELoss_keV->Fit("gaus", "q");
//    result.ELossGausMean  = dynamic_cast<TF1*>(hELoss_keV->GetListOfFunctions()->FindObject("gaus"))->GetParameter("Mean");
//    result.ELossGausSigma = dynamic_cast<TF1*>(hELoss_keV->GetListOfFunctions()->FindObject("gaus"))->GetParameter("Sigma");
//
//    hTx->Fit("gaus", "q");
//    result.TxGausSigma = dynamic_cast<TF1*>(hTx->GetListOfFunctions()->FindObject("gaus"))->GetParameter("Sigma");
//
//    hTy->Fit("gaus", "q");
//    result.TxGausSigma = dynamic_cast<TF1*>(hTy->GetListOfFunctions()->FindObject("gaus"))->GetParameter("Sigma");
//
//    // ===== Store =====
//
//    TFile outFile( gOptions->GetGeantOutFile().c_str(), "recreate");
//    hELoss_keV->Write();
//    hTx->Write();
//    hTy->Write();
//    outFile.Close();
//
//    // ===== Draw =====
//
//    if( GFTest::gOptions->GetGeantDraw() ) {
//        new TCanvas("ELoss");
//        hELoss_keV->Clone("Eloss")->Draw();
//        new TCanvas("Tx");
//        hTx->Clone("Tx")->Draw();
//        new TCanvas("Ty");
//        hTy->Clone("Ty")->Draw();
//    }
//
//    return result;
//}
//
//
//
//} /* namespace GFTest::GeantSim */
//
//} /* namespace GFTest */

