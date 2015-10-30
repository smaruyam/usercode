#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TPad.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include "tdrstyle.C"

using namespace std;

void drive(string sden, string dencut, string numcut, string caption, string fname, string leg, string rname, int ibins, float xmin, float xmax, bool isLog);

void  CMS_lumi( TPad* pad, int iPeriod, int iPosX , string);

void drawEff(){
//string rname = "newRunDOct05.root"; // file name
string rname = "New25ns.root"; // file name

gROOT -> Reset();
// syntax: drive( variable name >> histo name (nbin, min, max), denominator cut, numerator cut, labels, plot name, legend, root file name) ;

drive("jpt2","jpt1 > 120 & deta > 3.7 & mjj > 1200 & met > 120 & ht > 600 & nMu > 0 & mpt[0]>12","trig==1", "; Offline Jet p_{T} [GeV];Efficiency (muon + VBF jets trigger);","jet2Eff.png", "Denom: E_{T}^{miss} > 120, H_{T} > 600, M_{jj} > 1200, p_{T}^{#mu} > 12 GeV ", rname, 10, 0, 500, true);

drive("met","jpt1 > 120 & jpt2 > 120 & deta > 3.7 & mjj > 1200 & ht > 600 & nMu > 0 & mpt[0]>12","trig==1", "; Offline E_{T}^{miss} [GeV];Efficiency (muon + VBF jets trigger);", "met1Eff.png", "Denom: H_{T} > 600, M_{jj} > 1200, p_{T}^{#mu} > 12 GeV ", rname, 10, 0, 500, true);
drive("ht","jpt1 > 50 & jpt2 > 50 & deta > 3.7 & mjj > 1200 & met > 120 & nMu > 0 & mpt[0]>12","trig==1", "; Offline H_{T} [GeV];Efficiency (muon + VBF jets trigger);","htEff.png", "Denom: E_{T}^{miss} > 120, M_{jj} > 1200, p_{T}^{#mu} > 12 GeV ", rname, 10, 0, 1000, true);
drive("mjj","jpt1 > 50 & jpt2 > 50 & deta > 3.7 & ht > 600 & met > 120 & nMu > 0 & mpt[0]>12","trig==1", "; Offline M_{jj} [GeV];Efficiency (muon + VBF jets trigger);","mjjEff.png", "Denom: E_{T}^{miss} > 120, H_{T} > 600, p_{T}^{#mu} > 12 GeV", rname, 10, 0 , 2000, true);
drive("mpt[0]","jpt1 > 120 & jpt2 > 120 & deta > 3.7 & mjj > 1200 & met > 120 & ht > 600 & nMu > 0","trig==1", "; Offline Muon p_{T} [GeV];Efficiency (muon + VBF jets trigger);","muonPTEff.png", "Denom: E_{T}^{miss} > 120, M_{jj} > 1200, H_{T} > 600 GeV ", rname, 10, 0, 100, false);
}

