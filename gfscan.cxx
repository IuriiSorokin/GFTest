
#include "GeantSim.h"
#include "GenfitExt.h"
#include "TApplication.h"
#include "Options.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TFile.h"
#include <unistd.h>
#include <wait.h>
#include "TLegend.h"
#include "TF1.h"

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <string>
#include <cstdlib> //std::system

using GFTest::gOptions;
using GFTest::GFTestResult;
using namespace boost::interprocess;



template< typename Func, typename... Args >
auto CallInNewProcess(Func fun, Args... args) -> decltype( fun(args...) )
{
    using return_type = decltype(fun(args...)) ;

    struct shm_remove
    {
        shm_remove()  { boost::interprocess::shared_memory_object::remove("GFTestResultSegment"); }
        ~shm_remove() { boost::interprocess::shared_memory_object::remove("GFTestResultSegment"); }
    } remover;

    boost::interprocess::managed_shared_memory segment( boost::interprocess::create_only, "GFTestResultSegment", 65536 * (1 + sizeof(return_type) % 65536 ) );

    return_type * result = segment.construct<return_type>("GeantGFTestResult")();

    pid_t pid = fork();
    if( pid == 0 ) {
        // child process
        return_type * result_child= segment.find<return_type>("GeantGFTestResult").first;
        * result_child = fun(args...);
        _exit(0);
    }
    else if( pid > 0 ) {
        // Wait for the child to terminate (or it becomes a zombie)
        int status;
        waitpid(pid, &status, 0);
    }
    return * result;
}



struct GFTestScanResults
{
    std::string Name;
    std::string Title;
    Color_t     MarkerColor;
    Style_t     MarkerStyle;

    std::vector <double>       Energy;
    std::vector <GFTestResult> GeantResults;
    std::vector <GFTestResult> GenfitResults;

    GFTestScanResults() :
        MarkerColor( kBlack ),
        MarkerStyle(24)
    {
    }
};


std::vector<double> gEnergies_GeV( {0.01, 0.02, 0.05, 0.098, 0.102, 0.15, 0.2, 0.35, 0.5, 0.7, 1, 2, 5, 10 } );

// std::vector<double> gEnergies_GeV( {0.102, 0.15, 0.5, 1 } );
// std::vector<double> gEnergies_GeV( {0.01 } );



GFTestScanResults ScanWithThisOptions()
{
    GFTestScanResults scan;
    for( double energy_GeV : gEnergies_GeV ) {
           gOptions->SetEnergy_GeV( energy_GeV );
           gOptions->SetGeantOutFile( Form("geant_out/GeantHist_%s_%lf_%s_%lf.root", gOptions->GetParticle().c_str(), gOptions->GetThickness_um(), gOptions->GetMaterial().c_str(), energy_GeV ) );
           scan.Energy.push_back( energy_GeV );
           scan.GeantResults.emplace_back( CallInNewProcess( GFTest::GeantSim::Run ) );
           scan.GenfitResults.emplace_back( CallInNewProcess( GFTest::GenfitExt::Run ) );
       }
    return scan;
}



//template<typename Value>
//TCanvas * Plot( const std::vector<GFTestScanResults> & scans, Value what, std::string title )
//{
//    TCanvas * canv;
//    for( const auto & scan : scans ) {
//        double energy = scan.Energy;
//        const GFTestResult & geantResult = scan.GeantResults;
//    }
//}



TCanvas * PlotRelDiff( const std::vector<GFTestScanResults> & scans,  boost::optional<double> GFTestResult::* value, std::string title )
{
    TCanvas * canv = new TCanvas();

    TGraph *  scale( new TGraph() );
    scale->SetNameTitle( "Scale", title.c_str() );
    scale->SetPoint(0, gEnergies_GeV.front(), -1);
    scale->SetPoint(1, gEnergies_GeV.back(),   1);
    scale->Draw("ap");
    scale->GetXaxis()->SetTitle("Initial p / GeV/c");
    scale->GetYaxis()->SetTitle( "(GENFIT-GEANT)/GEANT" );
    scale->GetYaxis()->SetTitleOffset(1.3);

    TLegend * legend = new TLegend(0.65, 0.18, 0.85, 0.28 );

    for( const GFTestScanResults & scan : scans ) {
        TGraph * scanGraph = new TGraph();
        scanGraph->SetNameTitle( scan.Name.c_str(), scan.Title.c_str() );

        for( size_t iEnergy = 0; iEnergy < scan.Energy.size(); ++iEnergy ) {
            double energy     = scan.Energy.at(iEnergy);
            const boost::optional<double> & geantValue  = scan.GeantResults.at(iEnergy).*value;
            const boost::optional<double> & genfitValue = scan.GenfitResults.at(iEnergy).*value;

            if( geantValue.is_initialized() && genfitValue.is_initialized() ) {
                double relDiff = (genfitValue.get() - geantValue.get()) / geantValue.get();
                scanGraph->SetPoint( scanGraph->GetN(), energy, relDiff );
            }
        }

        scanGraph->Draw("plsame");
        scanGraph->SetMarkerStyle( scan.MarkerStyle );
        scanGraph->SetMarkerColor( scan.MarkerColor );
        scanGraph->SetLineStyle( 3 );
        scanGraph->SetLineColor( scan.MarkerColor );
        legend->AddEntry( scanGraph, scan.Title.c_str(), "pl" );
        legend->Draw();
    }

    return canv;
}



