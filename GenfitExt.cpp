
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

     genfit::MaterialEffects::getInstance()->init(new genfit::TGeoMaterialInterface());

     // ===== Materials =====

     TGeoElement * siliconElement = gGeoManager->GetElementTable()->FindElement("Silicon");
     TGeoElement * vacuumElement = gGeoManager->GetElementTable()->FindElement("Vacuum");
     assert(siliconElement);
     assert(vacuumElement);

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

     Double_t siliconDensity_g_cm3 = 2.3290;
     TGeoMaterial * siliconMaterial = new TGeoMaterial("siliconMaterial", siliconElement, siliconDensity_g_cm3);
     siliconMaterial->SetRadLen( 7777 ); // this should force ROOT to recalculate the radiation and interaction length. 7777 is an arbitrary positive number.
     std::cout << "Silicon radiation length = " << siliconMaterial->GetRadLen() << std::endl;
     TGeoMedium * siliconMedium = new TGeoMedium("siliconMedium", gGeoManager->GetListOfMedia()->GetSize(), siliconMaterial, mediumParameters);

     Double_t vacuumDensity_g_cm3 = 1e-27;
     TGeoMaterial * vacuumMaterial = new TGeoMaterial("vacuumMaterial", vacuumElement, vacuumDensity_g_cm3);
     vacuumMaterial->SetRadLen( 7777 ); // this should force ROOT to recalculate the radiation and interaction length. 7777 is an arbitrary positive number.
     std::cout << "Vacuum radiation length = " << vacuumMaterial->GetRadLen() << std::endl;
     TGeoMedium * vacuumMedium = new TGeoMedium("vacuumMedium", gGeoManager->GetListOfMedia()->GetSize(), vacuumMaterial, mediumParameters);

     // ===== Physical volumes =====
     double worldSize = 5 * ( std::abs( gOptions->GetDetZ_cm() ) + std::abs( gOptions->GetPartZ_cm() ) ) ;
     TGeoVolume * topVolume = gGeoManager->MakeBox("TopVolume", vacuumMedium, worldSize, worldSize, worldSize);
     gGeoManager->SetTopVolume(topVolume);

     TGeoVolume * siliconPlane = gGeoManager->MakeBox("SiliconPlane", siliconMedium, worldSize, worldSize, gOptions->GetScatThick_um() / 2 * 1e-4 );
     topVolume->AddNode(siliconPlane, 0, new TGeoTranslation(0, 0, 0));

     gGeoManager->CloseGeometry();

     // ===== Magnetic field =====

     genfit::FieldManager::getInstance()->init( new genfit::ConstField(0,0,0) );

     // ===== Detector plane =====

     genfit::SharedPlanePtr plane( new genfit::DetPlane( TVector3(0,0,gOptions->GetDetZ_cm()), TVector3(1,0,0), TVector3(0,1,0) ) );

     // ============================================
     //               Extrapolation
     // ============================================

     const Int_t kElectronPdgCode = TDatabasePDG::Instance()->GetParticle( gOptions->GetPartType().c_str() )->PdgCode();

     genfit::RKTrackRep trackRep(kElectronPdgCode);
     genfit::MeasuredStateOnPlane state( & trackRep );
     state.setPosMom( TVector3(gOptions->GetPartX_cm(),gOptions->GetPartY_cm(),gOptions->GetPartZ_cm()), TVector3(gOptions->GetPartPx_GeVc(),gOptions->GetPartPy_GeVc(),gOptions->GetPartPz_GeVc()) );

     try {
         state.extrapolateToPlane( plane, false, true );
     }
     catch (genfit::Exception & e) {
         std::cout << "Extrapolation failed. Genfit exception:" << std::endl;
         e.info();
     }

     // ============================================
     //        Prepare and return the result
     // ============================================

     GFTestResult result("GENFIT");
     result.Passed = (state.getPos().Z() == plane->getO().Z());
     result.XYCov  = state.get6DCov()(0,1);
     result.XStddev = sqrt( state.get6DCov()(0,0) );
     result.YStddev = sqrt( state.get6DCov()(1,1) );
     result.PLossMean  = ( gOptions->GetPartPMag_GeVc() - state.getMom().Mag() ) * 1e6;
     result.PLossStddev = sqrt(state.getMomVar()) * 1e6;

     return result;
}

} /* namespace GFTest::GenfitExt */

} /* namespace GFTest */

