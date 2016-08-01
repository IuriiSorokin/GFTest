
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



struct GFTestReslutVsMomentum {
    std::unique_ptr<TGraph> grPassedGeant;
    std::unique_ptr<TGraph> grPassedGenfit;
    std::unique_ptr<TGraph> grPLossMeanGeant;
    std::unique_ptr<TGraph> grPLossMeanGenfit;
    std::unique_ptr<TGraph> grPLossMeanRdiff;
    std::unique_ptr<TGraph> grPLossStddevGeant;
    std::unique_ptr<TGraph> grPLossStddevGenfit;
    std::unique_ptr<TGraph> grPLossStddevRdiff;
    std::unique_ptr<TGraph> grScatXGeant;
    std::unique_ptr<TGraph> grScatXGenfit;
    std::unique_ptr<TGraph> grScatXRdiff;
    std::unique_ptr<TGraph> grScatYGeant;
    std::unique_ptr<TGraph> grScatYGenfit;
    std::unique_ptr<TGraph> grScatYRdiff;

    GFTestReslutVsMomentum( std::string name ) :
        grPassedGeant( new TGraph ),
        grPassedGenfit( new TGraph ),
        grPLossMeanGeant( new TGraph ),
        grPLossMeanGenfit( new TGraph ),
        grPLossMeanRdiff( new TGraph ),
        grPLossStddevGeant( new TGraph ),
        grPLossStddevGenfit( new TGraph ),
        grPLossStddevRdiff( new TGraph ),
        grScatXGeant( new TGraph ),
        grScatXGenfit( new TGraph ),
        grScatXRdiff( new TGraph ),
        grScatYGeant( new TGraph ),
        grScatYGenfit( new TGraph ),
        grScatYRdiff( new TGraph )
    {
        grPassedGeant->SetName( Form("grPassedGeant_%s", name.c_str() ) );
        grPassedGeant->SetTitle( grPassedGeant->GetName() );

        grPassedGenfit->SetName( Form("grPassedGenfit_%s", name.c_str() ) );
        grPassedGenfit->SetTitle( grPassedGenfit->GetName() );

        grPLossMeanGeant->SetName( Form("grPLossMeanGeant_%s", name.c_str() ) );
        grPLossMeanGeant->SetTitle( grPLossMeanGeant->GetName() );

        grPLossMeanGenfit->SetName( Form("grPLossMeanGenfit_%s", name.c_str() ) );
        grPLossMeanGenfit->SetTitle( grPLossMeanGenfit->GetName() );

        grPLossMeanRdiff->SetName( Form("grPLossMeanRdiff_%s", name.c_str() ) );
        grPLossMeanRdiff->SetTitle( grPLossMeanRdiff->GetName() );

        grPLossStddevGeant->SetName( Form("grPLossStddevGeant_%s", name.c_str() ) );
        grPLossStddevGeant->SetTitle( grPLossStddevGeant->GetName() );

        grPLossStddevGenfit->SetName( Form("grPLossStddevGenfit_%s", name.c_str() ) );
        grPLossStddevGenfit->SetTitle( grPLossStddevGenfit->GetName() );

        grPLossStddevRdiff->SetName( Form("grPLossStddevRdiff_%s", name.c_str() ) );
        grPLossStddevRdiff->SetTitle( grPLossStddevRdiff->GetName() );

        grScatXGeant->SetName( Form("grScatXGeant_%s", name.c_str() ) );
        grScatXGeant->SetTitle( grScatXGeant->GetName() );

        grScatXGenfit->SetName( Form("grScatXGenfit_%s", name.c_str() ) );
        grScatXGenfit->SetTitle( grScatXGenfit->GetName() );

        grScatXRdiff->SetName( Form("grScatXRdiff_%s", name.c_str() ) );
        grScatXRdiff->SetTitle( grScatXRdiff->GetName() );

        grScatYGeant->SetName( Form("grScatYGeant_%s", name.c_str() ) );
        grScatYGeant->SetTitle( grScatYGeant->GetName() );

        grScatYGenfit->SetName( Form("grScatYGenfit_%s", name.c_str() ) );
        grScatYGenfit->SetTitle( grScatYGenfit->GetName() );

        grScatYRdiff->SetName( Form("grScatYRdiff_%s", name.c_str() ) );
        grScatYRdiff->SetTitle( grScatYRdiff->GetName() );
    }
};


