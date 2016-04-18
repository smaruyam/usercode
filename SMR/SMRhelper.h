#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "TLorentzVector.h"

/*
File: SMRhelper.h
Author: Sho Maruyama (Fermilab)
Date: April 18 2016
Description: Stochastic mass reconstruction implementation for CMS software in miniAOD format. For detail, see http://arxiv.org/abs/1512.04842.
Algorithm: Two algorithms are implemented: (1) for a pair of visible taus with large delta phi, and (2) for any pair of visible taus. While former combines two sets of daughters, latter doesn't combine them.
Note: SMR works out-of-the-box when heavy particles are truely heavy w.r.t. tau pT offline selection. When this is not the case, calibration shall be made with MC samples and/or data control regions, as the selection bias distorts estimated mass distribution. For example, we can use diagonal elements to calibrate off-diagonal elements; ie, calculating offsets in nDaughter1 = X = nDaughter2, and then using these offsets for pairs with nDaughter1 != nDaughters2. Repeat the similar sequence for electrons and muons.  
*/

class SMR {
	float smr;
	int fdc1;
	int fdc2;
	unsigned int indexTau;
	unsigned int indexElectronAndMuon;
	float calibrationTau[10];
	float calibrationElectronAndMuon[2];
	bool calibration;
	int algorithm;
	float dPhiCut;
	public:
		SMR();// : indexTau(10), indexElectronAndMuon(2), calibrationTau {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0}, calibrationElectronAndMuon {1.0, 1.0} {};
		SMR(bool flag, int algo, float dphi);// : indexTau(10), indexElectronAndMuon(2), calibrationTau {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0}, calibrationElectronAndMuon {1.0, 1.0} {};
		void estimateMass(pat::Tau, pat::Tau);
		void estimateMass(pat::Tau, pat::Electron);
		void estimateMass(pat::Tau, pat::Muon);
		void estimateMass(pat::Electron, pat::Muon);
		void estimateMass(pat::Muon, pat::Muon);
		void estimateMass(pat::Electron, pat::Electron);
		void setDphiCut(float cut){ dPhiCut = cut;}
		void setCalibrationFlag(bool flag){ calibration = flag;}
		void setCalibrationTau(unsigned int index, float newCalibration){ if(index < indexTau) calibrationTau[index] = newCalibration;}
		void setCalibrationElectronAndMuon(unsigned int index, float newCalibration){ if(index < indexElectronAndMuon) calibrationElectronAndMuon[index] = newCalibration;}
		void setAlgorithm(int algo){ algorithm = algo;}
		float getMass(){return smr;}
		float getNdaughter1(){return fdc1;}
		float getNdaughter2(){return fdc2;}
};

SMR::SMR() : smr(-1), fdc1(-1), fdc2(-1), indexTau(10), indexElectronAndMuon(2), calibrationTau {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0}, calibrationElectronAndMuon {1.0, 1.0} {
	calibration = false;
	algorithm = 1;
	dPhiCut = 0;
}

SMR::SMR(bool flag, int algo, float dphi) : smr(-1), fdc1(-1), fdc2(-1), indexTau(10), indexElectronAndMuon(2), calibrationTau {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0}, calibrationElectronAndMuon {1.0, 1.0} {
	calibration = flag;
	algorithm = algo;
	dPhiCut = dphi;
}

void SMR::estimateMass(pat::Tau tau1, pat::Tau tau2){
	float dphi = fabs( deltaPhi(tau1 . phi(), tau2 . phi() ) );
	if(dphi < dPhiCut){
		smr = -1;
		return;
	}

	int patdc1 = tau1 . decayMode() ;
	int dCharge = (patdc1 / 5) + 1;
	int dNeutral = patdc1 % 5;
	fdc1 = dCharge + dNeutral ;
	int patdc2 = tau2 . decayMode() ;
	dCharge = (patdc2 / 5) + 1;
	dNeutral = patdc2 % 5;
	fdc2 = dCharge + dNeutral ;
	if(algorithm == 1){ // back-to-back in phi plane. Let's combine two sets of tau daughters
		TLorentzVector* ltau1 = new TLorentzVector( tau1 . px(), tau1 . py(), tau1 . pz(), tau1 . energy());
		TLorentzVector* ltau2 = new TLorentzVector( tau2 . px(), tau2 . py(), tau2 . pz(), tau2 . energy());
		TLorentzVector lvis = (*ltau1 + *ltau2 );
		float betaz = lvis . BoostVector().Z();
		ltau1 -> Boost(0, 0, -(betaz) );
		ltau2 -> Boost(0, 0, -(betaz) );
		float tau1cp = ltau1 -> E();
		float tau2cp = ltau2 -> E();
		float mean1 = tau1cp * (fdc1 + 1);			
		float mean2 = tau2cp * (fdc2 + 1);			
		if(calibration == true){
			mean1 *= calibrationTau[fdc1 - 1];
			mean2 *= calibrationTau[fdc2 - 1];
		}
		smr = (mean1 + mean2) / (fdc1 + fdc2) * 2.0 ;
		delete ltau1;
		delete ltau2;
	}
	else if(algorithm == 2){ // not necessarily back-to-back in phi plane. Here we cannot combine daughters.
		float mvis = ( tau1 . p4() + tau2 . p4() ). mass();
		mvis *= sqrt( (fdc1 + 1) * (fdc2 + 1) );			
		if(calibration == true){
			mvis *= sqrt( calibrationTau[fdc1 - 1] );
			mvis *= sqrt( calibrationTau[fdc2 - 1] );
		}
		smr = mvis;
	}
}

