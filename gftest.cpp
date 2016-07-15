
#include "GeantSim.h"
#include "GenfitExt.h"
#include "TApplication.h"
#include "Options.h"

int main(int argc,char ** argv)
{
    std::unique_ptr<TApplication> rootApp;

    if( GFTest::gOptions->GetGeantDraw() ) {
        static int rootArgc = 0;
        static char * rootArgv = nullptr;
        rootApp.reset( new TApplication("rootApp", & rootArgc, & rootArgv ) );
    }

    GFTest::gOptions->Parse( argc, argv );

    GFTest::GeantSim::Run();
    GFTest::GenfitExt::Run();

    if( rootApp.get() ) rootApp->Run();
}
