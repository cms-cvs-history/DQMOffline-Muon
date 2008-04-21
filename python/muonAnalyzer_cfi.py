import FWCore.ParameterSet.Config as cms

# MuonAnalyzer
muonAnalyzer = cms.EDAnalyzer("MuonAnalyzer",
    MuonServiceProxy,
    OutputMEsInRootFile = cms.bool(False),
    CosmicsCollectionLabel = cms.InputTag("muons"),
    muonRecoAnalysis = cms.PSet(
        thetaMin = cms.double(0.0),
        phiMin = cms.double(-3.2),
        ptBin = cms.int32(500),
        thetaBin = cms.int32(100),
        pResMin = cms.double(-0.5),
        pResMax = cms.double(0.5),
        pResBin = cms.int32(1000),
        thetaMax = cms.double(3.2),
        pMin = cms.double(0.0),
        ptMin = cms.double(0.0),
        etaMin = cms.double(-3.0),
        etaBin = cms.int32(100),
        phiBin = cms.int32(100),
        pBin = cms.int32(500),
        ptMax = cms.double(500.0),
        etaMax = cms.double(3.0),
        pMax = cms.double(500.0),
        phiMax = cms.double(3.2)
    ),
    seedsAnalysis = cms.PSet(
        phiErrMax = cms.double(5.0),
        seedPxMin = cms.double(-50.0),
        seedPyMax = cms.double(50.0),
        seedPxMax = cms.double(50.0),
        RecHitBin = cms.int32(22),
        pErrMin = cms.double(-1000.0),
        pyErrMax = cms.double(1000.0),
        seedPzMax = cms.double(50.0),
        etaErrMin = cms.double(-0.5),
        seedPtMin = cms.double(-0.5),
        ThetaBin = cms.int32(100),
        RecHitMin = cms.double(3.0),
        seedPyBin = cms.int32(100),
        pzErrMin = cms.double(-1000.0),
        EtaMin = cms.double(-3.0),
        pErrBin = cms.int32(1000),
        pyErrMin = cms.double(-1000.0),
        phiErrBin = cms.int32(500),
        seedPyMin = cms.double(-50.0),
        EtaMax = cms.double(3.0),
        etaErrBin = cms.int32(500),
        PhiMax = cms.double(3.2),
        ThetaMin = cms.double(0.0),
        PhiMin = cms.double(-3.2),
        RecHitMax = cms.double(25.0),
        pzErrBin = cms.int32(1000),
        ptErrMin = cms.double(0.0),
        ThetaMax = cms.double(3.2),
        seedPzBin = cms.int32(100),
        etaErrMax = cms.double(0.5),
        EtaBin = cms.int32(100),
        pErrMax = cms.double(1000.0),
        pzErrMax = cms.double(1000.0),
        pxErrBin = cms.int32(1000),
        pxErrMax = cms.double(1000.0),
        seedPtMax = cms.double(19.5),
        ptErrBin = cms.int32(500),
        seedPxBin = cms.int32(100),
        seedPtBin = cms.int32(20),
        phiErrMin = cms.double(-5.0),
        pyErrBin = cms.int32(1000),
        pxErrMin = cms.double(-1000.0),
        PhiBin = cms.int32(100),
        debug = cms.bool(True),
        ptErrMax = cms.double(1000.0),
        seedPzMin = cms.double(-50.0)
    ),
    OutputFileName = cms.string('MuonMonitoring.root'),
    seedsCollectionLabel = cms.InputTag("CosmicMuonSeed"),
    muonEnergyAnalysis = cms.PSet(
        AlgoName = cms.string('muons'),
        hadS9SizeMin = cms.double(0.0),
        emSizeMin = cms.double(0.0),
        emS9SizeBin = cms.int32(1000),
        emS9SizeMin = cms.double(0.0),
        hoSizeMax = cms.double(10.0),
        hoS9SizeBin = cms.int32(1000),
        hoSizeMin = cms.double(0.0),
        emSizeMax = cms.double(10.0),
        hadS9SizeMax = cms.double(10.0),
        hoS9SizeMin = cms.double(0.0),
        hadSizeMin = cms.double(0.0),
        emSizeBin = cms.int32(1000),
        hadS9SizeBin = cms.int32(1000),
        debug = cms.bool(True),
        emS9SizeMax = cms.double(10.0),
        hoS9SizeMax = cms.double(10.0),
        hadSizeMax = cms.double(10.0),
        hoSizeBin = cms.int32(1000),
        hadSizeBin = cms.int32(1000)
    )
)

