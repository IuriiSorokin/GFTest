
#include "GenfitExt.h"
#include <assert.h>
#include <iostream>
#include <iomanip>
#include "TGeoManager.h"
#include "TGeoElement.h"
#include "TList.h"
#include "TVector3.h"
#include "TVectorD.h"
#include "FieldManager.h"
#include "ConstField.h"
#include "SharedPlanePtr.h"
#include "TDatabasePDG.h"
#include "RKTrackRep.h"
#include "MeasuredStateOnPlane.h"
#include "Exception.h"
#include "MaterialEffects.h"
#include "TGeoMaterialInterface.h"
#include "Options.h"
#include "G4NistManager.hh"
#include "G4Material.hh"
#include <CLHEP/Units/SystemOfUnits.h>
#include "G4String.hh"


namespace GFTest {

namespace GenfitExt
{

GFTestResult Run()
{
    // ============================================
     //                  Geometry
     // ============================================

     TGeoManager::SetVerboseLevel(-1);
     std::unique_ptr<TGeoManager> geoManager( new TGeoManager("genfitGeom", "GENFIT geometry") );

     // ===== Materials =====

     genfit::MaterialEffects::getInstance()->init(new genfit::TGeoMaterialInterface());
     // genfit::MaterialEffects::getInstance()->setEnergyLossBetheBloch(gOptions->GetIon());
     // genfit::MaterialEffects::getInstance()->setEnergyLossBrems(gOptions->GetRad());

     Double_t mediumParameters[10];
     mediumParameters[0]=0.;//sensitive volume flag
     mediumParameters[1]=1.;//magnetic field flag
     mediumParameters[2]=30.;//max fiel in kGauss
     mediumParameters[3]=0.1;//maximal angular dev. due to field
     mediumParameters[4]=0.01;//max step allowed (in cm)
     mediumParameters[5]=1.e-5;//max fractional energy loss
     mediumParameters[6]=1.e-3;//boundary crossing accuracy
     mediumParameters[7]=1.e-5;//minimum step
     mediumParameters[8]=0.;//not defined
     mediumParameters[9]=0.;//not defined

     G4Material * g4material = G4NistManager::Instance()->FindOrBuildMaterial( Form("G4_%s", gOptions->GetMaterial().c_str())  );
     TGeoMixture * gfMaterial = new TGeoMixture("Material", g4material->GetNumberOfElements(), g4material->GetDensity() / ( CLHEP::g / CLHEP::cm3) );
     for( size_t iElement = 0; iElement < g4material->GetNumberOfElements(); ++iElement ) {
         gfMaterial->AddElement( g4material->GetElement(iElement)->GetAtomicMassAmu(), g4material->GetElement(iElement)->GetZ(), g4material->GetFractionVector()[iElement] );
     }
     gfMaterial->SetRadLen( 7777 ); // this should force ROOT to recalculate the radiation and interaction length. 7777 is an arbitrary positive number.
     std::cout << "Scatterer material radiation length = " << gfMaterial->GetRadLen() << std::endl;
     TGeoMedium * medium = new TGeoMedium( "scattererMedium", gGeoManager->GetListOfMedia()->GetSize(), gfMaterial, mediumParameters);

     double halfThickness = gOptions->GetThickness_um() * 1e-4 / 2;
     double halfSize = 100 * halfThickness;
     TGeoVolume * topVolume = gGeoManager->MakeBox("TopVolume", medium, halfSize, halfSize, halfThickness );
     gGeoManager->SetTopVolume(topVolume);
     gGeoManager->CloseGeometry();

     genfit::FieldManager::getInstance()->init( new genfit::ConstField(0,0,0) );

     genfit::SharedPlanePtr detectingPlane( new genfit::DetPlane( TVector3(0,0,halfThickness), TVector3(1,0,0), TVector3(0,1,0) ) );

     // ============================================
     //               Extrapolation
     // ============================================

     const Int_t kElectronPdgCode = TDatabasePDG::Instance()->GetParticle( gOptions->GetParticle().c_str() )->PdgCode();

     genfit::RKTrackRep trackRep(kElectronPdgCode);
     genfit::MeasuredStateOnPlane state( & trackRep );
     double initMom = sqrt( gOptions->GetEnergy_GeV() * ( gOptions->GetEnergy_GeV() + 2 * state.getMass() ) );
     state.setPosMom( TVector3(0,0,-halfThickness), TVector3( 0,0, initMom ) );

     try {
         state.extrapolateToPlane( detectingPlane, false, true );
     }
     catch (genfit::Exception & e) {
         std::cout << "Extrapolation failed. Genfit exception:" << std::endl;
         e.info();
     }

     // ============================================
     //        Prepare and return the result
     // ============================================

     // GFTestResult result("GENFIT");
     GFTestResult result;

     double mom = state.getMom().Mag();
     double mass = state.getMass();
     double EKinInit = gOptions->GetEnergy_GeV();
     double ETot = hypot(mom,mass);
     double EKin = ETot - mass;
     double ELoss = EKinInit - EKin;
     double ELossError = mom / ETot * sqrt( state.getMomVar() );

     result.Passed = (state.getPos().Z() == detectingPlane->getO().Z());
     result.ELossMean =     ELoss * 1e6;
     result.ELossGausMean = ELoss * 1e6;
     result.ELossStddev =    ELossError * 1e6;
     result.ELossGausSigma = ELossError * 1e6;
     result.TxStddev    = sqrt( state.getCov()(1,1) );
     result.TxGausSigma = sqrt( state.getCov()(1,1) );
     result.TyStddev    = sqrt( state.getCov()(2,2) );
     result.TyGausSigma = sqrt( state.getCov()(2,2) );

     return result;
}

} /* namespace GFTest::GenfitExt */

} /* namespace GFTest */

