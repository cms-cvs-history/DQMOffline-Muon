# The following comments couldn't be translated into the new config version:

#keep the logging output to a nice level

import FWCore.ParameterSet.Config as cms

process = cms.Process("RPCSegmentEff")
process.load("Geometry.MuonCommonData.muonIdealGeometryXML_cfi")

process.load("Geometry.RPCGeometry.rpcGeometry_cfi")

process.load("Geometry.CSCGeometry.cscGeometry_cfi")

process.load("Geometry.DTGeometry.dtGeometry_cfi")

process.load("Geometry.MuonNumbering.muonNumberingInitialization_cfi")

process.load("DQMServices.Components.MEtoEDMConverter_cfi")

process.load("DQMServices.Core.DQM_cfg")

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1000)
)
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('rfio:/castor/cern.ch/cms//store/data/2008/5/20/T0ReReco-GlobalCruzet1-A-v1/0006/FEACFB38-E826-DD11-8509-000423D985E4.root')
)

process.MessageLogger = cms.Service("MessageLogger",
    destinations = cms.untracked.vstring('/tmp/carrillo/RPCEfficiency.log')
)

process.museg = cms.EDFilter("RPCEfficiency",
    cscSegments = cms.untracked.string('cscSegments'),
    dt4DSegments = cms.untracked.string('dt4DSegments'),
    incldtMB4 = cms.untracked.bool(True),
    MaxResStripToCountInAverage = cms.untracked.double(5.0),
    MinimalResidual = cms.untracked.double(4.5),
    inclcsc = cms.untracked.bool(False),
    MinimalResidualRB4 = cms.untracked.double(10.0),
    EffSaveRootFile = cms.untracked.bool(False),
    MaxDrb4 = cms.untracked.double(150.0),
    MinCosAng = cms.untracked.double(0.99),
    muonRPCDigis = cms.untracked.string('muonRPCDigis'),
    EffRootFileName = cms.untracked.string('MuonSegEff.root'),
    MaxResStripToCountInAverageRB4 = cms.untracked.double(7.0),
    incldt = cms.untracked.bool(True),
    EffSaveRootFileEventsInterval = cms.untracked.int32(100),
    MaxD = cms.untracked.double(80.0)
)

process.FEVT = cms.OutputModule("PoolOutputModule",
    outputCommands = cms.untracked.vstring('keep *_MEtoEDMConverter_*_*'),
    fileName = cms.untracked.string('/tmp/carrillo/prova.root')
)

process.p = cms.Path(process.museg*process.MEtoEDMConverter)
process.outpath = cms.EndPath(process.FEVT)
process.DQM.collectorHost = ''
process.DQM.collectorPort = 9090
process.DQM.debug = False


