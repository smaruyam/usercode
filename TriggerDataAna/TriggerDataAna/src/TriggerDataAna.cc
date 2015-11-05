// system include files
#include <memory>
#include <vector>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/PatCandidates/interface/Electron.h"

#include "DataFormats/Candidate/interface/Candidate.h"

#include "DataFormats/Common/interface/ValueMap.h"

#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

#include "DataFormats/EgammaCandidates/interface/ConversionFwd.h"
#include "DataFormats/EgammaCandidates/interface/Conversion.h"
#include "RecoEgamma/EgammaTools/interface/ConversionTools.h"

//muon reco 
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
//
// jet reco
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
// met reco
#include "DataFormats/PatCandidates/interface/MET.h"
// 
// trigger
#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/Common/interface/TriggerResultsByName.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
//
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "DataFormats/PatCandidates/interface/PackedTriggerPrescales.h"
//

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "TTree.h"
#include "Math/VectorUtil.h"

// class declaration

class TriggerDataAna : public edm::EDAnalyzer {
   public:
      explicit TriggerDataAna(const edm::ParameterSet&);
      ~TriggerDataAna();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

	edm::EDGetTokenT<edm::TriggerResults>  triggerResultsTag_;
      edm::EDGetTokenT<pat::TriggerObjectStandAloneCollection> triggerObjects_;
      edm::EDGetTokenT<pat::PackedTriggerPrescales> triggerPrescales_;
//	std::string strig ;
        std::vector<std::string> vstrig ;
	        
      edm::EDGetTokenT<reco::BeamSpot> beamSpotToken_;

      edm::EDGetToken electronsToken_;
      edm::EDGetToken muonsToken_;
      edm::EDGetTokenT<reco::VertexCollection> vtxToken_;
      edm::EDGetTokenT<reco::ConversionCollection> conversionsToken_;

      edm::EDGetToken electronsMiniAODToken_;
      edm::EDGetToken muonsMiniAODToken_;
      edm::EDGetToken jetsMiniAODToken_;
      edm::EDGetToken metsMiniAODToken_;
      edm::EDGetTokenT<reco::VertexCollection> vtxMiniAODToken_;
      edm::EDGetTokenT<reco::ConversionCollection> conversionsMiniAODToken_;

      edm::EDGetTokenT<edm::ValueMap<bool> > eleLooseIdMapToken_;

Int_t trig;
bool bpath;
float hltmet;
float met;
float metCalo;
float metRaw;
float ht;
float jpt1;
float jpt2;
float jeta1;
float jeta2;
float deta;
float mjj;
    unsigned int run;
    unsigned int event;
    unsigned int lumi;

  TTree *mytree;

  Int_t nElectrons_;
  Int_t nMuons_;
  Int_t nJets_;

  std::vector<Float_t> pt_;
  std::vector<Float_t> eta_;
  std::vector<Float_t> phi_;

  std::vector<Float_t> mpt_;
  std::vector<Float_t> meta_;
  std::vector<Float_t> mphi_;
  std::vector<Float_t> misotrk_;
  std::vector<Float_t> misohcl_;
  std::vector<Float_t> misoecl_;
  std::vector<Float_t> misopus_;
  std::vector<Float_t> misotot_;

  std::vector<Float_t> jpt_;
  std::vector<Float_t> jeta_;
  std::vector<Float_t> jphi_;

  std::vector<Float_t> dz_;
  std::vector<Int_t> passConversionVeto_;

  std::vector<Int_t> passLooseId_;
};

TriggerDataAna::TriggerDataAna(const edm::ParameterSet& iConfig):
  triggerResultsTag_(consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("triggerResults"))),
    triggerObjects_(consumes<pat::TriggerObjectStandAloneCollection>(iConfig.getParameter<edm::InputTag>("objects"))),
    triggerPrescales_(consumes<pat::PackedTriggerPrescales>(iConfig.getParameter<edm::InputTag>("prescales"))),
