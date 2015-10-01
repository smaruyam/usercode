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
pair<float, float> addInQuad( pair<float, float > countError, pair<float, float > tmpCountError );
pair<float, float> getRate(string numcut1, float crosssection, string sourcefile);
pair<float, float> drive(string rname, float xsec, string numcut1);
/*
READ ME:
Cut string definiton = cut string 
Input BG files already set in the main. Search 'root' in this file
QCD pT hat from 30 to 1000, W+jets, Z+jets

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

void eBgRate(){
gROOT -> Reset();
setTDRStyle();

string sbase = "l1pt>175 & l3leppt1>8 & l3leppt2>8 & l3leppt3>10 & calomet>20 & calometID>20 & dijet>%.0f & ht>%.0f & pfmet>60";
string stitle = "; Dijet mass threshold [GeV]; H_{T} threshold [GeV]; Background rate [Hz]";
float basecut1 = 750;
int nmax1 = 5;
float interval1 = 50;
float basecut2 = 350;
int nmax2 = 5;
float interval2 = 50;
string fname = "mjjhtScanEleM.png";
scanRate2D(sbase, basecut1, nmax1, interval1, basecut2, nmax2, interval2, fname, stitle);

sbase = "l1pt>175 & l3leppt1>8 & l3leppt2>8 & l3leppt3>10 & calomet>20 & calometID>20 & dijet>750 & ht>%.0f & pfmet>%.0f";
stitle = "; H_{T} threshold [GeV]; PF MET threshold [GeV]; Background rate [Hz]";
basecut1 = 350;
nmax1 = 5;
interval1 = 50;
basecut2 = 60;
nmax2 = 5;
interval2 = 10;
fname = "htmetScanEleM.png";
scanRate2D(sbase, basecut1, nmax1, interval1, basecut2, nmax2, interval2, fname, stitle);

sbase = "l1pt>175 & l3leppt1>8 & l3leppt2>8 & l3leppt3>10 & calomet>20 & calometID>20 & dijet>%.0f & ht>350 & pfmet>%.0f";
stitle = "; Dijet mass threshold [GeV]; PF MET threshold [GeV]; Background rate [Hz]";
basecut1 = 750;
nmax1 = 5;
interval1 = 50;
basecut2 = 60;
nmax2 = 5;
interval2 = 10;
fname = "mjjmetScanEleM.png";
scanRate2D(sbase, basecut1, nmax1, interval1, basecut2, nmax2, interval2, fname, stitle);

sbase = "l1pt>175 & l3leppt1>8 & l3leppt2>8 & l3leppt3>10 & calomet>20 & calometID>20 & dijet>750 & ht>350 & pfmet>%.0f";
stitle = "; PFMET threshold [GeV]; Background rate [Hz];";
basecut = 60;
nmax = 5;
interval = 20;
fname = "metScanEleM.png";
scanRate(sbase, basecut, nmax, interval, fname, stitle);

sbase = "l1pt>175 & l3leppt1>8 & l3leppt2>8 & l3leppt3>10 & calomet>20 & calometID>20 & dijet>750 & ht>%.0f & pfmet>60";
stitle = "; H_{T} threshold [GeV]; Background rate [Hz];";
basecut = 350;
nmax = 5;
interval = 100;
fname = "htScanEleM.png";
scanRate(sbase, basecut, nmax, interval, fname, stitle);

sbase = "l1pt>175 & l3leppt1>8 & l3leppt2>8 & l3leppt3>10 & calomet>20 & calometID>20 & dijet>%.0f & ht>350 & pfmet>60";
stitle = "; Mjj threshold [GeV]; Background rate [Hz];";
basecut = 750;
nmax = 5;
interval = 100;
fname = "mjjScanEleM.png";
scanRate(sbase, basecut, nmax, interval, fname, stitle);

}

void scanRate2D(string sbase, float basecut1, int nmax1, float interval1,  float basecut2, int nmax2, float interval2, string fname, string stitle){

char buffer[128] = "";
TH2F * h2d = new TH2F("this2DHisto", stitle.c_str(), nmax1, basecut1, basecut1 + interval1 * nmax1, nmax2,  basecut2, basecut2 + interval2 * nmax2 ) ;

float thiscut1 = 0;
float thiscut2 = 0;

for( int i = 0; i < nmax1; i++){
    thiscut1 = basecut1 + i * interval1;
    for( int j = 0; j < nmax2; j++){
        thiscut2 = basecut2 + j * interval2;
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
char buffer[128] = "";
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
cout << " rate = " << thirate.first << " +/- " << thirate.second << " Hz " << endl;
}

pair<float, float> wrapper(string numcut1){
//rate_mc_simple  =   (0.014  Hz/pb)  x   (xs_pb) x   (pass_fracIon)  ;
float WJMuNu       = 16100; // wj to munu
float DYJLL        = 6870 * 3; // dy to mumu times three
float QCD030to050  = 161500000 ;//    pb
float QCD050to080  = 22110000 ;// pb
float QCD080to120  = 3000000 ;// pb
float QCD120to170  = 493200 ;//   pb
float QCD170to300  = 120300 ;//   pb
float QCD300to470  = 7475 ;// pb
float QCD470to600  = 587.1 ;// pb
float QCD600to800  = 167.0 ;// pb
float QCD800to1000 = 28.2 ;//  pb

cout << " Background rate for " << numcut1 << endl;

pair<float, float > countError1 = getRate(numcut1, QCD050to080, "QCD50_80.root"); 
pair<float, float > tmpCountError = getRate(numcut1, QCD080to120, "QCD80_120.root"); 
pair<float, float > countError2 = addInQuad(countError1, tmpCountError);
tmpCountError = getRate(numcut1, QCD120to170, "QCD120_170.root"); 
pair<float, float > countError3 = addInQuad(countError2, tmpCountError);
tmpCountError = getRate(numcut1, QCD170to300, "QCD170_300.root"); 
pair<float, float > countError4 = addInQuad(countError3, tmpCountError);
tmpCountError = getRate(numcut1, QCD300to470, "QCD300_470.root"); 
pair<float, float > countError5 = addInQuad(countError4, tmpCountError);
tmpCountError = getRate(numcut1, QCD470to600, "QCD470_600.root"); 
pair<float, float > countError6 = addInQuad(countError5, tmpCountError);
tmpCountError = getRate(numcut1, QCD600to800, "QCD600_800.root"); 
pair<float, float > countError7 = addInQuad(countError6, tmpCountError);
tmpCountError = getRate(numcut1, QCD800to1000, "QCD800_1000.root"); 
pair<float, float > countError8 = addInQuad(countError7, tmpCountError);
tmpCountError = getRate(numcut1, QCD030to050, "QCD30_50.root"); 
pair<float, float > countError9 = addInQuad(countError8, tmpCountError);
tmpCountError = getRate(numcut1, WJMuNu, "WJ.root"); 
pair<float, float > countError10= addInQuad(countError9, tmpCountError);
tmpCountError = getRate(numcut1, DYJLL, "DYJ.root"); 
pair<float, float > countError11= addInQuad(countError10, tmpCountError);
cout << " rate = " << countError11.first << " +/- " << countError11.second << " Hz " << endl;
return countError11;
}

pair<float, float> addInQuad( pair<float, float > countError, pair<float, float > tmpCountError ){
pair<float, float > newCountError;
newCountError.first = countError.first + tmpCountError.first;
newCountError.second = sqrt( countError.second*countError.second + tmpCountError.second * tmpCountError.second);
return newCountError;
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

float nall = ttt -> GetEntries();

float npass = ttt->Draw("trig1", numcut1.c_str());
if( npass < 0 ) npass = 0;
float rate = 0;
if( nall > 0){ 
    rate = (0.014)  * (xsec) *  (npass / nall)  ;
    float error = (0.014)  * (xsec) *  (sqrt(npass) / nall) ;
    thisrate.first = rate;
    thisrate.second = error;
}
else cout << " No event passed " << endl;

ftt -> Close();
return thisrate ;
}
