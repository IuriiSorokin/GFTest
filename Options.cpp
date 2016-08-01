
#include "Options.h"

namespace GFTest {

namespace po = boost::program_options;

std::unique_ptr<Options> gOptions( new Options() );

Options::Options()
{
    fOptionsDescription.add_options()
            ( "part-type,P", po::value<std::string>( & fPartType )->default_value("e-"), "particle PDG name, e.g.: \"e-\", \"pi+\", \"proton\", ..." )
            ( "x", po::value<double>( & fPartX_cm )->default_value(   0 ), "initial x position in cm" )
            ( "y", po::value<double>( & fPartY_cm )->default_value(   0 ), "initial y position in cm" )
            ( "z", po::value<double>( & fPartZ_cm )->default_value( -10 ), "initial z position in cm" )
            ( "px", po::value<double>( & fPartPx_GeVc )->default_value(     0 ), "x momentum in GeV/c" )
            ( "py", po::value<double>( & fPartPy_GeVc )->default_value(     0 ), "y momentum in GeV/c" )
            ( "pz", po::value<double>( & fPartPz_GeVc )->default_value( 0.15 ), "z momentum in GeV/c" )
            ( "scat-thick,t",  po::value<double>( & fScatThick_um )->default_value( 300 ), "scatterer thickness in um" )
            ( "scat-mat", po::value<std::string>( & fScatMat )->default_value( "Si" ), "scatterer material, e.g. \"Si\",  " )
         // ( "det-x", po::value<double>( & fDetX_cm )->default_value( 0 ), "detector plane position in cm" )
         // ( "det-y", po::value<double>( & fDetY_cm )->default_value( 0 ), "detector plane position in cm" )
            ( "det-z", po::value<double>( & fDetZ_cm )->default_value( 10 ), "detector plane position in cm" )
         // ( "det-nx", po::value<double>()->default_value( 0 ), "detector plane normal x" )
         // ( "det-ny", po::value<double>()->default_value( 0 ), "detector plane normal y" )
         // ( "det-nz", po::value<double>()->default_value( 1 ), "detector plane normal z" )
            ( "geant-phys", po::value<std::string>( & fGeantPhys )->default_value("FTFP_BERT_EMY"), "GEANT physics list" )
            ( "geant-n-part,N", po::value<int>( & fGeantNPart )->default_value( 10000 ), "number of particles to shoot in GEANT" )
            ( "geant-draw,d", po::bool_switch( & fGeantDraw ), "draw the GEANT histograms" )
            ( "geant-out-file", po::value<std::string>( & fGeantOutFile )->default_value("geant_sim.root"), "GEANT out file name" )
         // ( "rad", po::value<bool>( & fRad )->default_value( true ), "radiative energy losses" )
         // ( "ion", po::value<bool>( & fIon )->default_value( true ), "ionizing energy losses" )
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
