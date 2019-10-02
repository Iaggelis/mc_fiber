import ROOT

def fill_tree(treename, filename, size):
    ROOT.ROOT.EnableImplicitMT()
    photons = ROOT.ROOT.RDataFrame(size)
    photons.Define("x", "gRandom->Uniform(0, 1)").Define("y", "gRandom->Uniform(0, 1)").Snapshot(treename, filename)
    # Disable multi-threading
    ROOT.ROOT.DisableImplicitMT()



filename = 'photons_py.root'
treename = 'myTree'
size = 1000
fill_tree(filename, treename, size)

ROOT.ROOT.EnableImplicitMT()
RDF = ROOT.ROOT.RDataFrame
photons = RDF(treename, filename)

beta0 = ROOT.TMath.DegToRad()*60

filter1 = photons.Define("r","sqrt(x*x+y*y)").Filter('r<=1.0',"Radius cut")\
                .Define("phi","gRandom->Uniform(0, 2.0*M_PI)")\
                .Define("salpha","r*sin(phi)")\
                .Define("ctheta","gRandom->Uniform()")\
                .Define("beta","acos(sqrt(2.0-ctheta*ctheta)*sqrt(1.0-salpha*salpha))").Filter('fmod(beta,2.0*M_PI)>=DegToRad()*60','Beta cut')


myHist = filter1.Histo1D({'myHist','photon yield',20,0,1},"x")
myHist.Scale(1.0/size)
myHist.Draw('L HIST')
ROOT.ROOT.DisableImplicitMT()
