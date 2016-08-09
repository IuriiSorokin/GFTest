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
    boost::format fmt("%|16g|");

    os << "Passed               ";
    for( const GFTestResult & result : results ) StreamOptional( result.Passed, fmt, os );
    os << "\n";

    os << "ELossMean     / keV  ";
    for( const GFTestResult & result : results ) StreamOptional( result.ELossMean, fmt, os );
    os << "\n";

    os << "ELossGausMean / keV  ";
    for( const GFTestResult & result : results ) StreamOptional( result.ELossGausMean, fmt, os );
    os << "\n";

    os << "ELossStddev / keV    ";
    for( const GFTestResult & result : results ) StreamOptional( result.ELossStddev, fmt, os );
    os << "\n";

    os << "ELossGausSigma / keV ";
    for( const GFTestResult & result : results ) StreamOptional( result.ELossGausSigma, fmt, os );
    os << "\n";

    os << "TxStddev             ";
    for( const GFTestResult & result : results ) StreamOptional( result.TxStddev, fmt, os );
    os << "\n";

    os << "TxGausSigma          ";
    for( const GFTestResult & result : results ) StreamOptional( result.TxGausSigma, fmt, os );
    os << "\n";

    os << "TyStddev             ";
    for( const GFTestResult & result : results ) StreamOptional( result.TyStddev, fmt, os );
    os << "\n";

    os << "TyGausSigma          ";
    for( const GFTestResult & result : results ) StreamOptional( result.TyGausSigma, fmt, os );
    os << "\n";

    os << "TailCutoff           ";
    for( const GFTestResult & result : results ) StreamOptional( result.TailCutoff, fmt, os );
    os << "\n";
}





} /* namespace GFTest */
