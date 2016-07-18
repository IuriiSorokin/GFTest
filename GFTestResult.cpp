/*
 * GFTestResult.cpp
 *
 *  Created on: Jul 17, 2016
 *      Author: sorokin
 */

#include "GFTestResult.h"

namespace GFTest {



template<typename T>
void GFTestResult::StreamOptional(const boost::optional<T> value, boost::format f, std::ostream & os)
{
    os << (value.is_initialized() ? ( f % value.get() ) : ( f % "--" ) );
}



void GFTestResult::Compare( std::vector<GFTestResult> results, std::ostream & os )
{
    boost::format f("%|16g|");

    os << "                   ";
    for( const GFTestResult & result : results ) os << f % result.Name;
    os << "\n";

    os << "Passed             ";
    for( const GFTestResult & result : results ) StreamOptional( result.Passed, f, os );
    os << "\n";

    os << "XStddev     / cm   ";
    for( const GFTestResult & result : results ) StreamOptional( result.XStddev, f, os );
    os << "\n";

    os << "XSigmaFit   / cm   ";
    for( const GFTestResult & result : results ) StreamOptional( result.XSigmaFit, f, os );
    os << "\n";

    os << "YStddev     / cm   ";
    for( const GFTestResult & result : results ) StreamOptional( result.YStddev, f, os );
    os << "\n";

    os << "YSigmaFit   / cm   ";
    for( const GFTestResult & result : results ) StreamOptional( result.YSigmaFit, f, os );
    os << "\n";

    os << "XYCov       / cm^2 ";
    for( const GFTestResult & result : results ) StreamOptional( result.XYCov, f, os );
    os << "\n";

    os << "PLossMean   / keV  ";
    for( const GFTestResult & result : results ) StreamOptional( result.PLossMean, f, os );
    os << "\n";

    os << "PLossMP     / keV  ";
    for( const GFTestResult & result : results ) StreamOptional( result.PLossMP, f, os );
    os << "\n";

    os << "PLossStddev / keV  ";
    for( const GFTestResult & result : results ) StreamOptional( result.PLossStddev, f, os );
    os << "\n";

}





} /* namespace GFTest */
