#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TPad.h"
#include "TStyle.h"
#include "TGraphErrors.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cstdlib>
#include "tdrstyle.C"
#include <vector>

using namespace std;

void scanRate2D(string sbase, float basecut1, int nmax1, float interval1,  float basecut2, int nmax2, float interval2, string fname, string stitle) ;
void scanRate(string sbase, float basecut, int nmax, float interval, string fname, string stitle);
pair<float, float> wrapper(string numcut1);
pair<float, float> getRate(string numcut1, float crosssection, string sourcefile);
pair<float, float> drive(string rname, float xsec, string numcut1);
/*
READ ME:
Cut string definiton = cut string 
Input test signal file is set in the main. Search 'root' in this file

Functions
(1) spotCheck(string snumcut)
It will print bg rate of cut string
See example below

(2) scanRate(string sbase, float basecut, int nmax, float interval, string fname, string stitle)
It will scan in 1D. 
See example below

(3) scanRate2D(string sbase, float basecut1, int nmax1, float interval1,  float basecut2, int nmax2, float interval2, string fname, string stitle)
It will scan in 2D
See example below
*/

void eSignalPlot(){
gROOT -> Reset();
setTDRStyle();

string sbase = "ak4genj1pt>40 & ak4genj1pt>40 & ak4genmjj > 750 & genleppt > 10 & genmet > 60 & genht > 350 & l1pt>175 & l3leppt1>8 & l3leppt2>8 & l3leppt3>10 & calomet>20 & calometID>20 & dijet>%.0f & ht>%.0f & pfmet>60";
string stitle = "; Dijet mass threshold [GeV]; H_{T} threshold [GeV]; Signal acceptance [%]";
float basecut1 = 750;
int nmax1 = 5;
float interval1 = 50;
float basecut2 = 350;
int nmax2 = 5;
float interval2 = 50;
string fname = "mjjhtSignalScanEleM.png";
scanRate2D(sbase, basecut1, nmax1, interval1, basecut2, nmax2, interval2, fname, stitle);

sbase = "ak4genj1pt>40 & ak4genj1pt>40 & ak4genmjj > 750 & genleppt > 10 & genmet > 60 & genht > 350 & l1pt>175 & l3leppt1>8 & l3leppt2>8 & l3leppt3>10 & calomet>20 & calometID>20 & dijet>750 & ht>%.0f & pfmet>%.0f";
stitle = "; H_{T} threshold [GeV]; PF MET threshold [GeV]; Signal acceptance [%]";
basecut1 = 350;
nmax1 = 5;
interval1 = 50;
basecut2 = 60;
nmax2 = 5;
interval2 = 10;
fname = "htmetSignalScanEleM.png";
scanRate2D(sbase, basecut1, nmax1, interval1, basecut2, nmax2, interval2, fname, stitle);

sbase = "ak4genj1pt>40 & ak4genj1pt>40 & ak4genmjj > 750 & genleppt > 10 & genmet > 60 & genht > 350 & l1pt>175 & l3leppt1>8 & l3leppt2>8 & l3leppt3>10 & calomet>20 & calometID>20 & dijet>%.0f & ht>350 & pfmet>%.0f";
stitle = "; Dijet mass threshold [GeV]; PF MET threshold [GeV]; Signal acceptance [%]";
basecut1 = 750;
nmax1 = 5;
interval1 = 50;
basecut2 = 60;
nmax2 = 5;
interval2 = 10;
fname = "mjjmetSignalScanEleM.png";
scanRate2D(sbase, basecut1, nmax1, interval1, basecut2, nmax2, interval2, fname, stitle);

sbase = "ak4genj1pt>40 & ak4genj1pt>40 & ak4genmjj > 750 & genleppt > 10 & genmet > 60 & genht > 350 & l1pt>175 & l3leppt1>8 & l3leppt2>8 & l3leppt3>10 & calomet>20 & calometID>20 & dijet>750 & ht>350 & pfmet>%.0f";
stitle = "; PFMET threshold [GeV]; Signal acceptance [%];";
basecut = 60;
nmax = 5;
interval = 20;
fname = "metSignalScanEleM.png";
scanRate(sbase, basecut, nmax, interval, fname, stitle);

sbase = "ak4genj1pt>40 & ak4genj1pt>40 & ak4genmjj > 750 & genleppt > 10 & genmet > 60 & genht > 350 & l1pt>175 & l3leppt1>8 & l3leppt2>8 & l3leppt3>10 & calomet>20 & calometID>20 & dijet>750 & ht>%.0f & pfmet>60";
stitle = "; H_{T} threshold [GeV]; Signal acceptance [%];";
basecut = 350;
nmax = 5;
interval = 100;
fname = "htSignalScanEleM.png";
scanRate(sbase, basecut, nmax, interval, fname, stitle);

sbase = "ak4genj1pt>40 & ak4genj1pt>40 & ak4genmjj > 750 & genleppt > 10 & genmet > 60 & genht > 350 & l1pt>175 & l3leppt1>8 & l3leppt2>8 & l3leppt3>10 & calomet>20 & calometID>20 & dijet>%.0f & ht>350 & pfmet>60";
stitle = "; Mjj threshold [GeV]; Signal acceptance [%];";
basecut = 750;
nmax = 5;
interval = 100;
fname = "mjjSignalScanEleM.png";
scanRate(sbase, basecut, nmax, interval, fname, stitle);

}