//  strig(iConfig.getUntrackedParameter<std::string>("triggerName", "none" )),
  vstrig(iConfig.getUntrackedParameter<std::vector<std::string> >("triggerNames" )),
  eleLooseIdMapToken_(consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("eleLooseIdMap")))
{
  // Prepare tokens for all input collections and objects
  beamSpotToken_    = consumes<reco::BeamSpot> 
    (iConfig.getParameter <edm::InputTag>
     ("beamSpot"));

  // AOD tokens
  electronsToken_    = mayConsume<edm::View<reco::GsfElectron> >
    (iConfig.getParameter<edm::InputTag>
     ("electrons"));
  muonsToken_    = mayConsume<edm::View<reco::Muon> >
    (iConfig.getParameter<edm::InputTag>
     ("muons"));

  vtxToken_          = mayConsume<reco::VertexCollection>
    (iConfig.getParameter<edm::InputTag>
     ("vertices"));

  conversionsToken_ = mayConsume< reco::ConversionCollection >
    (iConfig.getParameter<edm::InputTag>
     ("conversions"));

  // For electrons, use the fact that pat::Electron can be cast into 
  // GsfElectron
  electronsMiniAODToken_    = mayConsume<edm::View<reco::GsfElectron> >
    (iConfig.getParameter<edm::InputTag>
     ("electronsMiniAOD"));

  muonsMiniAODToken_    = mayConsume<edm::View<reco::Muon> >
    (iConfig.getParameter<edm::InputTag>
     ("muonsMiniAOD"));

  jetsMiniAODToken_    = mayConsume<edm::View<pat::Jet> >
    (iConfig.getParameter<edm::InputTag>
     ("jetsMiniAOD"));

  metsMiniAODToken_    = mayConsume<edm::View<pat::MET> >
    (iConfig.getParameter<edm::InputTag>
     ("metsMiniAOD"));

  vtxMiniAODToken_          = mayConsume<reco::VertexCollection>
    (iConfig.getParameter<edm::InputTag>
     ("verticesMiniAOD"));

  conversionsMiniAODToken_ = mayConsume< reco::ConversionCollection >
    (iConfig.getParameter<edm::InputTag>
     ("conversionsMiniAOD"));

  edm::Service<TFileService> fs;
  mytree = fs->make<TTree> ("ElectronTree", "Electron data");
  
  mytree->Branch("event_runNo",  &run,   "event_runNo/i");
  mytree->Branch("event_evtNo",  &event, "event_evtNo/i");
  mytree->Branch("event_lumi",   &lumi,  "event_lumi/i");
  mytree->Branch("trig",  &trig , "trig/I");

  mytree->Branch("hltmet",  &hltmet , "hltmet/F");
  mytree->Branch("met",  &met , "met/F");
  mytree->Branch("metRaw",  &metRaw , "metRaw/F");
  mytree->Branch("metCalo",  &metCalo , "metCalo/F");
  mytree->Branch("ht",  &ht , "ht/F");
  mytree->Branch("deta",  &deta , "deta/F");

  mytree->Branch("jeta1",  &jeta1 , "jeta1/F");
  mytree->Branch("jeta2",  &jeta2 , "jeta2/F");
  mytree->Branch("jpt1",  &jpt1 , "jpt1/F");
  mytree->Branch("jpt2",  &jpt2 , "jpt2/F");

  mytree->Branch("mjj",  &mjj , "mjj/F");

  mytree->Branch("nEle",  &nElectrons_ , "nEle/I");
  mytree->Branch("pt"  ,  &pt_    );
  mytree->Branch("eta" ,  &eta_ );
  mytree->Branch("phi" ,  &phi_ );

  mytree->Branch("nMu",  &nMuons_ , "nMu/I");
  mytree->Branch("mpt"  ,  &mpt_    );
  mytree->Branch("meta" ,  &meta_ );
  mytree->Branch("mphi" ,  &mphi_ );
  mytree->Branch("misotrk" ,  &misotrk_ );
  mytree->Branch("misohcl" ,  &misohcl_ );
  mytree->Branch("misoecl" ,  &misoecl_ );
  mytree->Branch("misopus" ,  &misopus_ );
  mytree->Branch("misotot" ,  &misotot_ );

  mytree->Branch("nJet",  &nJets_ , "nJet/I");
  mytree->Branch("jpt"  ,  &jpt_    );
  mytree->Branch("jeta" ,  &jeta_ );
  mytree->Branch("jphi" ,  &jphi_ );

  mytree->Branch("dz" ,  &dz_ );
  
}

