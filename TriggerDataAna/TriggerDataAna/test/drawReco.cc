#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TPad.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cstdlib>
#include "tdrstyle.C"

using namespace std;

void drive(string hden1, string dencut1, string caption, string fname, string rname, bool ); 

void drawReco(){
string rname = "myntuple_mini.root"; // file name

gROOT -> Reset();
setTDRStyle();

// syntax: drive( variable name >> histo name (nbin, min, max), cut, labels, plot name, root file name) ;
drive("jpt>>hden(25,0,100)","jpt>20 & nJet>0", "; Jet p_{T} [GeV];a.u.;", "jetPt.png", rname, false);
drive("jpt1>>hden(25,0,200)","deta > 3.5", "; Jet p_{T} [GeV];a.u.;", "jet1Pt.png", rname, false);

drive("pt>>hden(25,0,100)","pt>10 & nEle>0", "; Electron p_{T} [GeV];a.u.;", "electronPt.png", rname, false);
drive("mpt>>hden(25,0,100)","mpt>10 & nMu>0", "; Muon p_{T} [GeV];a.u.;", "muonPt.png", rname, false);

drive("met>>hden(25,0,200)","met>0", "; MET [GeV];a.u.;",   "met.png", rname, false);
drive("metRaw>>hden(25,0,200)","metRaw>0", "; MET [GeV];a.u.;",   "metRaw.png", rname, false);
drive("metCalo>>hden(25,0,200)","metCalo>0", "; MET [GeV];a.u.;",   "metCalo.png", rname, false);

drive("ht>>hden(25,0,500)","ht>0", "; H_{T} [GeV];a.u.;", "ht.png", rname, true);
drive("mjj>>hden(25,0,2500)","deta>3.5 & jpt1>40 & jpt2 > 40 ", "; Mjj [GeV];a.u.;", "mjj.png", rname, true);
drive("jeta1>>hden(25,-5,5)","jpt1>40 & jpt2>40", "; Jet #eta;a.u.;", "jet_Eta1.png", rname, false);
drive("jeta2>>hden(25,-5,5)","jpt1>40 & jpt2>40", "; Jet #eta;a.u.;", "jet_Eta2.png", rname, false);

drive("jeta>>hden(25,-5,5)","jpt>20 & nJet>0", "; Jet #eta;a.u.;", "jet20Eta.png", rname, false);
drive("jeta>>hden(25,-5,5)","jpt>30 & nJet>0", "; Jet #eta;a.u.;", "jet30Eta.png", rname, false);
drive("jeta>>hden(25,-5,5)","jpt>40 & nJet>0", "; Jet #eta;a.u.;", "jet40Eta.png", rname, false);

drive("eta>>hden(25,-3.5,3.5)","pt>10 & nEle>0", "; Electron #eta;a.u.;", "electronEta.png", rname, false);
drive("meta>>hden(25,-3.5,3.5)","mpt>10 & nMu>0", "; Muon #eta;a.u.;", "muonEta.png", rname, false);
drive("hltmet>>hden(25,0,100)","hltmet>0 ", "; HLT MET [GeV];a.u.;", "hltmet.png", rname, false);
}

void drive(string hden1, string dencut1, string caption, string fname, string rname, bool blog ){
TCanvas *c1 = new TCanvas("c1","multipads",800,600);
c1 -> SetFillColor (0);

cout << " Start " << endl;

TFile* ftt = new TFile(rname.c_str(),"READ");

cout << " File open ..." << endl;

TTree* ttt = (TTree *) (ftt -> Get("ntupler/ElectronTree"));

cout << " Tree open ..." << endl;

float ndata = ttt->Draw(hden1.c_str(), dencut1.c_str() );
TH1F* hden = (TH1F*)gDirectory->Get("hden");

cout << " Plot 1 drawn ... with nevents = " << ndata << endl;

hden -> SetTitle(caption.c_str() );
hden->Sumw2();

hden -> SetMarkerColor(kBlack);
hden -> SetLineColor(kBlack);
hden -> SetLineWidth(2);
hden -> SetMarkerStyle(23);
hden -> SetMarkerSize(2);

hden -> DrawNormalized("ep");
if(blog) gPad -> SetLogy(1);
c1 -> SaveAs(fname.c_str());
ftt -> Close();

cout << " Macro ends ... " << endl;
delete c1;
}