int main(int argc, char ** argv)
{
    (void) argc;
    (void) argv;

    std::vector<GFTestScanResults> scans;

    gOptions->Parse( 0, nullptr ); // Set default options
    gOptions->SetGeantNEvents( 1e6 );
    
//    gOptions->SetParticle("e-");
//    gOptions->SetMaterial("Si");
//    gOptions->SetThickness_um( 50 );
//    scans.emplace_back( ScanWithThisOptions() );
//    scans.back().Name =  "e-_50um_Si";
//    scans.back().Title = "e^{-} 50um Si";
//    scans.back().MarkerColor = 47;
//
//    gOptions->SetParticle("e-");
//    gOptions->SetMaterial("Si");
//    gOptions->SetThickness_um( 300 );
//    scans.emplace_back( ScanWithThisOptions() );
//    scans.back().Name =  "e-_300um_Si";
//    scans.back().Title = "e^{-} 300um Si";
//    scans.back().MarkerColor = kRed;
//
//
//    gOptions->SetParticle("e-");
//    gOptions->SetMaterial("Pb");
//    gOptions->SetThickness_um( 1000 );
//    scans.emplace_back( ScanWithThisOptions() );
//    scans.back().Name =  "e-_1000um_Pb";
//    scans.back().Title = "e^{-} 1000um Pb";
//    scans.back().MarkerColor = kBlue;
//
//    gOptions->SetParticle("e-");
//    gOptions->SetMaterial("Be");
//    gOptions->SetThickness_um( 5000 );
//    scans.emplace_back( ScanWithThisOptions() );
//    scans.back().Name =  "e-_5000um_Be";
//    scans.back().Title = "e^{-} 5000um Be";
//    scans.back().MarkerColor = kMagenta;

    gOptions->SetParticle("pi+");
    gOptions->SetMaterial("Si");
    gOptions->SetThickness_um( 50 );
    scans.emplace_back( ScanWithThisOptions() );
    scans.back().Name =  "pi+_50um_Si";
    scans.back().Title = "#pi^{+} 50um Si";
    scans.back().MarkerColor = 47;
    
    gOptions->SetParticle("pi-");
    gOptions->SetMaterial("Si");
    gOptions->SetThickness_um( 300 );
    scans.emplace_back( ScanWithThisOptions() );
    scans.back().Name =  "pi-_300um_Si";
    scans.back().Title = "#pi^{-} 300um Si";
    scans.back().MarkerColor = kRed;

    gOptions->SetParticle("pi-");
    gOptions->SetMaterial("Pb");
    gOptions->SetThickness_um( 1000 );
    scans.emplace_back( ScanWithThisOptions() );
    scans.back().Name =  "pi-_1000um_Pb";
    scans.back().Title = "pi^{-} 1000um Pb";
    scans.back().MarkerColor = kBlue;

    gOptions->SetParticle("mu-");
    gOptions->SetMaterial("Pb");
    gOptions->SetThickness_um( 1000 );
    scans.emplace_back( ScanWithThisOptions() );
    scans.back().Name =  "mu-_1000um_Pb";
    scans.back().Title = "mu^{-} 1000um Pb";
    scans.back().MarkerColor = 36;

    gOptions->SetParticle("proton");
    gOptions->SetMaterial("Be");
    gOptions->SetThickness_um( 5000 );
    scans.emplace_back( ScanWithThisOptions() );
    scans.back().Name =  "p+_5000um_Be";
    scans.back().Title = "p^{+} 5000um Be";
    scans.back().MarkerColor = kMagenta;

    gOptions->SetParticle("pi-");
    gOptions->SetMaterial("Be");
    gOptions->SetThickness_um( 500 );
    scans.emplace_back( ScanWithThisOptions() );
    scans.back().Name =  "pi-_500um_Be";
    scans.back().Title = "#pi^{-} 500um Be";
    scans.back().MarkerColor = 32;

    gOptions->SetParticle("pi-");
    gOptions->SetMaterial("C");
    gOptions->SetThickness_um( 500 );
    scans.emplace_back( ScanWithThisOptions() );
    scans.back().Name =  "pi-_500um_C";
    scans.back().Title = "#pi^{-} 500um C";
    scans.back().MarkerColor = kCyan;

    gOptions->SetParticle("proton");
    gOptions->SetMaterial("Ta");
    gOptions->SetThickness_um( 1000 );
    scans.emplace_back( ScanWithThisOptions() );
    scans.back().Name =  "p+_1000um_Ta";
    scans.back().Title = "p^{+} 1000um Ta";
    scans.back().MarkerColor = kBlack;

    std::cout << "Number of scans: " << scans.size() << std::endl;

    TFile fileOut("scan.root", "recreate");

    PlotRelDiff( scans, &GFTestResult::ELossMean,   "Mean energy loss" )->Write();
    PlotRelDiff( scans, &GFTestResult::ELossGausMean,   "Gaus Mean energy loss" )->Write();
    PlotRelDiff( scans, &GFTestResult::ELossStddev, "Standard deviation of energy loss" )->Write();
    PlotRelDiff( scans, &GFTestResult::ELossGausSigma, "Gaus Sigma of energy loss" )->Write();
    PlotRelDiff( scans, &GFTestResult::TxStddev,    "Standard deviation of direction tangent X" )->Write();
    PlotRelDiff( scans, &GFTestResult::TxGausSigma,    "GausSigma of direction tangent X" )->Write();
    PlotRelDiff( scans, &GFTestResult::TyStddev,    "Standard deviation of direction tangent Y" )->Write();
    PlotRelDiff( scans, &GFTestResult::TyGausSigma,    "GausSigma of direction tangent Y" )->Write();


    fileOut.Close();
}

