
#include "Options.h"

namespace GFTest {

namespace po = boost::program_options;

std::unique_ptr<Options> gOptions( new Options() );

Options::Options()
{
    fOptionsDescription.add_options()
            ( "particle,P", po::value<std::string>( & fParticle )->default_value("e-"), "particle PDG name, e.g.: \"e-\", \"pi+\", \"proton\", ..." )
            ( "energy,E", po::value<double>( & fEnergy_GeVc )->default_value( 0.15 ), "z momentum in GeV/c" )
            ( "thickness,T",  po::value<double>( & fThickness_um )->default_value( 300 ), "scatterer thickness in um" )
            ( "material,M", po::value<std::string>( & fMaterial )->default_value( "Si" ), "scatterer material, e.g. \"Si\",  " )
            ( "geant-n-events,N", po::value<int>( & fGeantNEvents )->default_value( 10000 ), "number of particles to shoot in GEANT" )
            ( "geant-draw,d", po::bool_switch( & fGeantDraw ), "draw the GEANT histograms" )
            ( "geant-out-file", po::value<std::string>( & fGeantOutFile )->default_value("geant_sim.root"), "GEANT out file name" )
            ( "help,h", "Print this help" )
            ;
}



Options::~Options()
{
}



void Options::Parse( int argc, char** argv )
{
    po::store( po::command_line_parser( argc, argv ).options( fOptionsDescription ).run(), fParsedOptions );
    po::notify( fParsedOptions );

    if( fParsedOptions.count( "help" ) ) {
        std::cout << fOptionsDescription << std::endl;
        exit( EXIT_SUCCESS );
    }
}

} /* namespace GFTest */