std::vector<double> gMomenta( {0.01, 0.02, 0.05, 0.098, 0.102, 0.15, 0.2, 0.35, 0.5, 0.7, 1, 2, 5, 10 } );
// std::vector<double> gMomenta( {0.102, 0.15, 0.5, 1 } );
// std::vector<double> gMomenta( {0.5 } );


GFTestReslutVsMomentum ScanWithThisOptions( std::string name )
{
    GFTestReslutVsMomentum scan( name );
    for( double pz : gMomenta ) {
           gOptions->SetPartPz_GeVc( pz );
           gOptions->SetGeantOutFile( Form("geant_out/GeantHist_%s_%lf.root", name.c_str(), pz ) );

           GFTestResult geantResult  = CallInNewProcess( GFTest::GeantSim::Run );

           if( geantResult.Passed.get_value_or(0) < 0.999 ) {
               continue;
           }

           GFTestResult genfitResult = CallInNewProcess( GFTest::GenfitExt::Run );

           if( genfitResult.Passed.get_value_or(0) < 0.999 ) {
               std::cout << "ERROR: In GEANT the particles pass through, while in GENFIT don't" << std::endl;
               exit(1);
           }


           scan.grPassedGeant->SetPoint( scan.grPassedGeant->GetN(), pz, geantResult.Passed.get_value_or(0) );
           scan.grPassedGenfit->SetPoint( scan.grPassedGenfit->GetN(), pz, genfitResult.Passed.get_value_or(0) );

           scan.grPLossMeanGeant->SetPoint( scan.grPLossMeanGeant->GetN(), pz, geantResult.PLossMean.get_value_or(0) );
           scan.grPLossMeanGenfit->SetPoint( scan.grPLossMeanGenfit->GetN(), pz, genfitResult.PLossMean.get_value_or(0) );
           scan.grPLossMeanRdiff->SetPoint( scan.grPLossMeanRdiff->GetN(), pz, ( genfitResult.PLossMean.get_value_or(0) - geantResult.PLossMean.get_value_or(0) ) / geantResult.PLossMean.get_value_or(0) );

           scan.grPLossStddevGeant->SetPoint( scan.grPLossStddevGeant->GetN(), pz, geantResult.PLossStddev.get_value_or(0) );
           scan.grPLossStddevGenfit->SetPoint( scan.grPLossStddevGenfit->GetN(), pz, genfitResult.PLossStddev.get_value_or(0) );
           scan.grPLossStddevRdiff->SetPoint( scan.grPLossStddevRdiff->GetN(), pz, ( genfitResult.PLossStddev.get_value_or(0) - geantResult.PLossStddev.get_value_or(0) ) / geantResult.PLossStddev.get_value_or(0) );

           scan.grScatXGeant->SetPoint( scan.grScatXGeant->GetN(), pz, geantResult.XStddev.get_value_or(0) );
           scan.grScatXGenfit->SetPoint( scan.grScatXGenfit->GetN(), pz, genfitResult.XStddev.get_value_or(0) );
           scan.grScatXRdiff->SetPoint( scan.grScatXRdiff->GetN(), pz, ( genfitResult.XStddev.get_value_or(0) - geantResult.XStddev.get_value_or(0) ) / geantResult.XStddev.get_value_or(0) );

           scan.grScatYGeant->SetPoint( scan.grScatYGeant->GetN(), pz, geantResult.YStddev.get_value_or(0) );
           scan.grScatYGenfit->SetPoint( scan.grScatYGenfit->GetN(), pz, genfitResult.YStddev.get_value_or(0) );
           scan.grScatYRdiff->SetPoint( scan.grScatYRdiff->GetN(), pz, ( genfitResult.YStddev.get_value_or(0) - geantResult.YStddev.get_value_or(0) ) / geantResult.YStddev.get_value_or(0) );
       }

    return scan;
}



