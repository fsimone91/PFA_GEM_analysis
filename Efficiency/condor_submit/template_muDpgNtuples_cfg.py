import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
from Configuration.StandardSequences.Eras import eras
from Configuration.AlCa.GlobalTag import GlobalTag

import subprocess
import sys

options = VarParsing.VarParsing()

options.register('globalTag',
                 '110X_mcRun3_2021_realistic_v9',
                 #'auto:phase1_2021_realistic',
                 #'111X_upgrade2018_realistic_v1', #default value
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 "Global Tag")

options.register('nEvents',
                 -1, #default value
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.int,
                 "Maximum number of processed events")

options.register('inputFolder',
                 #'/afs/cern.ch/user/f/fsimone/public/Gabriele/',
                 #'Run3Summer19GS-step2.root',
                 '/eos/cms/store/express/Commissioning2020/ExpressCosmics/FEVT/Express-v1/000/337/973/00000/',
                 #'/store/data/Commissioning2020/Cosmics/RAW-RECO/CosmicSP-PromptReco-v1/000/335/443/00000',
                 #'/afs/cern.ch/work/r/rosma/public/DTNtuples/MyCosmicProduction/',
                 #'/afs/cern.ch/user/g/gmilella/CMSSW_11_2_0_pre2/src/DTNtuples/MyCosmicProduction/',
                 #'/eos/cms/store/user/battilan/DPGO/MuonDPGNtuples/', #default value
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 "EOS folder with input files")

options.register('secondaryInputFolder',
                 '', #default value
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 "EOS folder with input files for secondary files")

options.register('ntupleName',
#ADD OUTPUT PATH HERE
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 "Folder and name ame for output ntuple")

options.parseArguments()

process = cms.Process("MUNTUPLES",eras.Run3)#Run2_2018)

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
#process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(options.nEvents))

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')

process.GlobalTag.globaltag = cms.string(options.globalTag)

process.source = cms.Source("PoolSource",
                            
        fileNames = cms.untracked.vstring(),
        secondaryFileNames = cms.untracked.vstring()

)

#if "eos/cms" in options.inputFolder:
#    files = subprocess.check_output(['xrdfs', 'root://eoscms.cern.ch', 'ls', options.inputFolder])
#    process.source.fileNames = ["root://eoscms.cern.ch//" + f for f in files.split()]
#else:
#    files = subprocess.check_output(['ls', options.inputFolder])
#    process.source.fileNames = ["file://" + options.inputFolder + "/" + f for f in files.split()]
#
#if options.secondaryInputFolder != "" :
#    files = subprocess.check_output(["ls", options.secondaryInputFolder])
#    process.source.secondaryFileNames = ["file://" + options.secondaryInputFolder + "/" + f for f in files.split()]


process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
#ADD INPUT FILES HERE

    ),
)

print process.source.fileNames
process.TFileService = cms.Service('TFileService',
        fileName = cms.string(options.ntupleName)
    )

process.load('Configuration/StandardSequences/GeometryRecoDB_cff')
process.load("Configuration.StandardSequences.MagneticField_cff")

process.load("TrackingTools/TransientTrack/TransientTrackBuilder_cfi")
process.load('TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAny_cfi')

process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
process.load('MuDPGAnalysis.MuonDPGNtuples.muNtupleProducer_cfi')

process.p = cms.Path(#process.muonDTDigis + 
                      process.muNtupleProducer)


