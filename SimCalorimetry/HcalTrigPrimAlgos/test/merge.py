import FWCore.ParameterSet.Config as cms

process = cms.Process('MERGE')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100)
)

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        [
            '/store/relval/CMSSW_6_2_0_SLHC17/RelValTTbar_14TeV/GEN-SIM-RECO/DES19_62_V8_UPG2019withGEM-v1/00000/0C5147DF-452D-E411-A5B3-0025905A6068.root',
            '/store/relval/CMSSW_6_2_0_SLHC17/RelValTTbar_14TeV/GEN-SIM-RECO/DES19_62_V8_UPG2019withGEM-v1/00000/108122F4-532D-E411-AEC2-0025905A60F2.root',
            '/store/relval/CMSSW_6_2_0_SLHC17/RelValTTbar_14TeV/GEN-SIM-RECO/DES19_62_V8_UPG2019withGEM-v1/00000/44DA5DE7-4C2D-E411-ACF7-0025905B858C.root',
            '/store/relval/CMSSW_6_2_0_SLHC17/RelValTTbar_14TeV/GEN-SIM-RECO/DES19_62_V8_UPG2019withGEM-v1/00000/566488AD-4C2D-E411-B392-0025905B855C.root',
            '/store/relval/CMSSW_6_2_0_SLHC17/RelValTTbar_14TeV/GEN-SIM-RECO/DES19_62_V8_UPG2019withGEM-v1/00000/587CB334-492D-E411-B983-002590593872.root',
            '/store/relval/CMSSW_6_2_0_SLHC17/RelValTTbar_14TeV/GEN-SIM-RECO/DES19_62_V8_UPG2019withGEM-v1/00000/5C4A3A11-4B2D-E411-B0A2-002618943913.root',
            '/store/relval/CMSSW_6_2_0_SLHC17/RelValTTbar_14TeV/GEN-SIM-RECO/DES19_62_V8_UPG2019withGEM-v1/00000/8029021E-4B2D-E411-AE12-0025905A60F4.root',
            '/store/relval/CMSSW_6_2_0_SLHC17/RelValTTbar_14TeV/GEN-SIM-RECO/DES19_62_V8_UPG2019withGEM-v1/00000/94126170-472D-E411-931C-0025905B8598.root',
            '/store/relval/CMSSW_6_2_0_SLHC17/RelValTTbar_14TeV/GEN-SIM-RECO/DES19_62_V8_UPG2019withGEM-v1/00000/B0AA5732-5D2D-E411-A435-0025905B855C.root'
        ]
    ),
    secondaryFileNames = cms.untracked.vstring(
        [
            '/store/relval/CMSSW_6_2_0_SLHC17/RelValTTbar_14TeV/GEN-SIM-DIGI-RAW/DES19_62_V8_UPG2019withGEM-v1/00000/000399A6-4A2D-E411-8A32-0025905A60AA.root',
            '/store/relval/CMSSW_6_2_0_SLHC17/RelValTTbar_14TeV/GEN-SIM-DIGI-RAW/DES19_62_V8_UPG2019withGEM-v1/00000/425D5D0B-442D-E411-9801-0025905B860E.root',
            '/store/relval/CMSSW_6_2_0_SLHC17/RelValTTbar_14TeV/GEN-SIM-DIGI-RAW/DES19_62_V8_UPG2019withGEM-v1/00000/480A4002-442D-E411-9C5B-0025905B85E8.root',
            '/store/relval/CMSSW_6_2_0_SLHC17/RelValTTbar_14TeV/GEN-SIM-DIGI-RAW/DES19_62_V8_UPG2019withGEM-v1/00000/C8305535-4B2D-E411-A8F1-0025905A609E.root',
            '/store/relval/CMSSW_6_2_0_SLHC17/RelValTTbar_14TeV/GEN-SIM-DIGI-RAW/DES19_62_V8_UPG2019withGEM-v1/00000/CE8427B2-472D-E411-AA62-0025905964C4.root',
            '/store/relval/CMSSW_6_2_0_SLHC17/RelValTTbar_14TeV/GEN-SIM-DIGI-RAW/DES19_62_V8_UPG2019withGEM-v1/00000/D4798386-442D-E411-9462-0025905A48FC.root',
            '/store/relval/CMSSW_6_2_0_SLHC17/RelValTTbar_14TeV/GEN-SIM-DIGI-RAW/DES19_62_V8_UPG2019withGEM-v1/00000/F8B8193B-462D-E411-96B6-0025905B85EE.root'
        ]
    )
    # fileNames = cms.untracked.vstring('/store/relval/CMSSW_6_2_0_SLHC17/RelValTTbar_14TeV/GEN-SIM-DIGI-RAW/DES19_62_V8_UPG2019withGEM-v1/00000/425D5D0B-442D-E411-9801-0025905B860E.root')
    # fileNames = cms.untracked.vstring('/store/relval/CMSSW_6_2_0_SLHC17/RelValTTbar_14TeV/GEN-SIM-DIGI-RAW/DES19_62_V8_UPG2019withGEM-v1/00000/C8305535-4B2D-E411-A8F1-0025905A609E.root')
)

process.out = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string("merged.root")
)

process.end = cms.EndPath(process.out)