void WriteScan( GFTestReslutVsMomentum & scan )
{
    scan.grPassedGeant->Write();
    scan.grPassedGenfit->Write();
    scan.grPLossMeanGeant->Write();
    scan.grPLossMeanGenfit->Write();
    scan.grPLossMeanRdiff->Write();
    scan.grPLossStddevGeant->Write();
    scan.grPLossStddevGenfit->Write();
    scan.grPLossStddevRdiff->Write();
    scan.grScatXGeant->Write();
    scan.grScatXGenfit->Write();
    scan.grScatXRdiff->Write();
    scan.grScatYGeant->Write();
    scan.grScatYGenfit->Write();
    scan.grScatYRdiff->Write();
}



//std::unique_ptr<TCanvas> MakePLossMeanRdiffCombined( std::vector< std::pair< std::reference_wrapper<GFTestReslutVsMomentum>, std::string > > data )
//{
//    std::unique_ptr<TCanvas> cPLossMeanRDiffCombined( new TCanvas("PLossMeanRdiffCombined", "PLossMeanRdiffCombined" ) );
//    std::unique_ptr<TGraph>  scale( new TGraph() );
//    scale->SetNameTitle("PLossMeanRdiffCombinedScale", "Relative difference of the mean momentum loss");
//    scale->SetPoint(0, gMomenta.front(), -0.15);
//    scale->SetPoint(1, gMomenta.back(),   0.15);
//    scale->Draw("ap");
//    scale->GetXaxis()->SetTitle("Initial p / GeV/c");
//    scale->GetYaxis()->SetTitle("(#Deltap_{GENFIT} - #Deltap_{GEANT})/#Deltap_{GEANT}");
//    scale->GetYaxis()->SetTitleOffset(1.3);
//    cPLossMeanRDiffCombined->SetLogx(1);
//    std::unique_ptr<TF1> zeroLine( new TF1("PLossMeanRdiffCombinedZero", "pol0", - gMomenta.back(), 2 * gMomenta.back() ) );
//    zeroLine->Draw("same");
//
//    std::unique_ptr<TLegend> lPLossMean( new TLegend(0.65, 0.18, 0.85, 0.28 ) );
//
////    scan_300umSi_emy.grPLossMeanRdiff->Draw("psame");
////    scan_300umSi_emy.grPLossMeanRdiff->SetMarkerStyle(24);
////    scan_300umSi_emy.grPLossMeanRdiff->SetMarkerColor(kRed);
////    lPLossMean->AddEntry( scan_300umSi_emy.grPLossMeanRdiff.get(), "300um Si, EMY", "p" );
////
////    scan_80umPb.grPLossMeanRdiff->Draw("psame");
////    scan_80umPb.grPLossMeanRdiff->SetMarkerStyle(24);
////    scan_80umPb.grPLossMeanRdiff->SetMarkerColor(kBlue);
////    lPLossMean->AddEntry( scan_80umPb.grPLossMeanRdiff.get(), "80um Pb, EMY", "p" );
//
//    lPLossMean->Draw();
//    cPLossMeanRDiffCombined->Write();
//}


