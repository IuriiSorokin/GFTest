#ifndef GEANTDETECTORCONSTRUCTION_H
#define GEANTDETECTORCONSTRUCTION_H

#include "G4VUserDetectorConstruction.hh"
#include "GeantTrackerSD.h"

namespace GFTest {

class GeantDetectorConstruction : public G4VUserDetectorConstruction
{
public:
    GeantDetectorConstruction() : fDetPlaneSD(nullptr) {};
    virtual ~GeantDetectorConstruction() {};
    virtual G4VPhysicalVolume* Construct();
    GeantTrackerSD * GetDetPlaneSD() const { return fDetPlaneSD; }

private:
    GeantTrackerSD * fDetPlaneSD;

};

} /* namespace GFTest */

#endif /* GEANTDETECTORCONSTRUCTION_H */
