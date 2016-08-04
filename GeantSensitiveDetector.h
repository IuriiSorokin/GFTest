
#ifndef GEANTTRACKERSD_H
#define GEANTTRACKERSD_H

#include "G4VSensitiveDetector.hh"
#include <memory>
#include "TH1F.h"
#include "TH2F.h"

namespace GFTest {

class GeantSensitiveDetector : public G4VSensitiveDetector
{
public:
    GeantSensitiveDetector( G4int id );

    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);

    const TH1F * GetHistTx() const { return fHistTx.get(); }
    const TH1F * GetHistTy() const { return fHistTy.get(); }
    const TH1F * GetHistELoss_keV() const { return fHistELoss_keV.get(); }

private:
    std::unique_ptr<TH1F> fHistTx;
    std::unique_ptr<TH1F> fHistTy;
    std::unique_ptr<TH1F> fHistELoss_keV;
};

} /* namespace GFTest */

#endif /* GEANTTRACKERSD_H */
