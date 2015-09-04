from WMCore.Configuration import Configuration
config = Configuration()
config.section_('General')
config.General.transferOutputs = True
config.section_('JobType')
config.JobType.psetName = 'runData.py'
config.JobType.pluginName = 'Analysis'
config.JobType.outputFiles = ['myntuple_mini.root']
config.JobType.pyCfgParams = ['noprint']
config.section_('Data')
config.Data.inputDataset = '/SingleMuon/Run2015C-PromptReco-v1/MINIAOD'
config.Data.unitsPerJob = 10
config.Data.splitting = 'LumiBased'
config.Data.publication = False
config.Data.lumiMask = 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions15/13TeV/Cert_246908-254879_13TeV_PromptReco_Collisions15_JSON.txt'
# min 25ns run 254227
config.Data.runRange = '254227-254879'
config.Data.outLFNDirBase = '/store/user/smaruyam/'
config.section_('User')
config.section_('Site')
config.Site.storageSite = 'T3_US_FNALLPC'