void drive(string sden, string dencut, string numcut, string caption, string fname, string leg, string rname, int ibins, float xmin, float xmax, bool isLog){
TCanvas *c1 = new TCanvas("c1","multipads",800,600);
c1 -> SetFillColor (0);

setTDRStyle();

string lumi = "1.28";
int iPeriod = 4;    // 1=7TeV, 2=8TeV, 3=7+8TeV, 4=13TeV, 7=7+8+13TeV 
int iPos = 11; // top left

TPad *pad1 = new TPad("pad1","",0,0,1,1);
TPad *pad2 = new TPad("pad2","",0,0,1,1);

pad2->SetFillStyle(4000); //will be transparent
pad1->Draw();
pad1->cd();

TLegend *myleg = new TLegend(0.14,0.78,0.84,0.90);
myleg -> SetFillStyle(0);
myleg -> SetBorderSize(0);
myleg -> SetTextFont(42);

cout << " Start " << endl;

TFile* ftt = new TFile(rname.c_str(),"READ");

cout << " File open ..." << endl;

TTree* ttt = (TTree *) (ftt -> Get("ntupler/ElectronTree"));

cout << " Tree open ..." << endl;

std::stringstream sstm;
sstm << sden << ">>hden(" << ibins << "," << xmin << "," << xmax << ")" ;
string formden = sstm.str();

cout << " Denominator cut string = " << formden << endl;

float ndata = ttt->Draw(formden.c_str(), dencut.c_str() );
TH1F* hden = (TH1F*)gDirectory->Get("hden");

// prepare numerator
string snum = formden;
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

if(isLog){
	hden ->  SetMaximum( hden -> GetMaximum() * 60 );
	hnum ->  SetMaximum( hden -> GetMaximum() * 60 );
}
else{
	hden ->  SetMaximum( hden -> GetMaximum() * 1.6 );
	hnum ->  SetMaximum( hden -> GetMaximum() * 1.6);
}

hden ->  GetXaxis() -> SetRangeUser(xmin, xmax);
hnum ->  GetXaxis() -> SetRangeUser(xmin, xmax);

hden -> SetLineColor(kBlue+2);
hden -> SetLineWidth(2);
hden -> SetLineStyle(3);

hnum -> SetLineColor(38);
hnum -> SetLineWidth(1);
hnum -> SetFillStyle(1001);
hnum -> SetFillColor(38);

// prepare a string for events binning
sstm << " ; ; Events / " << (xmax - xmin) / ibins << " GeV;" ;
string rtitle = sstm.str();
hden -> SetTitle(rtitle.c_str() );

hden -> Draw("HIST Y+");
hnum -> Draw("HIST same");

if(isLog){
	pad1 -> SetLogy(1);
}
else{
	pad1 -> SetLogy(0);
}

pad1->Modified();
c1->cd();

pad2->Draw();
pad2->cd();
pad2 -> SetLogy(0);

TGraphAsymmErrors * heff = new TGraphAsymmErrors(hnum,hden);

heff -> SetTitle(caption.c_str() );
heff -> SetMarkerColor(kBlack);
heff -> SetLineColor(kBlack);
heff -> SetLineWidth(2);
heff -> SetMarkerStyle(20);
heff -> SetMarkerSize(1.4);
heff -> SetMinimum(0);
heff -> SetMaximum(1.4);
heff ->  GetXaxis() -> SetLimits(xmin, xmax);
heff -> Draw("ap");

TF1 *fa1 = new TF1("fa1","1.0",xmin,xmax);
fa1 -> SetLineColor (kGray) ;
fa1 -> SetLineWidth (2) ;
fa1 -> SetLineStyle (2) ;
fa1 -> SetFillColor (kGray) ;
fa1 -> Draw("same");

myleg -> AddEntry( hden,     leg.c_str(),"F");
myleg -> AddEntry( hnum,     "Num: Denom + (muon + VBF jets trigger)","F");
myleg -> AddEntry( heff,     "Efficiency","LEP");
myleg -> Draw();

pad2->Update();

CMS_lumi(pad2,iPeriod,0, lumi);
pad2->Update();

c1 -> SaveAs(fname.c_str());

delete c1;
ftt -> Close();

cout << " Macro ends ... " << endl;
}


