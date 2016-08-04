
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

    std::string GetParticle() const { return fParticle; }
    void        SetParticle( std::string partType ) { fParticle = partType; }

    // Kinetic energy
    double      GetEnergy_GeV() const { return fEnergy_GeVc; }
    void        SetEnergy_GeV( double energy_GeV ) { fEnergy_GeVc = energy_GeV; }

    double      GetThickness_um() const { return fThickness_um; }
    void        SetThickness_um( double thickness_um) { fThickness_um = thickness_um; }

    std::string GetMaterial() const { return fMaterial; }
    void        SetMaterial( std::string material ) { fMaterial = material; }

    int         GetGeantNEvents() const { return fGeantNEvents; }
    void        SetGeantNEvents( int geantNPart ) { fGeantNEvents = geantNPart; }

    bool        GetGeantDraw() const { return fGeantDraw; }
    void        SetGeantDraw( bool geantDraw ) { fGeantDraw = geantDraw; }

    std::string GetGeantOutFile() const { return fGeantOutFile; }
    void        SetGeantOutFile( std::string geantOutFile ) { fGeantOutFile = geantOutFile; }

private:
    void Declare();
    boost::program_options::options_description fOptionsDescription;
    boost::program_options::variables_map fParsedOptions;

    std::string fParticle;
    double      fEnergy_GeVc;
    double      fThickness_um;
    std::string fMaterial;
    int         fGeantNEvents;
    bool        fGeantDraw;
    std::string fGeantOutFile;
};

extern std::unique_ptr<Options> gOptions;

} /* namespace GFTest */

#endif /* OPTIONS_H_ */
