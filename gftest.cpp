
#include "GeantSim.h"
#include "TApplication.h"
#include "Options.h"

using GFTest::gOptions;
using GFTest::GFTestResult;

int main(int argc,char ** argv)
{
    gOptions->Parse( argc, argv );

    std::unique_ptr<TApplication> rootApp;
    if( gOptions->GetGeantDraw() ) {
        static int rootArgc = 0;
        static char * rootArgv = nullptr;
        rootApp.reset( new TApplication("rootApp", & rootArgc, & rootArgv ) );
    }

    GFTestResult geantResult = GFTest::GeantSim::Run();

    std::cout << "================================================================\n";
    GFTestResult::Compare( { geantResult } ); // simply prints geantResult

    if( rootApp.get() ) {
        rootApp->Run();
    }

}
