import FWCore.ParameterSet.Config as cms

process = cms.Process("Test")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load("Configuration.Geometry.GeometryIdeal_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
process.GlobalTag.globaltag = '74X_dataRun2_Prompt_v1'

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
inputFilesAOD = cms.untracked.vstring(
    '/store/mc/Phys14DR/DYJetsToLL_M-50_13TeV-madgraph-pythia8/AODSIM/PU20bx25_PHYS14_25_V1-v1/00000/00CC714A-F86B-E411-B99A-0025904B5FB8.root',
    )    

inputFilesMiniAOD = cms.untracked.vstring(
	'/store/data/Run2015C/SingleMuon/MINIAOD/PromptReco-v1/000/254/833/00000/EAEF3DB9-104B-E511-9C21-02163E0139BA.root'
    )

useAOD = False

if useAOD == True :
    inputFiles = inputFilesAOD
    outputFile = "myntuple.root"
    print("AOD input files are used")
else :
    inputFiles = inputFilesMiniAOD
    outputFile = "myntuple_mini.root"
    print("MiniAOD input files are used")
process.source = cms.Source ("PoolSource", fileNames = inputFiles )                             

# Set up electron ID (VID framework)
from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
if useAOD == True :
    dataFormat = DataFormat.AOD
else :
    dataFormat = DataFormat.MiniAOD

switchOnVIDElectronIdProducer(process, dataFormat)

my_id_modules = ['RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_PHYS14_PU20bx25_V2_cff']

for idmod in my_id_modules:
    setupAllVIDIdsInModule(process,idmod,setupVIDElectronSelection)

process.ntupler = cms.EDAnalyzer('TriggerDataAna',
				triggerResults = cms.InputTag( 'TriggerResults', '', 'HLT' ),
    prescales = cms.InputTag("patTrigger"),
    objects = cms.InputTag("selectedPatTrigger"),
				triggerName = cms.untracked.string("HLT_Mu10_TrkIsoVVL_DiPFJet40_DEta3p5_MJJ750_HTT350_PFMETNoMu60_v"),
                                 beamSpot = cms.InputTag('offlineBeamSpot'),
                                 electrons    = cms.InputTag("gedGsfElectrons"),
                                 muons    = cms.InputTag("muons"),
                                 vertices     = cms.InputTag("offlinePrimaryVertices"),
                                 conversions  = cms.InputTag('allConversions'),
                                 electronsMiniAOD    = cms.InputTag("slimmedElectrons"),
                                 muonsMiniAOD    = cms.InputTag("slimmedMuons"),
                                 jetsMiniAOD    = cms.InputTag("slimmedJets"),
                                 metsMiniAOD    = cms.InputTag("slimmedMETs"),
                                 verticesMiniAOD     = cms.InputTag("offlineSlimmedPrimaryVertices"),
                                 conversionsMiniAOD  = cms.InputTag('reducedEgamma:reducedConversions'),
                                 eleLooseIdMap = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-PHYS14-PU20bx25-V2-standalone-loose")
                                )

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string( outputFile )
                                   )
process.p = cms.Path(process.egmGsfElectronIDSequence * process.ntupler)
