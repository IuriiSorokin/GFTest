
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

    //    std::string GetPartType() const { return fParsedOptions["part-type"].as<std::string>(); }
    //    double GetPartX_cm() const { return fParsedOptions["x"].as<double>(); }
    //    double GetPartY_cm() const { return fParsedOptions["y"].as<double>(); }
    //    double GetPartZ_cm() const { return fParsedOptions["z"].as<double>(); }
    //    double GetPartPx_GeVc() const { return fParsedOptions["px"].as<double>(); }
    //    double GetPartPy_GeVc() const { return fParsedOptions["py"].as<double>(); }
    //    double GetPartPz_GeVc() const { return fParsedOptions["pz"].as<double>(); }
    //    double GetScatThick_um() const { return fParsedOptions["scat-thick"].as<double>(); }
    //    std::string GetScatMat() const { return fParsedOptions["scat-mat"].as<std::string>(); }
    //    // double GetDetX_cm() const { return fParsedOptions["det-x"].as<double>(); }
    //    // double GetDetY_cm() const { return fParsedOptions["det-y"].as<double>(); }
    //    double GetDetZ_cm() const { return fParsedOptions["det-z"].as<double>(); }
    //    // double GetDetNX() const { return fParsedOptions["det-nx"].as<double>(); }
    //    // double GetDetNY() const { return fParsedOptions["det-ny"].as<double>(); }
    //    // double GetDetNZ() const { return fParsedOptions["det-nz"].as<double>(); }
    //    std::string GetGeantPhys() const { return fParsedOptions["geant-phys"].as<std::string>(); }
    //    int GetGeantNPart() const { return fParsedOptions["geant-n-part"].as<int>(); }
    //    bool GetGeantDraw() const { return fParsedOptions.count("geant-draw") && fParsedOptions["geant-draw"].as<bool>() ; }
    //    // bool GetRad() const { return fParsedOptions["rad"].as<bool>(); }
    //    // bool GetIon() const { return fParsedOptions["ion"].as<bool>(); }


    std::string GetPartType() const { return fPartType; }
    void        SetPartType( std::string partType ) { fPartType = partType; }

    double      GetPartX_cm() const { return fPartX_cm; }
    void        SetPartX_cm( double partX_cm ) { fPartX_cm = partX_cm; }

    double      GetPartY_cm() const { return fPartY_cm; }
    void        SetPartY_cm( double partY_cm ) { fPartY_cm = partY_cm; }

    double      GetPartZ_cm() const { return fPartZ_cm; }
    void        SetPartZ_cm( double partZ_cm ) { fPartZ_cm = partZ_cm; }

    double      GetPartPx_GeVc() const { return fPartPx_GeVc; }
    void        SetPartPx_GeVc( double partPx_GeVc ) { fPartPx_GeVc = partPx_GeVc; }

    double      GetPartPy_GeVc() const { return fPartPy_GeVc; }
    void        SetPartPy_GeVc( double partPy_GeVc ) { fPartPy_GeVc = partPy_GeVc; }

    double      GetPartPz_GeVc() const { return fPartPz_GeVc; }
    void        SetPartPz_GeVc( double partPz_GeVc ) { fPartPz_GeVc = partPz_GeVc; }

    double      GetScatThick_um() const { return fScatThick_um; }
    void        SetScatThick_um( double scatThick_um) { fScatThick_um = scatThick_um; }

    std::string GetScatMat() const { return fScatMat; }
    void        SetScatMat( std::string scatMat ) { fScatMat = scatMat; }

    double      GetDetZ_cm() const { return fDetZ_cm; }
    void        SetDetZ_cm( double detZ_cm ) { fDetZ_cm = detZ_cm; }

    std::string GetGeantPhys() const { return fGeantPhys; }
    void        SetGeantPhys( std::string geantPhys ) { fGeantPhys = geantPhys; }

    int         GetGeantNPart() const { return fGeantNPart; }
    void        SetGeantNPart( int geantNPart ) { fGeantNPart = geantNPart; }

    bool        GetGeantDraw() const { return fGeantDraw; }
    void        SetGeantDraw( bool geantDraw ) { fGeantDraw = geantDraw; }

    std::string GetGeantOutFile() const { return fGeantOutFile; }
    void        SetGeantOutFile( std::string geantOutFile ) { fGeantOutFile = geantOutFile; }

    double GetPartPMag_GeVc() const { return sqrt( GetPartPx_GeVc() * GetPartPx_GeVc() + GetPartPy_GeVc() * GetPartPy_GeVc() + GetPartPz_GeVc() * GetPartPz_GeVc() ); }

private:
    void Declare();
    boost::program_options::options_description fOptionsDescription;
    boost::program_options::variables_map fParsedOptions;

    std::string fPartType;
    double      fPartX_cm;
    double      fPartY_cm;
    double      fPartZ_cm;
    double      fPartPx_GeVc;
    double      fPartPy_GeVc;
    double      fPartPz_GeVc;
    double      fScatThick_um;
    std::string fScatMat;
    double      fDetZ_cm;
    std::string fGeantPhys;
    int         fGeantNPart;
    bool        fGeantDraw;
    std::string fGeantOutFile;
};

extern std::unique_ptr<Options> gOptions;

} /* namespace GFTest */

#endif /* OPTIONS_H_ */
