/*
 * GFTestResult.h
 *
 *  Created on: Jul 17, 2016
 *      Author: sorokin
 */

#ifndef GFTESTRESULT_H_
#define GFTESTRESULT_H_

#include <string>
#include <boost/optional.hpp>
#include <boost/format.hpp>
#include <vector>

namespace GFTest {

struct GFTestResult
{
    boost::optional<double> Passed;      // Fraction of particles that passed the absorber and were detected by the detector plane; For Genfit it's either 0 or 1;
    boost::optional<double> ELossMean;
    boost::optional<double> ELossStddev;
    boost::optional<double> ELossGausMean;
    boost::optional<double> ELossGausSigma;
    boost::optional<double> TxStddev;
    boost::optional<double> TxGausSigma;
    boost::optional<double> TyStddev;
    boost::optional<double> TyGausSigma;
    boost::optional<double> TailCutoff;

    static void Compare( std::vector<GFTestResult> results, std::ostream & os = std::cout  );

    template<typename T>
    static void StreamOptional(const boost::optional<T> value, boost::format f, std::ostream & os = std::cout );
};

} /* namespace GFTest */

#endif /* GFTESTRESULT_H_ */