TriggerDataAna::~TriggerDataAna(){}

void
TriggerDataAna::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace std;
  using namespace edm;
  using namespace reco;

    edm::Handle<edm::TriggerResults>           triggerResultsHandle_;
    edm::Handle<pat::TriggerObjectStandAloneCollection> triggerObjects;
    edm::Handle<pat::PackedTriggerPrescales> triggerPrescales;

    iEvent.getByToken(triggerResultsTag_, triggerResultsHandle_);
    iEvent.getByToken(triggerObjects_, triggerObjects);
    iEvent.getByToken(triggerPrescales_, triggerPrescales);

  trig = -1;
  hltmet = -1;
    const edm::TriggerNames &names = iEvent.triggerNames(*triggerResultsHandle_);
    for (pat::TriggerObjectStandAlone obj : *triggerObjects) { // note: not "const &" since we want to call unpackPathNames
        obj.unpackPathNames(names);
	const std::vector< std::string > sfilters = obj.filterLabels();
	if(sfilters.size() > 0 ){
            for (unsigned h = 0; h < sfilters.size(); h++){// std::cout << " Filer = " << obj.filterLabels()[h];
		if ( sfilters.at(h) . find("hltPFMETNoMu60") != std::string::npos) {
			hltmet = obj.pt() ;
        	}
            }
        }
    }

  if(triggerResultsHandle_.isValid() == true ){
        for(unsigned int u = 0; u < names.size(); u++){
                for(unsigned int v = 0; v < vstrig.size(); v++){
                        if ( names.triggerName(u).find(vstrig.at(v) ) != std::string::npos) {
//		if ( names.triggerName(u).find(strig) != std::string::npos) {
				if(triggerResultsHandle_ -> wasrun(u) ){
					if(triggerResultsHandle_ -> accept(u)){ 
						trig = 1;
					}
				}
			}
		}
	}
  }

  // Get the beam spot
  edm::Handle<reco::BeamSpot> theBeamSpot;
  iEvent.getByToken(beamSpotToken_,theBeamSpot);  
  
  // Retrieve the collection of electrons from the event.
  // If we fail to retrieve the collection with the standard AOD
  // name, we next look for the one with the stndard miniAOD name.
  //   We use exactly the same handle for AOD and miniAOD formats
  // since pat::Electron objects can be recast as reco::GsfElectron objects.
  edm::Handle<edm::View<reco::GsfElectron> > electrons;
  bool isAOD = true;
  iEvent.getByToken(electronsToken_, electrons);
  if( !electrons.isValid() ){
    isAOD = false;
    iEvent.getByToken(electronsMiniAODToken_,electrons);
  }

  edm::Handle<edm::View<reco::Muon> > muons;
  iEvent.getByToken(muonsMiniAODToken_, muons);
  
  edm::Handle<edm::View<pat::Jet> > jets;
  iEvent.getByToken(jetsMiniAODToken_, jets);
  
  edm::Handle<edm::View<pat::MET> > mets;
  iEvent.getByToken(metsMiniAODToken_, mets);

  // Get PV
  edm::Handle<reco::VertexCollection> vertices;
  if( isAOD )
    iEvent.getByToken(vtxToken_, vertices);
  else
    iEvent.getByToken(vtxMiniAODToken_, vertices);
  
  if (vertices->empty()) return; // skip the event if no PV found
  
  // Find the first vertex in the collection that passes
  // good quality criteria
  VertexCollection::const_iterator firstGoodVertex = vertices->end();
  int firstGoodVertexIdx = 0;
  for (VertexCollection::const_iterator vtx = vertices->begin(); 
       vtx != vertices->end(); ++vtx, ++firstGoodVertexIdx) {
    // Replace isFake() for miniAOD because it requires tracks and miniAOD vertices don't have tracks:
    // Vertex.h: bool isFake() const {return (chi2_==0 && ndof_==0 && tracks_.empty());}
    bool isFake = vtx->isFake();
    if( !isAOD )
      isFake = (vtx->chi2()==0 && vtx->ndof()==0);
    // Check the goodness
    if ( !isFake
	 &&  vtx->ndof()>=4. && vtx->position().Rho()<=2.0
	 && fabs(vtx->position().Z())<=24.0) {
      firstGoodVertex = vtx;
      break;
    }
  }

  if ( firstGoodVertex==vertices->end() )
    return; // skip event if there are no good PVs

  // Get the conversions collection
  edm::Handle<reco::ConversionCollection> conversions;
  if(isAOD)
    iEvent.getByToken(conversionsToken_, conversions);
  else
    iEvent.getByToken(conversionsMiniAODToken_, conversions);

  edm::Handle<edm::ValueMap<bool> > loose_id_decisions;
  iEvent.getByToken(eleLooseIdMapToken_ ,loose_id_decisions);

  run   = 0;
  event = 0;
  lumi  = 0;

  met = -1;
  metCalo = -1;
  metRaw = -1;
  ht = 0;
  deta = -1;
  
  jeta1 = -9;
  jeta2 = -9;
  jpt1 = -1;
  jpt2 = -1;

  mjj = -1;

  nElectrons_ = 0;
  nMuons_ = 0;
  nJets_ = 0;
  pt_.clear();
  eta_.clear();
  phi_.clear();
  mpt_.clear();
  meta_.clear();
  mphi_.clear();
  misotrk_.clear();
  misohcl_.clear();
  misoecl_.clear();
  misopus_.clear();
  misotot_.clear();
  jpt_.clear();
  jeta_.clear();
  jphi_.clear();

  dz_.clear();
  passConversionVeto_.clear();     
  passLooseId_ .clear();

  run   = iEvent.id().run();
  event = iEvent.id().event();
  lumi  = iEvent.luminosityBlock();
  // Loop over electrons
  for (size_t i = 0; i < electrons->size(); i++){
    const auto el = electrons->ptrAt(i);
    if( el->pt() < 5 || fabs(el -> eta() ) > 2.5 ) // keep only electrons above 5 GeV
      continue;
    bool passConvVeto = !ConversionTools::hasMatchedConversion(*el, 
							       conversions,
							       theBeamSpot->position());
    if(passConvVeto){
      bool isPassLoose  = (*loose_id_decisions)[el];
      if( isPassLoose ){
        nElectrons_++;
        pt_  .push_back( el->pt() );
        eta_ .push_back( el->superCluster()->eta() );
        phi_ .push_back( el->superCluster()->phi() );
        reco::GsfTrackRef theTrack = el->gsfTrack();
        dz_.push_back( theTrack->dz( firstGoodVertex->position() ) );
      }
    }  
   }

