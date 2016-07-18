
#include "GeantSim.h"
#include "G4RunManager.hh"
#include "G4PhysListFactory.hh"
#include "G4StepLimiterPhysics.hh"
#include "GeantDetectorConstruction.h"
#include "GeantDetectorConstruction.h"
#include "GeantPrimaryGeneratorAction.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "Options.h"
#include "TF1.h"
#include <memory>

namespace GFTest {

namespace GeantSim {

GFTestResult Run()
{
    std::unique_ptr<G4RunManager> runManager( new G4RunManager );
    runManager->SetUserInitialization( G4PhysListFactory().GetReferencePhysList( gOptions->GetGeantPhys() ) );
    runManager->SetUserInitialization( new GeantDetectorConstruction() );
    runManager->SetUserAction( new GeantPrimaryGeneratorAction );

    runManager->Initialize();
    runManager->BeamOn( gOptions->GetGeantNPart() ); // events

    // ===== Prepare the result =====

    GFTestResult result( std::string("GEANT(") + gOptions->GetGeantPhys() + ")" );

    const GeantTrackerSD * det = dynamic_cast<const GeantDetectorConstruction * >( runManager->GetUserDetectorConstruction())->GetDetPlaneSD();

    std::unique_ptr<TH2F> pos_cm( dynamic_cast<TH2F*>(det->GetPos_cm()->Clone("pos_cm")) );
    std::unique_ptr<TH1F> posX_cm( dynamic_cast<TH1F*>(det->GetPosX_cm()->Clone("posX_cm")) ); // Need to clone it to be able to fit
    std::unique_ptr<TH1F> posY_cm( dynamic_cast<TH1F*>(det->GetPosY_cm()->Clone("posY_cm")) );
    std::unique_ptr<TH1F> momLoss_keV( dynamic_cast<TH1F*>(det->GetMomLoss_keV()->Clone("momLoss_keV")) );

    pos_cm->SetDirectory(0); // take the ownership away from ROOT
    posX_cm->SetDirectory(0);
    posY_cm->SetDirectory(0);
    momLoss_keV->SetDirectory(0);

    result.XStddev = posX_cm->GetRMS();
    result.YStddev = posY_cm->GetRMS();
    result.XYCov   = pos_cm->GetCovariance();

    posX_cm->Fit("gaus");
    posY_cm->Fit("gaus");

    result.XSigmaFit  = dynamic_cast<const TF1*>(posX_cm->GetListOfFunctions()->FindObject("gaus"))->GetParameter("Sigma");
    result.YSigmaFit  = dynamic_cast<const TF1*>(posY_cm->GetListOfFunctions()->FindObject("gaus"))->GetParameter("Sigma");

    result.PLossMean   = momLoss_keV->GetMean();
    result.PLossStddev = momLoss_keV->GetRMS();
    result.Passed = momLoss_keV->GetEntries() / gOptions->GetGeantNPart();

    // ===== Store =====

    TFile outFile("geant_sim.root", "recreate");
    pos_cm->Write();
    posX_cm->Write();
    posY_cm->Write();
    momLoss_keV->Write();
    outFile.Close();

    // ===== Draw =====

    if( GFTest::gOptions->GetGeantDraw() ) {
        new TCanvas();
        pos_cm->SetDirectory(gROOT); // give the ownership back to ROOT
        pos_cm->Draw("colz");
        pos_cm.release();
        new TCanvas();
        posX_cm->SetDirectory(gROOT);
        posX_cm->Draw();
        posX_cm.release();
        new TCanvas();
        posY_cm->SetDirectory(gROOT);
        posY_cm->Draw();
        posY_cm.release();
        new TCanvas();
        momLoss_keV->SetDirectory(gROOT);
        momLoss_keV->Draw();
        momLoss_keV.release();
    }

    return result;
}

} /* namespace GFTest::GeantSim */

} /* namespace GFTest */

