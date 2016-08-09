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
#include <algorithm>
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

    std::vector<G4double> & lossVect = runaction->TotalLossVect();
    std::sort( lossVect.begin(), lossVect.end() );

    if( gOptions->GetParticle() != "e+" && gOptions->GetParticle() != "e-") {
        while( lossVect.size() > static_cast<size_t>( gOptions->GetTailCutoff() * gOptions->GetGeantNEvents() ) ) {
            lossVect.pop_back();
        }
    }

    double sum = 0;
    double sum2 = 0;
    double num = lossVect.size();
    for( const auto & eloss : lossVect ) {
        sum += eloss;
        sum2 += eloss * eloss;
    }
    // std::cout << "truncated eloss stddev = " << sqrt( sum2 / num / pow(keV,2) - pow(sum / num / keV, 2) ) << std::endl;


    GFTestResult result;

    double nEvents = G4RunManager::GetRunManager()->GetCurrentRun()->GetNumberOfEvent();
    result.ELossMean   = runaction->GetTotalLoss() / keV / nEvents;
    // result.ELossStddev = sqrt( runaction->GetTotalLoss2() / pow(keV,2) / nEvents - pow( result.ELossMean.get(), 2 ) );
    result.ELossStddev = sqrt( sum2 / num / pow(keV,2) - pow(sum / num / keV, 2) );

    delete runManager;

    return result;
}

} /* namespace GFTest::GeantSim */

} /* namespace GFTest */