std::vector<unsigned int> mindex ;
mindex . clear();
  for (size_t i = 0; i < muons->size(); i++){
    const auto mu = muons->ptrAt(i);
    if(mu -> pt () < 5 || fabs( mu -> eta() ) > 2.5 ) continue;
    bool bmuon = muon::isLooseMuon( *mu );
    if ( bmuon ){ 
	float muIsoTrk =  mu -> pfIsolationR04().sumChargedHadronPt / mu -> pt();
	float muIsoHcl =  mu -> pfIsolationR04().sumNeutralHadronEt / mu -> pt();
	float muIsoEcl =  mu -> pfIsolationR04().sumPhotonEt / mu -> pt();
	float muIsoPUs =  mu -> pfIsolationR04().sumPUPt / mu -> pt();
	float muIsoTot = muIsoTrk + max(0., muIsoHcl + muIsoEcl - 0.5 * muIsoPUs);
	if( muIsoTot < 0.2 ){ // loose iso
	    	mpt_  .push_back( mu->pt() );
		meta_ .push_back( mu->eta() );
	    	mphi_ .push_back( mu->phi() );
		misotrk_.push_back ( muIsoTrk ) ;
		misohcl_.push_back ( muIsoHcl ) ;
		misoecl_.push_back ( muIsoEcl ) ;
		misopus_.push_back ( muIsoPUs ) ;
		misotot_.push_back ( muIsoTot ) ;
		nMuons_ ++;
		mindex . push_back(i) ;
	}
    }
  }