void SMR::estimateMass(pat::Tau tau1, pat::Muon muon){
	float dphi = fabs( deltaPhi(tau1 . phi(), muon . phi() ) );
	if(dphi < dPhiCut){
		smr = -1;
		return;
	}

	int patdc1 = tau1 . decayMode() ;
	int dCharge = (patdc1 / 5) + 1;
	int dNeutral = patdc1 % 5;
	fdc1 = dCharge + dNeutral ;
	fdc2 = 1 ;
	if(algorithm == 1){ // back-to-back in phi plane
		TLorentzVector* ltau1 = new TLorentzVector( tau1 . px(), tau1 . py(), tau1 . pz(), tau1 . energy());
		TLorentzVector* lmuon = new TLorentzVector( muon . px(), muon . py(), muon . pz(), muon . energy());
		TLorentzVector lvis = (*ltau1 + *lmuon );
		float betaz = lvis . BoostVector().Z();
		ltau1 -> Boost(0, 0, -(betaz) );
		lmuon -> Boost(0, 0, -(betaz) );
		float tau1cp = ltau1 -> E();
		float muoncp = lmuon -> E();
		float mean1 = tau1cp * (fdc1 + 1);			
		float mean2 = muoncp * (fdc2 + 2);			
		if(calibration == true){
			mean1 *= calibrationTau[fdc1 - 1];
			mean2 *= calibrationElectronAndMuon[1]; // mapping hard-corded
		}
		smr = (mean1 + mean2) / (fdc1 + fdc2) * 2.0 ;
		delete ltau1;
		delete lmuon;
	}
	else if(algorithm == 2){ // not necessarily back-to-back in phi plane
		float mvis = ( tau1 . p4() + muon . p4() ). mass();
		mvis *= sqrt( (fdc1 + 1) * (fdc2 + 2) );			
		if(calibration == true){
			mvis *= sqrt( calibrationTau[fdc1 - 1] );
			mvis *= sqrt( calibrationElectronAndMuon[1] );
		}
		smr = mvis;
	}
}

void SMR::estimateMass(pat::Tau tau1, pat::Electron elec){
	float dphi = fabs( deltaPhi(tau1 . phi(), elec . phi() ) );
	if(dphi < dPhiCut){
		smr = -1;
		return;
	}

	int patdc1 = tau1 . decayMode() ;
	int dCharge = (patdc1 / 5) + 1;
	int dNeutral = patdc1 % 5;
	fdc1 = dCharge + dNeutral ;
	fdc2 = 1 ;
	if(algorithm == 1){ // back-to-back in phi plane
		TLorentzVector* ltau1 = new TLorentzVector( tau1 . px(), tau1 . py(), tau1 . pz(), tau1 . energy());
		TLorentzVector* lelec = new TLorentzVector( elec . px(), elec . py(), elec . pz(), elec . energy());
		TLorentzVector lvis = (*ltau1 + *lelec );
		float betaz = lvis . BoostVector().Z();
		ltau1 -> Boost(0, 0, -(betaz) );
		lelec -> Boost(0, 0, -(betaz) );
		float tau1cp = ltau1 -> E();
		float eleccp = lelec -> E();
		float mean1 = tau1cp * (fdc1 + 1);			
		float mean2 = eleccp * (fdc2 + 2);			
		if(calibration == true){
			mean1 *= calibrationTau[fdc1 - 1];
			mean2 *= calibrationElectronAndMuon[0]; // mapping hard-corded
		}
		smr = (mean1 + mean2) / (fdc1 + fdc2) * 2.0 ;
		delete ltau1;
		delete lelec;
	}
	else if(algorithm == 2){ // not necessarily back-to-back in phi plane
		float mvis = ( tau1 . p4() + elec . p4() ). mass();
		mvis *= sqrt( (fdc1 + 1) * (fdc2 + 2) );			
		if(calibration == true){
			mvis *= sqrt( calibrationTau[fdc1 - 1] );
			mvis *= sqrt( calibrationElectronAndMuon[0] );
		}
		smr = mvis;
	}
}