int main(int argc, char ** argv)
{
    TH1::StatOverflows( true );

    (void) argc;
    (void) argv;

    gOptions->Parse( 0, nullptr );
    gOptions->SetGeantNPart( 1e6 );

    TFile fileOut("scan.root", "recreate");

    gOptions->SetScatMat("Si");
    gOptions->SetScatThick_um( 50 );
    GFTestReslutVsMomentum scan_electron_50umSi_emy = ScanWithThisOptions("50umSi");
    WriteScan( scan_electron_50umSi_emy );

    gOptions->SetScatMat("Si");
    gOptions->SetScatThick_um( 300 );
    GFTestReslutVsMomentum scan_electron_300umSi_emy = ScanWithThisOptions("300umSi");
    WriteScan( scan_electron_300umSi_emy );

    gOptions->SetScatMat("Pb");
    gOptions->SetScatThick_um( 1000 );
    GFTestReslutVsMomentum scan_electron_1000umPb = ScanWithThisOptions("1000umPb");
    WriteScan( scan_electron_1000umPb );

    gOptions->SetScatMat("Be");
    gOptions->SetScatThick_um( 5000 );
    GFTestReslutVsMomentum scan_electron_5000umBe = ScanWithThisOptions("5000umBe");
    WriteScan( scan_electron_5000umBe );

    gOptions->SetScatMat("C");
    gOptions->SetScatThick_um( 500 );
    gOptions->SetPartType("pi-");
    GFTestReslutVsMomentum scan_piminus_500umC = ScanWithThisOptions("500umC");
    WriteScan( scan_piminus_500umC );

    gOptions->SetScatMat("Ta");
    gOptions->SetScatThick_um( 1000 );
    gOptions->SetPartType("proton");
    GFTestReslutVsMomentum scan_proton_1000umTa = ScanWithThisOptions("1000umTa");
    WriteScan( scan_proton_1000umTa );

    // ================
    std::unique_ptr<TCanvas> cPLossMeanRDiffCombined( new TCanvas("PLossMeanRdiffCombined", "PLossMeanRdiffCombined" ) );
    std::unique_ptr<TGraph>  scalePLossMean( new TGraph() );
    scalePLossMean->SetNameTitle("PLossMeanRdiffCombinedScale", "Relative difference of the mean momentum loss");
    scalePLossMean->SetPoint(0, gMomenta.front(), -1.0);
    scalePLossMean->SetPoint(1, gMomenta.back(),   1.0);
    scalePLossMean->Draw("ap");
    scalePLossMean->GetXaxis()->SetTitle("Initial p / GeV/c");
    scalePLossMean->GetYaxis()->SetTitle("(#Deltap_{GENFIT} - #Deltap_{GEANT})/#Deltap_{GEANT}");
    scalePLossMean->GetYaxis()->SetTitleOffset(1.3);
    cPLossMeanRDiffCombined->SetLogx(1);
    std::unique_ptr<TF1> zeroLinePLossMean( new TF1("PLossMeanRdiffCombinedZero", "pol0", - gMomenta.back(), 2 * gMomenta.back() ) );
    zeroLinePLossMean->Draw("same");

    std::unique_ptr<TLegend> lPLossMean( new TLegend(0.65, 0.18, 0.85, 0.28 ) );

    scan_electron_50umSi_emy.grPLossMeanRdiff->Draw("psame");
    scan_electron_50umSi_emy.grPLossMeanRdiff->SetMarkerStyle(24);
    scan_electron_50umSi_emy.grPLossMeanRdiff->SetMarkerColor(47);
    lPLossMean->AddEntry( scan_electron_50umSi_emy.grPLossMeanRdiff.get(), "e^{-} 50um Si, EMY", "p" );

    scan_electron_300umSi_emy.grPLossMeanRdiff->Draw("psame");
    scan_electron_300umSi_emy.grPLossMeanRdiff->SetMarkerStyle(24);
    scan_electron_300umSi_emy.grPLossMeanRdiff->SetMarkerColor(kRed);
    lPLossMean->AddEntry( scan_electron_300umSi_emy.grPLossMeanRdiff.get(), "e^{-} 300um Si, EMY", "p" );

    scan_electron_5000umBe.grPLossMeanRdiff->Draw("psame");
    scan_electron_5000umBe.grPLossMeanRdiff->SetMarkerStyle(24);
    scan_electron_5000umBe.grPLossMeanRdiff->SetMarkerColor(kMagenta);
    lPLossMean->AddEntry( scan_electron_5000umBe.grPLossMeanRdiff.get(), "e^{-} 5000um Be, EMY", "p" );

    scan_electron_1000umPb.grPLossMeanRdiff->Draw("psame");
    scan_electron_1000umPb.grPLossMeanRdiff->SetMarkerStyle(24);
    scan_electron_1000umPb.grPLossMeanRdiff->SetMarkerColor(kBlue);
    lPLossMean->AddEntry( scan_electron_1000umPb.grPLossMeanRdiff.get(), "e^{-} 1000um Pb, EMY", "p" );

    scan_piminus_500umC.grPLossMeanRdiff->Draw("psame");
    scan_piminus_500umC.grPLossMeanRdiff->SetMarkerStyle(24);
    scan_piminus_500umC.grPLossMeanRdiff->SetMarkerColor(kBlack);
    lPLossMean->AddEntry( scan_piminus_500umC.grPLossMeanRdiff.get(), "#pi^{-} 500um C, EMY", "p" );

    scan_proton_1000umTa.grPLossMeanRdiff->Draw("psame");
    scan_proton_1000umTa.grPLossMeanRdiff->SetMarkerStyle(24);
    scan_proton_1000umTa.grPLossMeanRdiff->SetMarkerColor(kCyan);
    lPLossMean->AddEntry( scan_proton_1000umTa.grPLossMeanRdiff.get(), "p^{+} 1000um Ta, EMY", "p" );

    lPLossMean->Draw();
    cPLossMeanRDiffCombined->Write();


    // =================
    std::unique_ptr<TCanvas> cPLossStddevRDiffCombined( new TCanvas("PLossStddevRdiffCombined", "PLossStddevRdiffCombined" ) );
    std::unique_ptr<TGraph>  scalePLossStddev( new TGraph() );
    scalePLossStddev->SetNameTitle("PLossStddevRdiffCombinedScale", "Relative difference of standard deviation of the mean momentum loss");
    scalePLossStddev->SetPoint(0, gMomenta.front(), -0.15);
    scalePLossStddev->SetPoint(1, gMomenta.back(),   0.15);
    scalePLossStddev->Draw("ap");
    scalePLossStddev->GetXaxis()->SetTitle("Initial p / GeV/c");
    scalePLossStddev->GetYaxis()->SetTitle("(#Deltap_{GENFIT} - #Deltap_{GEANT})/#Deltap_{GEANT}");
    scalePLossStddev->GetYaxis()->SetTitleOffset(1.3);
    cPLossStddevRDiffCombined->SetLogx(1);
    std::unique_ptr<TF1> zeroLinePLossStddev( new TF1("PLossStddevRdiffCombinedZero", "pol0", - gMomenta.back(), 2 * gMomenta.back() ) );
    zeroLinePLossStddev->Draw("same");

    std::unique_ptr<TLegend> lPLossStddev( new TLegend(0.65, 0.18, 0.85, 0.28 ) );

    scan_electron_50umSi_emy.grPLossStddevRdiff->Draw("psame");
    scan_electron_50umSi_emy.grPLossStddevRdiff->SetMarkerStyle(24);
    scan_electron_50umSi_emy.grPLossStddevRdiff->SetMarkerColor(47);
    lPLossStddev->AddEntry( scan_electron_50umSi_emy.grPLossStddevRdiff.get(), "e^{-} 50um Si, EMY", "p" );

    scan_electron_300umSi_emy.grPLossStddevRdiff->Draw("psame");
    scan_electron_300umSi_emy.grPLossStddevRdiff->SetMarkerStyle(24);
    scan_electron_300umSi_emy.grPLossStddevRdiff->SetMarkerColor(kRed);
    lPLossStddev->AddEntry( scan_electron_300umSi_emy.grPLossStddevRdiff.get(), "e^{-} 300um Si, EMY", "p" );

    scan_electron_1000umPb.grPLossStddevRdiff->Draw("psame");
    scan_electron_1000umPb.grPLossStddevRdiff->SetMarkerStyle(24);
    scan_electron_1000umPb.grPLossStddevRdiff->SetMarkerColor(kBlue);
    lPLossStddev->AddEntry( scan_electron_1000umPb.grPLossStddevRdiff.get(), "e^{-} 1000um Pb, EMY", "p" );

    scan_electron_5000umBe.grPLossStddevRdiff->Draw("psame");
    scan_electron_5000umBe.grPLossStddevRdiff->SetMarkerStyle(24);
    scan_electron_5000umBe.grPLossStddevRdiff->SetMarkerColor(kMagenta);
    lPLossStddev->AddEntry( scan_electron_5000umBe.grPLossStddevRdiff.get(), "e^{-} 5000um Be, EMY", "p" );

    scan_piminus_500umC.grPLossStddevRdiff->Draw("psame");
    scan_piminus_500umC.grPLossStddevRdiff->SetMarkerStyle(24);
    scan_piminus_500umC.grPLossStddevRdiff->SetMarkerColor(kBlack);
    lPLossStddev->AddEntry( scan_piminus_500umC.grPLossStddevRdiff.get(), "pi^{-} 500um C, EMY", "p" );

    scan_proton_1000umTa.grPLossStddevRdiff->Draw("psame");
    scan_proton_1000umTa.grPLossStddevRdiff->SetMarkerStyle(24);
    scan_proton_1000umTa.grPLossStddevRdiff->SetMarkerColor(kCyan);
    lPLossStddev->AddEntry( scan_proton_1000umTa.grPLossStddevRdiff.get(), "p^{+} 1000um Ta, EMY", "p" );

    lPLossStddev->Draw();
    cPLossStddevRDiffCombined->Write();

    // =================
    std::unique_ptr<TCanvas> cScatXRDiffCombined( new TCanvas("ScatXRdiffCombined", "ScatXRdiffCombined" ) );
    std::unique_ptr<TGraph>  scaleScatX( new TGraph() );
    scaleScatX->SetNameTitle("ScatXRdiffCombinedScale", "Relative difference of the position straggling at the detector plane");
    scaleScatX->SetPoint(0, gMomenta.front(), -0.15);
    scaleScatX->SetPoint(1, gMomenta.back(),   0.15);
    scaleScatX->Draw("ap");
    scaleScatX->GetXaxis()->SetTitle("Initial p / GeV/c");
    scaleScatX->GetYaxis()->SetTitle("(#Deltap_{GENFIT} - #Deltap_{GEANT})/#Deltap_{GEANT}");
    scaleScatX->GetYaxis()->SetTitleOffset(1.3);
    cScatXRDiffCombined->SetLogx(1);
    std::unique_ptr<TF1> zeroLineScatX( new TF1("ScatXRdiffCombinedZero", "pol0", - gMomenta.back(), 2 * gMomenta.back() ) );
    zeroLineScatX->Draw("same");

    std::unique_ptr<TLegend> lScatX( new TLegend(0.65, 0.18, 0.85, 0.38 ) );

    scan_electron_50umSi_emy.grScatXRdiff->Draw("psame");
    scan_electron_50umSi_emy.grScatXRdiff->SetMarkerStyle(24);
    scan_electron_50umSi_emy.grScatXRdiff->SetMarkerColor(47);
    lScatX->AddEntry( scan_electron_50umSi_emy.grScatXRdiff.get(), "e^{-} 50um Si", "p" );

    scan_electron_300umSi_emy.grScatXRdiff->Draw("psame");
    scan_electron_300umSi_emy.grScatXRdiff->SetMarkerStyle(24);
    scan_electron_300umSi_emy.grScatXRdiff->SetMarkerColor(kRed);
    lScatX->AddEntry( scan_electron_300umSi_emy.grScatXRdiff.get(), "e^{-} 300um Si", "p" );

    scan_electron_1000umPb.grScatXRdiff->Draw("psame");
    scan_electron_1000umPb.grScatXRdiff->SetMarkerStyle(24);
    scan_electron_1000umPb.grScatXRdiff->SetMarkerColor(kBlue);
    lScatX->AddEntry( scan_electron_1000umPb.grScatXRdiff.get(), "e^{-} 1000um Pb", "p" );

    scan_electron_5000umBe.grScatXRdiff->Draw("psame");
    scan_electron_5000umBe.grScatXRdiff->SetMarkerStyle(24);
    scan_electron_5000umBe.grScatXRdiff->SetMarkerColor(kMagenta);
    lScatX->AddEntry( scan_electron_5000umBe.grScatXRdiff.get(), "e^{-} 5000um Be", "p" );

    scan_piminus_500umC.grScatXRdiff->Draw("psame");
    scan_piminus_500umC.grScatXRdiff->SetMarkerStyle(24);
    scan_piminus_500umC.grScatXRdiff->SetMarkerColor(kBlack);
    lScatX->AddEntry( scan_piminus_500umC.grScatXRdiff.get(), "#pi^{-} 500um C", "p" );

    scan_proton_1000umTa.grScatXRdiff->Draw("psame");
    scan_proton_1000umTa.grScatXRdiff->SetMarkerStyle(24);
    scan_proton_1000umTa.grScatXRdiff->SetMarkerColor(kCyan);
    lScatX->AddEntry( scan_proton_1000umTa.grScatXRdiff.get(), "p^{+} 1000um Ta", "p" );

    lScatX->Draw();
    cScatXRDiffCombined->Write();



    fileOut.Close();
}