void 
CMS_lumi( TPad* pad, int iPeriod, int iPosX, string lumi )
{            
TString cmsText     = "CMS";
float cmsTextFont   = 61;  // default is helvetic-bold

bool writeExtraText = true;
TString extraText   = "Preliminary";
float extraTextFont = 52;  // default is helvetica-italics

// text sizes and text offsets with respect to the top frame
// in unit of the top margin size
float lumiTextSize     = 0.6;
float lumiTextOffset   = 0.2;
float cmsTextSize      = 0.75;
float cmsTextOffset    = 0.1;  // only used in outOfFrame version

float relPosX    = 0.045;
float relPosY    = 0.035;
float relExtraDY = 1.2;

// ratio of "CMS" and extra text size
float extraOverCmsTextSize  = 0.76;

TString lumi_13TeV = lumi + " fb^{-1}";
//TString lumi_8TeV  = "19.7 fb^{-1}";
TString lumi_8TeV  = "18.5 fb^{-1}";
TString lumi_7TeV  = "5.1 fb^{-1}";

// set lumi style
  int H_ref = 600; 
  int W_ref = 800; 
  // references for T, B, L, R
  //   float 
  float T = 0.08*H_ref;
  float B = 0.12*H_ref; 
  float L = 0.12*W_ref;
  float R = 0.04*W_ref;
//
  bool outOfFrame    = false;
  if( iPosX/10==0 ) 
    {
      outOfFrame = true;
    }
  int alignY_=3;
  int alignX_=2;
  if( iPosX/10==0 ) alignX_=1;
  if( iPosX==0    ) alignX_=1;
  if( iPosX==0    ) alignY_=1;
  if( iPosX/10==1 ) alignX_=1;
  if( iPosX/10==2 ) alignX_=2;
  if( iPosX/10==3 ) alignX_=3;
  if( iPosX == 0  ) relPosX = 0.12;
  int align_ = 10*alignX_ + alignY_;

  float l = pad->GetLeftMargin();
  float t = pad->GetTopMargin();
  float r = pad->GetRightMargin();
  float b = pad->GetBottomMargin();

  pad->cd();

  TString lumiText;
  if( iPeriod==1 )
    {
      lumiText += lumi_7TeV;
      lumiText += " (7 TeV)";
    }
  else if ( iPeriod==2 )
    {
      lumiText += lumi_8TeV;
      lumiText += " (8 TeV)";
    }
  else if( iPeriod==3 ) 
    {
      lumiText = lumi_8TeV; 
      lumiText += " (8 TeV)";
      lumiText += " + ";
      lumiText += lumi_7TeV;
      lumiText += " (7 TeV)";
    }
  else if ( iPeriod==4 )
    {
      lumiText += lumi_13TeV;
      lumiText += " (13 TeV)";
    }
  else if ( iPeriod==7 )
    { 
      if( outOfFrame ) lumiText += "#scale[0.85]{";
      lumiText += lumi_13TeV; 
      lumiText += " (13 TeV)";
      lumiText += " + ";
      lumiText += lumi_8TeV; 
      lumiText += " (8 TeV)";
      lumiText += " + ";
      lumiText += lumi_7TeV;
      lumiText += " (7 TeV)";
      if( outOfFrame) lumiText += "}";
    }
  else if ( iPeriod==12 )
    {
      lumiText += "8 TeV";
    }
   
  std::cout << lumiText << endl;

  TLatex latex;
  latex.SetNDC();
  latex.SetTextAngle(0);
  latex.SetTextColor(kBlack);    

  float extraTextSize = extraOverCmsTextSize*cmsTextSize;

  latex.SetTextFont(42);
  latex.SetTextAlign(31); 
  latex.SetTextSize(lumiTextSize*t);    
  latex.DrawLatex(1-r,1-t+lumiTextOffset*t,lumiText);

  if( outOfFrame )
    {
      latex.SetTextFont(cmsTextFont);
      latex.SetTextAlign(11); 
      latex.SetTextSize(cmsTextSize*t);    
      latex.DrawLatex(l,1-t+lumiTextOffset*t,cmsText);
    }
  
  pad->cd();

  float posX_=0;
  if( iPosX%10<=1 )
    {
      posX_ =   l + relPosX*(1-l-r);
    }
  else if( iPosX%10==2 )
    {
      posX_ =  l + 0.5*(1-l-r);
    }
  else if( iPosX%10==3 )
    {
      posX_ =  1-r - relPosX*(1-l-r);
    }
  float posY_ = 1-t - relPosY*(1-t-b);
  if( !outOfFrame )
    {
	  latex.SetTextFont(cmsTextFont);
	  latex.SetTextSize(cmsTextSize*t);
	  latex.SetTextAlign(align_);
	  latex.DrawLatex(posX_, posY_, cmsText);
	  if( writeExtraText ) 
	    {
	      latex.SetTextFont(extraTextFont);
	      latex.SetTextAlign(align_);
	      latex.SetTextSize(extraTextSize*t);
	      latex.DrawLatex(posX_, posY_- relExtraDY*cmsTextSize*t, extraText);
	    }
    }
  else if( writeExtraText )
    {
      if( iPosX==0) 
	{
	  posX_ =   l +  relPosX*(1-l-r);
	  posY_ =   1-t+lumiTextOffset*t;
	}
      latex.SetTextFont(extraTextFont);
      latex.SetTextSize(extraTextSize*t);
      latex.SetTextAlign(align_);
      latex.DrawLatex(posX_, posY_, extraText);      
    }
  return;
}
