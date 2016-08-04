
#include "GeantDetectorConstruction.h"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "Options.h"
#include "G4UserLimits.hh"

namespace GFTest {

G4VPhysicalVolume * GeantDetectorConstruction::Construct()
{
    G4double            halfThickness = gOptions->GetThickness_um() * um / 2;
    G4double            halfSize = 100 * halfThickness;
    G4Box *             solid = new G4Box( "WorldS", halfSize, halfSize, halfThickness );
    G4Material *        material =  G4NistManager::Instance()->FindOrBuildMaterial(Form("G4_%s", gOptions->GetMaterial().c_str()));
    G4LogicalVolume *   logicalVolume = new G4LogicalVolume( solid, material, "World");
    // logicalVolume->SetUserLimits( new G4UserLimits( halfThickness / 100 ) );
    G4VPhysicalVolume * physicalVolume = new G4PVPlacement( 0, G4ThreeVector(0,0,0), logicalVolume, "WorldPV", 0, false, 0, true );
    fSensitiveDetector = new GeantSensitiveDetector( 0 );
    SetSensitiveDetector( logicalVolume, fSensitiveDetector );

    G4cout << "World material: " << material->GetName() << " Density = " << material->GetDensity() / ( g / cm3 ) << " g/cm3  Radiation length = "<< material->GetRadlen() / cm << " cm"<< G4endl;
    return physicalVolume;
}

} /* namespace GFTest */
