
#include "GeantDetectorConstruction.h"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "Options.h"

namespace GFTest {

G4VPhysicalVolume * GeantDetectorConstruction::Construct()
{
    // World
    G4double worldSize = 5 * ( std::abs( gOptions->GetDetZ_cm() * cm ) + std::abs( gOptions->GetPartZ_cm() * cm ) ) ;
    G4Box * worldSolid = new G4Box("WorldS", worldSize, worldSize, worldSize);
    G4LogicalVolume * worldLV = new G4LogicalVolume( worldSolid, G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"), "World");
    G4VPhysicalVolume * worldPV = new G4PVPlacement( 0, G4ThreeVector(0,0,0), worldLV, "WorldPV", 0, false, 0, true);

    // Scatterer plane
    G4Box * scatPlaneSolid = new G4Box("scatPlaneSolid", worldSize, worldSize, gOptions->GetScatThick_um() * um / 2  );
    G4Material * scatMat =  G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");
    G4cout << "Scatterer material: " << scatMat->GetName() << " Radiation length = "<< scatMat->GetRadlen() / cm << " cm"<< G4endl;
    G4LogicalVolume * scatPlaneLV = new G4LogicalVolume( scatPlaneSolid, scatMat, "scatPlaneLV");
    new G4PVPlacement( 0, G4ThreeVector(0,0,0), scatPlaneLV, "scatPlanePV", worldLV, false, 0, true);

    // Detector plane
    G4Box * detPlaneSolid = new G4Box("detPlaneSolid", worldSize, worldSize, 1 * um );
    G4LogicalVolume * detPlaneLV = new G4LogicalVolume( detPlaneSolid, G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"), "detPlaneLV");
    new G4PVPlacement( 0, G4ThreeVector( 0, 0, gOptions->GetDetZ_cm() + detPlaneSolid->GetZHalfLength() ), detPlaneLV, "DetPlanePV", worldLV, false, 0, true);
    fDetPlaneSD = new GeantTrackerSD( 0 );
    SetSensitiveDetector( detPlaneLV, fDetPlaneSD );

    return worldPV;
}

} /* namespace GFTest */
