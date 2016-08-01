
#ifndef GEANTSIM_H_
#define GEANTSIM_H_

#include "GFTestResult.h"
#include "TH1F.h"

namespace GFTest {

namespace GeantSim
{
    GFTestResult Run();

    Double_t GetMedian(const TH1F * h, Double_t nParticles );

} /* namespace GFTest::GeantSim */

} /* namespace GFTest */

#endif /* GEANTSIM_H_ */
