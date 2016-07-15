
#ifndef GEANTTRACKERSD_H
#define GEANTTRACKERSD_H

#include "G4VSensitiveDetector.hh"
#include <memory>
#include "TH1F.h"
#include "TH2F.h"

namespace GFTest {

class GeantTrackerSD : public G4VSensitiveDetector
{
public:
    GeantTrackerSD( G4int planeId );
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);

    const TH2F * GetPos_cm() const  { return fPos_cm.get(); }
    const TH1F * GetPosX_cm() const { return fPosX_cm.get(); }
    const TH1F * GetPosY_cm() const { return fPosY_cm.get(); }
    const TH1F * GetMomLoss_keV() const { return fMomLoss_keV.get(); }

private:
    std::unique_ptr<TH2F> fPos_cm;
    std::unique_ptr<TH1F> fPosX_cm;
    std::unique_ptr<TH1F> fPosY_cm;
    std::unique_ptr<TH1F> fMomLoss_keV;
};

} /* namespace GFTest */

#endif /* GEANTTRACKERSD_H */
