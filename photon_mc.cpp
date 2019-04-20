/* compile with:
g++ photon_mc.cpp -o photon_mc.exe `root-config --cflags --glibs`
*/
#include <iostream>
#include <vector>
#include <ROOT/RDataFrame.hxx>
#include <TRandom3.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TApplication.h>

using namespace std;
using namespace ROOT;

int main(int argc, char **argv)
{
    TApplication theApp("App", &argc, argv);
    auto treename = "myTree";
    auto filename = "photon_tracks.root";
    constexpr int N = 10;
    constexpr double Radius = 1.0;
    constexpr double beta0 = TMath::DegToRad() * 60.0;
    TRandom3 R(1);
    RDataFrame photons(N);

    auto genXY = [&]() {
        return R.Uniform(1.0);
    };
    auto genTheta = [&]() {
        return TMath::ACos(R.Uniform(1.0));
    };

    auto genPhi = [&]() {
        return fmod((R.Uniform(1.0) * 2.0 * M_PI), M_PI);
    };

    photons.Define("x", genXY).Define("y", genXY).Define("r", "x*x+y*y").Define("phi", genPhi).Define("alpha", "asin(r*r*sin(phi))").Define("theta", genTheta).Define("beta", "acos(sin(theta)*cos(alpha))").Snapshot(treename, filename);

    RDataFrame photons_load(treename, filename);

    // auto augmented = photons_load.Define("r", "x*x+y*y")
    //                      .Define("phi", genPhi)
    //                      .Define("alpha", "asin((x*x+y*y)*sin(phi))")
    //                      .Define("theta", genTheta)
    //                      .Define("beta", "acos(sin(theta)*cos(alpha))");
    auto filter1 = photons_load.Filter("r<=1.0", "Radius").Filter([&](const double &b) { return b > beta0; }, {"beta"}, "beta_cut");
    // auto filter2 = filter1.Filter([&](const double &b) { return b > beta0; }, {"beta"}, "beta_cut");
    auto c2 = new TCanvas("c2", "test", 100, 100, 800, 600);
    auto myHist = filter1.Histo1D("y");
    myHist->DrawClone();
    c2->Update();
    auto counter1 = filter1.Count();
    // auto counter22 = filter1.Count();
    cout << *counter1 << endl;
    // cout << *counter22 << endl;
    auto d1 = filter1.Display("", 35);
    d1->Print();
    cout << "stats:" << endl;
    auto report = photons_load.Report();
    report->Print();
    theApp.Run();
    return 0;
}