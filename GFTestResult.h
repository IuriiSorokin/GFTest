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
    std::string Name; // GEANT or GENFIT
    boost::optional<double> Passed;      // Fraction of particles that passed the absorber and were detected by the detector plane; For Genfit it's either 0 or 1;
    boost::optional<double> XStddev;     // Standard deviation
    boost::optional<double> XSigmaFit;   // Sigma from a gaussian fit
    boost::optional<double> YStddev;
    boost::optional<double> YSigmaFit;
    boost::optional<double> XYCov;       // Covariance (not a sqrt of it); In case of GEANT it is taken from unfitted data
    boost::optional<double> PLossMean;
    boost::optional<double> PLossMP;     // Most probable
    boost::optional<double> PLossMedi;   // Median
    boost::optional<double> PLossStddev;

    GFTestResult( std::string name ): Name(name) {};

    static void Compare( std::vector<GFTestResult> results, std::ostream & os = std::cout  );

    template<typename T>
    static void StreamOptional(const boost::optional<T> value, boost::format f, std::ostream & os = std::cout );
};

} /* namespace GFTest */

#endif /* GFTESTRESULT_H_ */
