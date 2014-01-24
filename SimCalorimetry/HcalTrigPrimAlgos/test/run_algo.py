# Auto generated configuration file
# using: 
# Revision: 1.20 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: step2 --conditions W19_500_62E2::All -n 10 --eventcontent FEVTDEBUGHLT -s DIGI:pdigi_valid,L1,DIGI2RAW --datatier GEN-SIM-DIGI-RAW --customise SLHCUpgradeSimulations/Configuration/combinedCustoms.cust_2019,SLHCUpgradeSimulations/Configuration/aging.customise_aging_500 --geometry Extended2019 --magField 38T_PostLS1 --filein /store/relval/CMSSW_6_2_0_SLHC17/RelValTTbar_14TeV/GEN-SIM-DIGI-RAW/DES19_62_V8_UPG2019withGEM-v1/00000/000399A6-4A2D-E411-8A32-0025905A60AA.root --fileout step2.root
import FWCore.ParameterSet.Config as cms

process = cms.Process('TPPROD')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
# process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.Geometry.GeometryExtended2019Reco_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_PostLS1_cff')
process.load('Configuration.StandardSequences.Digi_cff')
process.load('Configuration.StandardSequences.SimL1Emulator_cff')
process.load('Configuration.StandardSequences.DigiToRaw_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100)
)

# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('/store/relval/CMSSW_6_2_0_SLHC17/RelValTTbar_14TeV/GEN-SIM-DIGI-RAW/DES19_62_V8_UPG2019withGEM-v1/00000/C8305535-4B2D-E411-A8F1-0025905A609E.root')
)

process.options = cms.untracked.PSet(

)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('$Revision: 1.20 $'),
    annotation = cms.untracked.string('step2 nevts:10'),
    name = cms.untracked.string('Applications')
)

# Output definition

process.FEVTDEBUGHLToutput = cms.OutputModule("PoolOutputModule",
    splitLevel = cms.untracked.int32(0),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    outputCommands = process.FEVTDEBUGHLTEventContent.outputCommands,
    fileName = cms.untracked.string('step2.root'),
    dataset = cms.untracked.PSet(
        filterName = cms.untracked.string(''),
        dataTier = cms.untracked.string('DIGI-RECO')
    )
)

# Additional output definition

# Other statements
# process.mix.digitizers = cms.PSet(process.theDigitizersValid)
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:upgrade2019', '')

# Path and EndPath definitions
process.digitisation_step = cms.Path(process.pdigi_valid)
process.L1simulation_step = cms.Path(process.SimL1Emulator)
process.digi2raw_step = cms.Path(process.DigiToRaw)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.FEVTDEBUGHLToutput_step = cms.EndPath(process.FEVTDEBUGHLToutput)

# process.chainplotter = cms.EDAnalyzer("HcalCompareChains",
#         TriggerPrimitives = cms.InputTag('simHcalTriggerPrimitiveDigis', '', 'TPPROD'),
#         RecHits = cms.InputTag('hbheUpgradeReco'))

# process.mapping = cms.EDAnalyzer("HcalMapping")

# process.poke = cms.Path(process.chainplotter) # for plots
# process.poke = cms.Path(process.mapping)
process.poke = cms.Path()

# process.TFileService = cms.Service("TFileService",
#         closeFileFast = cms.untracked.bool(True),
#         fileName = cms.string("debug.root"))

# Schedule definition
# process.schedule = cms.Schedule(process.digitisation_step,process.L1simulation_step,process.digi2raw_step,process.endjob_step,process.FEVTDEBUGHLToutput_step)
# process.simpledigi = cms.Path(process.simHcalTriggerPrimitiveDigis)
# process.schedule = cms.Schedule(process.simpledigi,process.poke,process.endjob_step,process.FEVTDEBUGHLToutput_step)
process.schedule = cms.Schedule(process.digitisation_step,process.poke,process.endjob_step,process.FEVTDEBUGHLToutput_step)
# process.schedule = cms.Schedule(process.simpledigi,process.endjob_step,process.FEVTDEBUGHLToutput_step)

# customisation of the process.

# Automatic addition of the customisation function from SLHCUpgradeSimulations.Configuration.combinedCustoms
from SLHCUpgradeSimulations.Configuration.combinedCustoms import cust_2019

#call to customisation function cust_2019 imported from SLHCUpgradeSimulations.Configuration.combinedCustoms
process = cust_2019(process)

process.simHcalTriggerPrimitiveDigis.inputLabel = cms.VInputTag(
        cms.InputTag("simHcalUnsuppressedDigis","HBHEUpgradeDigiCollection","DIGI2RAW"),
        cms.InputTag("simHcalUnsuppressedDigis","HFUpgradeDigiCollection","DIGI2RAW"))

# Automatic addition of the customisation function from SLHCUpgradeSimulations.Configuration.aging
# from SLHCUpgradeSimulations.Configuration.aging import customise_aging_500 

#call to customisation function customise_aging_500 imported from SLHCUpgradeSimulations.Configuration.aging
# process = customise_aging_500(process)

# End of customisation functions

# print process.dumpPython()
