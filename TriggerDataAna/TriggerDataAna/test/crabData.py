from WMCore.Configuration import Configuration
config = Configuration()
config.section_('General')
config.General.transferOutputs = True
config.section_('JobType')
config.JobType.psetName = 'runRunB.py'
config.JobType.pluginName = 'Analysis'
config.JobType.outputFiles = ['myntuple_mini.root']
config.JobType.pyCfgParams = ['noprint']
config.section_('Data')
config.Data.inputDataset = '/SingleMuon/Run2015B-PromptReco-v1/MINIAOD'
config.Data.unitsPerJob = 3
config.Data.splitting = 'FileBased'
config.Data.publication = False
config.Data.lumiMask = 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions15/13TeV/Cert_246908-254879_13TeV_PromptReco_Collisions15_JSON.txt'
config.Data.outLFNDirBase = '/store/user/smaruyam/'
config.section_('User')
config.section_('Site')
config.Site.storageSite = 'T3_US_FNALLPC'