std::vector<unsigned int> jindex ;
jindex . clear();
  for (size_t i = 0; i < jets->size(); i++){
    const auto jet = jets->ptrAt(i);
    if(jet -> pt () < 10 || fabs(jet -> eta() ) > 5.0 ) continue;
    bool bljet = false;
float NHF = jet->neutralHadronEnergyFraction();
float NEMF = jet->neutralEmEnergyFraction();
float CHF = jet->chargedHadronEnergyFraction();
float MUF = jet->muonEnergyFraction();
float CEMF = jet->chargedEmEnergyFraction();
float NumConst = jet->chargedMultiplicity() + jet->neutralMultiplicity();
float CHM = jet->chargedMultiplicity();
float eta = fabs( jet -> eta() );
if ( (NHF<0.99 && NEMF<0.99 && NumConst>1 && MUF<0.8) && ( ( eta <= 2.4 && CHF>0 && CHM>0 && CEMF<0.99) || eta > 2.4 ) ){
	bljet = true;
}
    if ( bljet ){ 
	if( jet -> pt() > 20 ){
	    	jpt_  .push_back( jet->pt() );
		jeta_ .push_back( jet->eta() );
	    	jphi_ .push_back( jet->phi() );
		nJets_ ++;
		jindex.push_back(i);
		if( eta < 3.0 ) ht += jet->pt() ;
	}
    }
  }

  float tmp_mjj = -1;
  for (size_t i = 0; i < jindex . size(); i++){
  	for (size_t j = i; j < jindex . size(); j++){
		if( j > i ){
			if(  jets->ptrAt( jindex.at(i) ) -> pt() > 30 && jets->ptrAt( jindex.at(j) ) -> pt() > 30 ){
				float etaeta = jets->ptrAt( jindex.at(i) ) -> eta() * jets->ptrAt( jindex.at(j) ) -> eta() ;
				if ( etaeta < 0 ){
					float tmp_deta = fabs ( jets->ptrAt( jindex.at(i) ) -> eta() - jets->ptrAt(jindex.at(j) ) -> eta() );
					if( tmp_deta > 3.5){
						tmp_mjj = (  jets->ptrAt( jindex.at(i) ) -> p4() + jets->ptrAt( jindex.at(j) ) -> p4() ).mass();
						if ( tmp_mjj > mjj ){ 
							mjj = tmp_mjj;
							deta = tmp_deta;
							jeta1 = jets->ptrAt( jindex.at(i) ) -> eta();
							jeta2 = jets->ptrAt( jindex.at(j) ) -> eta();
							jpt1 = jets->ptrAt( jindex.at(i) ) -> pt();
							jpt2 = jets->ptrAt( jindex.at(j) ) -> pt();
						}
					}
				}
			}
		}
	}
  }

  if( mets->size() > 0){
     const auto pmet = mets->ptrAt(0);
     met     = pmet -> pt();
//     metRaw  = pmet -> uncorrectedPt();
     metCalo = pmet -> caloMETPt();
  }

  mytree->Fill();
}


void TriggerDataAna::beginJob(){}

void TriggerDataAna::endJob() {}

void
TriggerDataAna::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

DEFINE_FWK_MODULE(TriggerDataAna);
