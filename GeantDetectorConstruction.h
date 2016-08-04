#ifndef GEANTDETECTORCONSTRUCTION_H
#define GEANTDETECTORCONSTRUCTION_H

#include "G4VUserDetectorConstruction.hh"
#include "GeantSensitiveDetector.h"

namespace GFTest {

class GeantDetectorConstruction : public G4VUserDetectorConstruction
{
public:
    GeantDetectorConstruction() : fSensitiveDetector(nullptr) {};
    virtual ~GeantDetectorConstruction() {};
    virtual G4VPhysicalVolume* Construct();
    GeantSensitiveDetector * GetSensitiveDetector() const { return fSensitiveDetector; }

private:
    GeantSensitiveDetector * fSensitiveDetector;

};

} /* namespace GFTest */

#endif /* GEANTDETECTORCONSTRUCTION_H */