void SMR::estimateMass(pat::Electron elec1, pat::Electron elec2){
	float dphi = fabs( deltaPhi(elec1 . phi(), elec2 . phi() ) );
	if(dphi < dPhiCut){
		smr = -1;
		return;
	}

	fdc1 = 1;
	fdc2 = 1 ;
	if(algorithm == 1){ // back-to-back in phi plane
		TLorentzVector* lelec1 = new TLorentzVector( elec1 . px(), elec1 . py(), elec1 . pz(), elec1 . energy());
		TLorentzVector* lelec2 = new TLorentzVector( elec2 . px(), elec2 . py(), elec2 . pz(), elec2 . energy());
		TLorentzVector lvis = (*lelec1 + *lelec2 );
		float betaz = lvis . BoostVector().Z();
		lelec1 -> Boost(0, 0, -(betaz) );
		lelec2 -> Boost(0, 0, -(betaz) );
		float elec1cp = lelec1 -> E();
		float elec2cp = lelec2 -> E();
		float mean1 = elec1cp * (fdc1 + 2);			
		float mean2 = elec2cp * (fdc2 + 2);			
		if(calibration == true){
			mean1 *= calibrationElectronAndMuon[0]; // mapping hard-corded
			mean2 *= calibrationElectronAndMuon[0]; // mapping hard-corded
		}
		smr = (mean1 + mean2) / (fdc1 + fdc2) * 2.0 ;
		delete lelec1;
		delete lelec2;
	}
	else if(algorithm == 2){ // not necessarily back-to-back in phi plane
		float mvis = ( elec1 . p4() + elec2 . p4() ). mass();
		mvis *= sqrt( (fdc1 + 2) * (fdc2 + 2) );			
		if(calibration == true){
			mvis *= calibrationElectronAndMuon[0] ;
		}
		smr = mvis;
	}
}

void SMR::estimateMass(pat::Muon muon1, pat::Muon muon2){
	float dphi = fabs( deltaPhi(muon1 . phi(), muon2 . phi() ) );
	if(dphi < dPhiCut){
		smr = -1;
		return;
	}

	fdc1 = 1;
	fdc2 = 1 ;
	if(algorithm == 1){ // back-to-back in phi plane
		TLorentzVector* lmuon1 = new TLorentzVector( muon1 . px(), muon1 . py(), muon1 . pz(), muon1 . energy());
		TLorentzVector* lmuon2 = new TLorentzVector( muon2 . px(), muon2 . py(), muon2 . pz(), muon2 . energy());
		TLorentzVector lvis = (*lmuon1 + *lmuon2 );
		float betaz = lvis . BoostVector().Z();
		lmuon1 -> Boost(0, 0, -(betaz) );
		lmuon2 -> Boost(0, 0, -(betaz) );
		float muon1cp = lmuon1 -> E();
		float muon2cp = lmuon2 -> E();
		float mean1 = muon1cp * (fdc1 + 2);			
		float mean2 = muon2cp * (fdc2 + 2);			
		if(calibration == true){
			mean1 *= calibrationElectronAndMuon[1]; // mapping hard-corded
			mean2 *= calibrationElectronAndMuon[1]; // mapping hard-corded
		}
		smr = (mean1 + mean2) / (fdc1 + fdc2) * 2.0 ;
		delete lmuon1;
		delete lmuon2;
	}
	else if(algorithm == 2){ // not necessarily back-to-back in phi plane
		float mvis = ( muon1 . p4() + muon2 . p4() ). mass();
		mvis *= sqrt( (fdc1 + 2) * (fdc2 + 2) );			
		if(calibration == true){
			mvis *= calibrationElectronAndMuon[1] ;
		}
		smr = mvis;
	}
}

void SMR::estimateMass(pat::Electron elec, pat::Muon muon){
	float dphi = fabs( deltaPhi(elec . phi(), muon . phi() ) );
	if(dphi < dPhiCut){
		smr = -1;
		return;
	}

	fdc1 = 1;
	fdc2 = 1 ;
	if(algorithm == 1){ // back-to-back in phi plane
		TLorentzVector* lelec = new TLorentzVector( elec . px(), elec . py(), elec . pz(), elec . energy());
		TLorentzVector* lmuon = new TLorentzVector( muon . px(), muon . py(), muon . pz(), muon . energy());
		TLorentzVector lvis = (*lelec + *lmuon );
		float betaz = lvis . BoostVector().Z();
		lelec -> Boost(0, 0, -(betaz) );
		lmuon -> Boost(0, 0, -(betaz) );
		float eleccp = lelec -> E();
		float muoncp = lmuon -> E();
		float mean1 = eleccp * (fdc1 + 2);			
		float mean2 = muoncp * (fdc2 + 2);			
		if(calibration == true){
			mean1 *= calibrationElectronAndMuon[0]; // mapping hard-corded
			mean2 *= calibrationElectronAndMuon[1]; // mapping hard-corded
		}
		smr = (mean1 + mean2) / (fdc1 + fdc2) * 2.0 ;
		delete lelec;
		delete lmuon;
	}
	else if(algorithm == 2){ // not necessarily back-to-back in phi plane
		float mvis = ( elec . p4() + muon . p4() ). mass();
		mvis *= sqrt( (fdc1 + 2) * (fdc2 + 2) );			
		if(calibration == true){
			mvis *= sqrt( calibrationElectronAndMuon[0] );
			mvis *= sqrt( calibrationElectronAndMuon[1] );
		}
		smr = mvis;
	}
}

