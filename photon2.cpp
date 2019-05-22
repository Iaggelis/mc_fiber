/* compile with:
g++ photon2.cpp -o photon2.exe `root-config --cflags --glibs`
*/
#include <iostream>
#include <vector>
#include <ROOT/RDataFrame.hxx>
#include <TRandom3.h>
#include <TMath.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TApplication.h>

using namespace std;
using namespace ROOT;
TRandom3 R(1);
auto f1 = new TF1("f1", "sin(x)", 0, M_PI_2);

auto genXY = [&]() {
    return R.Uniform();
};
auto genTheta = [&]() {
    return R.Uniform();
    // return TMath::ACos(R.Uniform(1.0));
    // return TMath::ASin(sqrt(R.Uniform(1.0)));
    // return TMath::ACos(TMath::Sqrt(1.0 - R.Uniform(1.0)));
    // return f1->GetRandom();
};

auto genPhi = [&]() {
    return R.Uniform(2.0 * M_PI);
};

void fill_tree(const char *treename, const char *filename, const int size)
{

    ROOT::EnableImplicitMT();
    RDataFrame photons(size);
    photons.Define("x", genXY).Define("y", genXY).Snapshot(treename, filename);
    ROOT::DisableImplicitMT();
}

int main(int argc, char **argv)
{
    TApplication theApp("App", &argc, argv);
    auto treename = "myTree";
    auto filename = "photon_tracks.root";
    constexpr long N = 100000000;
    constexpr double Radius = 1.0;
    constexpr double beta0 = TMath::DegToRad() * 60.0;
    fill_tree(treename, filename, N);

    ROOT::EnableImplicitMT();

    RDataFrame photons_load(treename, filename);
    // auto augmented = photons_load.Define("lenght", "x+y");
    auto filter1 = photons_load.Define("r", "sqrt(x*x+y*y)").Filter("r<=1.0", "Radius").Define("phi", genPhi).Define("salpha", "r*sin(phi)").Define("ctheta", genTheta).Define("beta", "acos(sqrt(1.0-ctheta*ctheta)*sqrt(1.0-salpha*salpha))").Filter([&](const double &b) { return fmod(b, 2.0 * M_PI) >= beta0; }, {"beta"}, "beta_cut");
    // auto filter1 = photons_load.Define("r", "sqrt(x*x+y*y)").Filter("r*r<=1.0", "Radius").Define("phi", genPhi).Define("alpha", "asin(r*sin(phi))").Define("theta", genTheta).Define("beta", "acos(sin(theta)*cos(alpha))").Filter([&](const double &b) { return b >= beta0; }, {"beta"}, "beta_cut");
    auto counter1 = filter1.Count();
    cout << *counter1 << endl;

    // auto c2 = new TCanvas("c2", "test", 100, 100, 800, 600);

    // auto trackHist = augmented.Graph("x", "lenght");
    // auto length = augmented.Range(0, 10);
    auto myHist = filter1.Histo1D({"myHist", "photon yield", 20, 0, 1}, "x");
    myHist->Scale(1.0 / N);
    myHist->Draw("L HIST");
    // c2->Update();
    // trackHist->Sort();
    // trackHist->Draw("AP");
    ROOT::DisableImplicitMT();

    cout << "stats:" << endl;
    auto report = photons_load.Report();
    report->Print();

    // auto d1 = filter1.Display("");
    // d1->Print();

    theApp.Run();
    return 0;
}