void scanRate2D(string sbase, float basecut1, int nmax1, float interval1,  float basecut2, int nmax2, float interval2, string fname, string stitle){

char buffer[256] = "";
TH2F * h2d = new TH2F("this2DHisto", stitle.c_str(), nmax1, basecut1, basecut1 + interval1 * nmax1, nmax2,  basecut2, basecut2 + interval2 * nmax2 ) ;

float thiscut1 = 0;
float thiscut2 = 0;

for( int i = 0; i < nmax1; i++){
    thiscut1 = basecut1 + (float) i * interval1;
    for( int j = 0; j < nmax2; j++){
        thiscut2 = basecut2 + (float) j * interval2;
        int n = sprintf(buffer, sbase.c_str(), thiscut1, thiscut2 );
        string scut(buffer);
        pair<float, float> temppair = wrapper (scut.c_str()) ;
        h2d -> SetBinContent(i+1, j+1, temppair.first );
    }
}

TCanvas *c3 = new TCanvas("c3","multipads",800,600);
c3 -> SetFillColor (0);
h2d -> Draw("zcol");
gStyle->SetPalette(1, 0);
gPad->SetRightMargin(0.17);
c3 -> SaveAs(fname.c_str() );
delete h2d ;
delete c3 ;
}

void scanRate(string sbase, float basecut, int nmax, float interval, string fname, string stitle){
char buffer[256] = "";
float thiscut = 0;
vector<float> vbin;
vector<float> vrate;
vector<float> verror;

for( int i = 0; i < nmax; i++){
    thiscut = basecut + i * interval;
    int n = sprintf(buffer, sbase.c_str(), thiscut );
    string scut(buffer);
    pair<float, float> temppair = wrapper (scut.c_str()) ;
    vbin.push_back(thiscut);
    vrate.push_back(temppair.first);
    verror.push_back(temppair.second);
}


TCanvas *c2 = new TCanvas("c2","multipads",800,600);
c2 -> SetFillColor (0);
TGraphErrors * tg = new TGraphErrors( nmax );
tg -> SetTitle(stitle.c_str() );
for( int i = 0; i < nmax; i++){
    tg -> SetPoint(i, vbin.at(i), vrate.at(i));
    tg -> SetPointError(i, 0, verror.at(i));
}
tg -> SetMarkerSize(2);
tg -> SetMarkerStyle(20);
tg -> Draw("ape");
c2 -> SaveAs(fname.c_str() );

delete tg ;
delete c2 ;
}

void spotCheck(string numcut1){
pair<float, float> thirate = wrapper(numcut1);
cout << " eff = " << thirate.first << " +/- " << thirate.second << endl;
}

pair<float, float> wrapper(string numcut1){
float fakexsec = 1.0;

cout << " Signal eff for " << numcut1 << endl;

pair<float, float > countError1 = getRate(numcut1, fakexsec, "signalTest.root"); // test signal file
cout << " eff = " << countError1.first << " +/- " << countError1.second << endl;
return countError1;
}

pair<float, float> getRate(string numcut1, float crosssection, string sourcefile){
pair<float, float> sumrate = drive(sourcefile.c_str(), crosssection, numcut1);
//cout << sumrate.first << " +/- " << sumrate.second << endl;
return sumrate ;
}

pair<float, float> drive(string rname, float xsec, string numcut1){
pair<float, float> thisrate ;

TFile* ftt = new TFile(rname.c_str(),"READ");
TTree* ttt = (TTree *) (ftt -> Get("vbftrig/mytree"));

string denomcut1 = "ak4genj1pt>40 & ak4genj1pt>40 & ak4genmjj > 750 & genleppt > 10 & genmet > 60 & genht > 350";

float nall = ttt->Draw("trig1", denomcut1.c_str());
float npass = ttt->Draw("trig1", numcut1.c_str());
if( npass < 0 ) npass = 0;
float rate = 0;
if( nall > 0){ 
    rate = npass / nall ;
    float error = sqrt(npass) / nall ;
    thisrate.first = rate;
    thisrate.second = error;
}
else cout << " No event passed " << endl;

ftt -> Close();
return thisrate ;
}
