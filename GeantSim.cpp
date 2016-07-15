
#include "GeantSim.h"
#include "G4RunManager.hh"
#include "FTFP_BERT.hh"
#include "G4StepLimiterPhysics.hh"
#include "GeantDetectorConstruction.h"
#include "GeantDetectorConstruction.h"
#include "GeantPrimaryGeneratorAction.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "Options.h"

namespace GFTest {

namespace GeantSim {

void Run()
{
    std::unique_ptr<G4RunManager> runManager( new G4RunManager );

    G4VModularPhysicsList* physicsList = new FTFP_BERT; // PhysicsList !!!
    physicsList->RegisterPhysics(new G4StepLimiterPhysics());
    runManager->SetUserInitialization(physicsList);

    runManager->SetUserInitialization( new GeantDetectorConstruction() );
    runManager->SetUserAction( new GeantPrimaryGeneratorAction );

    runManager->Initialize();
    runManager->BeamOn( gOptions->GetGeantNPart() ); // events

    // ===== print, draw and store =====

    TFile outFile("geant_sim.root", "recreate");

    const GeantTrackerSD * det = dynamic_cast<const GeantDetectorConstruction * >( runManager->GetUserDetectorConstruction())->GetDetPlaneSD();

    TH2F * pos_cm = dynamic_cast<TH2F*>(det->GetPos_cm()->Clone("pos_cm"));
    pos_cm->SetDirectory(gROOT);
    pos_cm->Write();

    TH1F * posX_cm = dynamic_cast<TH1F*>(det->GetPosX_cm()->Clone("posX_cm"));
    posX_cm->SetDirectory(gROOT);
    posX_cm->Fit("gaus");
    posX_cm->Write();

    TH1F * posY_cm = dynamic_cast<TH1F*>(det->GetPosY_cm()->Clone("posY_cm"));
    posY_cm->SetDirectory(gROOT);
    posY_cm->Fit("gaus");
    posY_cm->Write();

    TH1F * momLoss_keV = dynamic_cast<TH1F*>(det->GetMomLoss_keV()->Clone("momLoss_keV"));
    momLoss_keV->SetDirectory(gROOT);
    momLoss_keV->Fit("gaus");
    momLoss_keV->Write();
    std::cout << "GEANT mean mom  loss, keV        : " << momLoss_keV->GetMean() << std::endl;
    std::cout << "GEANT RMS momentum loss, keV     : " << momLoss_keV->GetRMS() << std::endl;
    outFile.Close();

    if( GFTest::gOptions->GetGeantDraw() ) {
        new TCanvas();
        pos_cm->Draw("colz");
        new TCanvas();
        posX_cm->Draw();
        new TCanvas();
        posY_cm->Draw();
        new TCanvas();
        momLoss_keV->Draw();
    }


}

} /* namespace GFTest::GeantSim */

} /* namespace GFTest */

