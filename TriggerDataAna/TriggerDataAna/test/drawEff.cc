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

void drive(string sden, string dencut, string numcut, string caption, string fname, string leg, string rname);

void drawEff(){
string rname = "myntuple_mini.root"; // file name

gROOT -> Reset();
setTDRStyle();

// syntax: drive( variable name >> histo name (nbin, min, max), denominator cut, numerator cut, labels, plot name, legend, root file name) ;

drive("jpt2>>hden(10,0,200)","jpt1 > 50 & deta > 3.7 & mjj > 1000 & met > 100 & ht > 500 & nMu > 0 & mpt[0]>12","trig==1", "; Jet p_{T} [GeV];Efficiency;","jet2Eff.png", "MET>100 H_{T}>500 Mjj>1000 #mu p_{T}>12 ", rname);
drive("met>>hden(10,0,200)","jpt1 > 50 & jpt2 > 50 & deta > 3.7 & mjj > 1000 & ht > 500 & nMu > 0 & mpt[0]>12","trig==1", "; Missing E_{T} [GeV];Efficiency;", "metEff.png", "H_{T} > 500 GeV M_{jj} > 1000 GeV #mu p_{T} > 12 GeV ", rname);
drive("metRaw>>hden(10,0,200)","jpt1 > 50 & jpt2 > 50 & deta > 3.7 & mjj > 1000 & ht > 500 & nMu > 0 & mpt[0]>12","trig==1", "; Missing E_{T} [GeV];Efficiency;", "metRawEff.png", "H_{T} > 500 GeV M_{jj} > 1000 GeV #mu p_{T} > 12 GeV ", rname);
drive("metCalo>>hden(10,0,200)","jpt1 > 50 & jpt2 > 50 & deta > 3.7 & mjj > 1000 & ht > 500 & nMu > 0 & mpt[0]>12","trig==1", "; Missing E_{T} [GeV];Efficiency;", "metCaloEff.png", "H_{T} > 500 GeV M_{jj} > 1000 GeV #mu p_{T} > 12 GeV ", rname);

drive("ht>>hden(10,0,600)","jpt1 > 50 & jpt2 > 50 & deta > 3.7 & mjj > 1000 & met > 100 & nMu > 0 & mpt[0]>12","trig==1", "; H_{T} [GeV];Efficiency;","htEff.png", "ME_{T} > 100 GeV M_{jj} > 1000 GeV #mu p_{T} > 12 GeV ", rname);
drive("mjj>>hden(10,0,1500)","jpt1 > 50 & jpt2 > 50 & deta > 3.7 & ht > 500 & met > 100 & nMu > 0 & mpt[0]>12","trig==1", "; VBF jet pair mass [GeV];Efficiency;","mjjEff.png", "ME_{T} > 100 GeV H_{T} > 500 GeV #mu p_{T} > 12 GeV", rname);
drive("mpt[0]>>hden(10,0,100)","jpt1 > 50 & jpt2 > 50 & deta > 3.7 & mjj > 1000 & met > 100 & ht > 500 & nMu > 0","trig==1", "; Muon p_{T} [GeV];Efficiency;","muonPTEff.png", "ME_{T} > 100 GeV M_{jj} > 1000 GeV H_{T} > 500 GeV ", rname);
}

void drive(string sden, string dencut, string numcut, string caption, string fname, string leg, string rname){
TCanvas *c1 = new TCanvas("c1","multipads",800,600);
c1 -> SetFillColor (0);

TLegend *myleg = new TLegend(0.20,0.74,0.78,0.92);
myleg -> SetHeader("LHC 13TeV Run B and C period");
myleg -> SetFillStyle(0);

cout << " Start " << endl;

TFile* ftt = new TFile(rname.c_str(),"READ");

cout << " File open ..." << endl;

TTree* ttt = (TTree *) (ftt -> Get("ntupler/ElectronTree"));

cout << " Tree open ..." << endl;

float ndata = ttt->Draw(sden.c_str(), dencut.c_str() );
TH1F* hden = (TH1F*)gDirectory->Get("hden");

// prepare numerator
string snum = sden;
string sfrom = "hden";
string sto   = "hnum";
size_t start_pos = snum.find(sfrom);
if(start_pos == std::string::npos) return;
else{
	snum.replace(start_pos, sfrom.length(), sto);
}

if(dencut != ""){
	if(numcut != "") numcut += " & " + dencut;
	else numcut = dencut ;
}
else numcut = dencut;

float ndata1 = ttt->Draw(snum.c_str(), numcut.c_str());
TH1F* hnum = (TH1F*)gDirectory->Get("hnum");

cout << " Plot 1 drawn ... " << endl;

hnum->Sumw2();
hden->Sumw2();

TGraphAsymmErrors * heff = new TGraphAsymmErrors(hnum,hden);

heff -> SetTitle(caption.c_str() );
heff -> SetMarkerColor(kBlack);
heff -> SetLineColor(kBlack);
heff -> SetLineWidth(2);
heff -> SetMarkerStyle(23);
heff -> SetMarkerSize(2);
heff -> SetMinimum(0);
heff -> SetMaximum(1.4);
heff -> Draw("ap");

myleg -> AddEntry( heff,     leg.c_str(),"LEP");
myleg -> Draw();

c1 -> SaveAs(fname.c_str());
ftt -> Close();
delete c1;

cout << " Macro ends ... " << endl;
}
