
#ifndef OPTIONS_H_
#define OPTIONS_H_

#include "boost/program_options.hpp"
#include <memory>

namespace GFTest {

class Options
{
public:
    Options();
    virtual ~Options();
    void Parse( int argc, char ** argv );

    std::string GetPartType() const { return fParsedOptions["part-type"].as<std::string>(); }
    double GetPartX_cm() const { return fParsedOptions["x"].as<double>(); }
    double GetPartY_cm() const { return fParsedOptions["y"].as<double>(); }
    double GetPartZ_cm() const { return fParsedOptions["z"].as<double>(); }
    double GetPartPx_GeVc() const { return fParsedOptions["px"].as<double>(); }
    double GetPartPy_GeVc() const { return fParsedOptions["py"].as<double>(); }
    double GetPartPz_GeVc() const { return fParsedOptions["pz"].as<double>(); }
    double GetScatThick_um() const { return fParsedOptions["scat-thick"].as<double>(); }
    // double GetDetX_cm() const { return fParsedOptions["det-x"].as<double>(); }
    // double GetDetY_cm() const { return fParsedOptions["det-y"].as<double>(); }
    double GetDetZ_cm() const { return fParsedOptions["det-z"].as<double>(); }
    // double GetDetNX() const { return fParsedOptions["det-nx"].as<double>(); }
    // double GetDetNY() const { return fParsedOptions["det-ny"].as<double>(); }
    // double GetDetNZ() const { return fParsedOptions["det-nz"].as<double>(); }
    int GetGeantNPart() const { return fParsedOptions["geant-n-part"].as<int>(); }
    bool GetGeantDraw() const { return fParsedOptions.count("geant-draw"); }

    double GetPartPMag_GeVc() const { return sqrt( GetPartPx_GeVc() * GetPartPx_GeVc() + GetPartPy_GeVc() * GetPartPy_GeVc() + GetPartPz_GeVc() * GetPartPz_GeVc() ); }

private:
    void Declare();
    boost::program_options::options_description fOptionsDescription;
    boost::program_options::variables_map fParsedOptions;
};

extern std::unique_ptr<Options> gOptions;

} /* namespace GFTest */

#endif /* OPTIONS_H_ */
