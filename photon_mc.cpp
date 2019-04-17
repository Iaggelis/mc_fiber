/* compile with:
g++ photon_mc.cpp -o photon_mc.exe `root-config --cflags --glibs`
*/
#include <iostream>
#include <vector>
#include <ROOT/RDataFrame.hxx>
#include <ROOT/TRandom3.h>
#include <ROOT/TMath.h>

using namespace std;
using namespace ROOT;

int main()
{
    auto treename = "myTree";
    auto filename = "photon_tracks.root";
    constexpr int N = 10;
    constexpr double beta0 = TMath::DegToRad() * 60.0;
    TRandom3 R(1);
    RDataFrame photons(N);

    auto genXY = [&]() {
        return R.Uniform(1.0);
    };
    auto genTheta = [&]() {
        return TMath::ACos(R.Uniform(1.0));
    };

    photons.Define("x", genXY).Define("y", genXY).Snapshot(treename, filename);

    auto cuts = photons.Filter("x*x+y*y<=1").Define("phi", [&]() {
                                                            return R.Uniform(1.0) * 2.0 * M_PI;
                                                        })
                    .Define("alpha", "asin((x*x+y*y)*sin(phi))")
                    .Define("theta", genTheta)
                    .Define("beta", "acos(sin(theta)*cos(alpha))")
                    .Filter([](double b) { return b > beta0; }, {"beta"});
    return 0;
}