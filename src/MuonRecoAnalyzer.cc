
/*
 *  See header file for a description of this class.
 *
 *  $Date: 2008/11/17 15:19:10 $
 *  $Revision: 1.16 $
 *  \author G. Mila - INFN Torino
 */

#include "DQMOffline/Muon/src/MuonRecoAnalyzer.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h" 
#include "DataFormats/MuonReco/interface/MuonEnergy.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include <string>
using namespace std;
using namespace edm;



MuonRecoAnalyzer::MuonRecoAnalyzer(const edm::ParameterSet& pSet, MuonServiceProxy *theService):MuonAnalyzerBase(theService) {
  parameters = pSet;
}


MuonRecoAnalyzer::~MuonRecoAnalyzer() { }


void MuonRecoAnalyzer::beginJob(edm::EventSetup const& iSetup,DQMStore * dbe) {

  metname = "muRecoAnalyzer";

  LogTrace(metname)<<"[MuonRecoAnalyzer] Parameters initialization";
  dbe->setCurrentFolder("Muons/MuonRecoAnalyzer");

  muReco = dbe->book1D("muReco", "muon reconstructed tracks", 6, 1, 7);
  muReco->setBinLabel(1,"glb+tk+sta");
  muReco->setBinLabel(2,"glb+sta");
  muReco->setBinLabel(3,"tk+sta");
  muReco->setBinLabel(4,"tk");
  muReco->setBinLabel(5,"sta");
  muReco->setBinLabel(6,"calo");

  int binFactor = 4;

  // monitoring of eta parameter
  etaBin = parameters.getParameter<int>("etaBin");
  etaMin = parameters.getParameter<double>("etaMin");
  etaMax = parameters.getParameter<double>("etaMax");
  std::string histname = "GlbMuon_";
  etaGlbTrack.push_back(dbe->book1D(histname+"Glb_eta", "#eta_{GLB}", etaBin, etaMin, etaMax));
  etaGlbTrack.push_back(dbe->book1D(histname+"Tk_eta", "#eta_{TKfromGLB}", etaBin, etaMin, etaMax));
  etaGlbTrack.push_back(dbe->book1D(histname+"Sta_eta", "#eta_{STAfromGLB}", etaBin, etaMin, etaMax));
  etaResolution.push_back(dbe->book1D("Res_TkGlb_eta", "#eta_{TKfromGLB} - #eta_{GLB}", etaBin*binFactor, etaMin/3000, etaMax/3000));
  etaResolution.push_back(dbe->book1D("Res_GlbSta_eta", "#eta_{GLB} - #eta_{STAfromGLB}", etaBin*binFactor, etaMin/100, etaMax/100));
  etaResolution.push_back(dbe->book1D("Res_TkSta_eta", "#eta_{TKfromGLB} - #eta_{STAfromGLB}", etaBin*binFactor, etaMin/100, etaMax/100));
  etaResolution.push_back(dbe->book2D("ResVsEta_TkGlb_eta", "(#eta_{TKfromGLB} - #eta_{GLB}) vs #eta_{GLB}", etaBin, etaMin, etaMax, etaBin*binFactor, etaMin/3000, etaMax/3000));
  etaResolution.push_back(dbe->book2D("ResVsEta_GlbSta_eta", "(#eta_{GLB} - #eta_{STAfromGLB}) vs #eta_{GLB}", etaBin, etaMin, etaMax, etaBin*binFactor, etaMin/100, etaMax/100));
  etaResolution.push_back(dbe->book2D("ResVsEta_TkSta_eta", "(#eta_{TKfromGLB} - #eta_{STAfromGLB}) vs #eta_{TKfromGLB}", etaBin, etaMin, etaMax, etaBin*binFactor, etaMin/100, etaMax/100));
  etaTrack = dbe->book1D("TkMuon_eta", "#eta_{TK}", etaBin, etaMin, etaMax);
  etaStaTrack = dbe->book1D("StaMuon_eta", "#eta_{STA}", etaBin, etaMin, etaMax);
  etaEfficiency.push_back(dbe->book1D("StaEta", "#eta_{STAfromGLB}", etaBin, etaMin, etaMax));
  etaEfficiency.push_back(dbe->book1D("StaEta_ifCombinedAlso", "#eta_{STAfromGLB} if isGlb=true", etaBin, etaMin, etaMax));

  // monitoring of theta parameter
  thetaBin = parameters.getParameter<int>("thetaBin");
  thetaMin = parameters.getParameter<double>("thetaMin");
  thetaMax = parameters.getParameter<double>("thetaMax");
  thetaGlbTrack.push_back(dbe->book1D(histname+"Glb_theta", "#theta_{GLB}", thetaBin, thetaMin, thetaMax));
  thetaGlbTrack[0]->setAxisTitle("rad");
  thetaGlbTrack.push_back(dbe->book1D(histname+"Tk_theta", "#theta_{TKfromGLB}", thetaBin, thetaMin, thetaMax));
  thetaGlbTrack[1]->setAxisTitle("rad");
  thetaGlbTrack.push_back(dbe->book1D(histname+"Sta_theta", "#theta_{STAfromGLB}", thetaBin, thetaMin, thetaMax));
  thetaGlbTrack[2]->setAxisTitle("rad");
  thetaResolution.push_back(dbe->book1D("Res_TkGlb_theta", "#theta_{TKfromGLB} - #theta_{GLB}", thetaBin*binFactor, -(thetaMax/3000), thetaMax/3000));
  thetaResolution[0]->setAxisTitle("rad");
  thetaResolution.push_back(dbe->book1D("Res_GlbSta_theta", "#theta_{GLB} - #theta_{STAfromGLB}", thetaBin*binFactor,-(thetaMax/100), thetaMax/100));
  thetaResolution[1]->setAxisTitle("rad");
  thetaResolution.push_back(dbe->book1D("Res_TkSta_theta", "#theta_{TKfromGLB} - #theta_{STAfromGLB}", thetaBin*binFactor, -(thetaMax/100), thetaMax/100));
  thetaResolution[2]->setAxisTitle("rad");
  thetaResolution.push_back(dbe->book2D("ResVsTheta_TkGlb_theta", "(#theta_{TKfromGLB} - #theta_{GLB}) vs #theta_{GLB}", thetaBin, thetaMin, thetaMax, thetaBin*binFactor, -(thetaMax/3000), thetaMax/3000));
  thetaResolution[3]->setAxisTitle("rad",1);
  thetaResolution[3]->setAxisTitle("rad",2);
  thetaResolution.push_back(dbe->book2D("ResVsTheta_GlbSta_theta", "(#theta_{GLB} - #theta_{STAfromGLB}) vs #theta_{GLB}", thetaBin, thetaMin, thetaMax, thetaBin*binFactor, -(thetaMax/100), thetaMax/100));
  thetaResolution[4]->setAxisTitle("rad",1);
  thetaResolution[4]->setAxisTitle("rad",2);
  thetaResolution.push_back(dbe->book2D("ResVsTheta_TkSta_theta", "(#theta_{TKfromGLB} - #theta_{STAfromGLB}) vs #theta_{TKfromGLB}", thetaBin, thetaMin, thetaMax, thetaBin*binFactor, -(thetaMax/100), thetaMax/100));
  thetaResolution[5]->setAxisTitle("rad",1);
  thetaResolution[5]->setAxisTitle("rad",2);
  thetaTrack = dbe->book1D("TkMuon_theta", "#theta_{TK}", thetaBin, thetaMin, thetaMax);
  thetaTrack->setAxisTitle("rad");
  thetaStaTrack = dbe->book1D("StaMuon_theta", "#theta_{STA}", thetaBin, thetaMin, thetaMax);
  thetaStaTrack->setAxisTitle("rad");

  // monitoring of phi paramater
  phiBin = parameters.getParameter<int>("phiBin");
  phiMin = parameters.getParameter<double>("phiMin");
  phiMax = parameters.getParameter<double>("phiMax");
  phiGlbTrack.push_back(dbe->book1D(histname+"Glb_phi", "#phi_{GLB}", phiBin, phiMin, phiMax));
  phiGlbTrack[0]->setAxisTitle("rad");
  phiGlbTrack.push_back(dbe->book1D(histname+"Tk_phi", "#phi_{TKfromGLB}", phiBin, phiMin, phiMax));
  phiGlbTrack[1]->setAxisTitle("rad");
  phiGlbTrack.push_back(dbe->book1D(histname+"Sta_phi", "#phi_{STAfromGLB}", phiBin, phiMin, phiMax));
  phiGlbTrack[2]->setAxisTitle("rad");
  phiResolution.push_back(dbe->book1D("Res_TkGlb_phi", "#phi_{TKfromGLB} - #phi_{GLB}", phiBin*binFactor, phiMin/3000, phiMax/3000));
  phiResolution[0]->setAxisTitle("rad");
  phiResolution.push_back(dbe->book1D("Res_GlbSta_phi", "#phi_{GLB} - #phi_{STAfromGLB}", phiBin*binFactor, phiMin/100, phiMax/100));
  phiResolution[1]->setAxisTitle("rad");
  phiResolution.push_back(dbe->book1D("Res_TkSta_phi", "#phi_{TKfromGLB} - #phi_{STAfromGLB}", phiBin*binFactor, phiMin/100, phiMax/100));
  phiResolution[2]->setAxisTitle("rad");
  phiResolution.push_back(dbe->book2D("ResVsPhi_TkGlb_phi", "(#phi_{TKfromGLB} - #phi_{GLB}) vs #phi_GLB", phiBin, phiMin, phiMax, phiBin*binFactor, phiMin/3000, phiMax/3000));
  phiResolution[3]->setAxisTitle("rad",1);
  phiResolution[3]->setAxisTitle("rad",2);
  phiResolution.push_back(dbe->book2D("ResVsPhi_GlbSta_phi", "(#phi_{GLB} - #phi_{STAfromGLB}) vs #phi_{GLB}", phiBin, phiMin, phiMax, phiBin*binFactor, phiMin/100, phiMax/100));
  phiResolution[4]->setAxisTitle("rad",1);
  phiResolution[4]->setAxisTitle("rad",2);
  phiResolution.push_back(dbe->book2D("ResVsPhi_TkSta_phi", "(#phi_{TKfromGLB} - #phi_{STAfromGLB}) vs #phi_{TKfromGLB}", phiBin, phiMin, phiMax, phiBin*binFactor, phiMin/100, phiMax/100));
  phiResolution[5]->setAxisTitle("rad",1);
  phiResolution[5]->setAxisTitle("rad",2);
  phiTrack = dbe->book1D("TkMuon_phi", "#phi_{TK}", phiBin, phiMin, phiMax);
  phiTrack->setAxisTitle("rad"); 
  phiStaTrack = dbe->book1D("StaMuon_phi", "#phi_{STA}", phiBin, phiMin, phiMax);
  phiStaTrack->setAxisTitle("rad"); 
  phiEfficiency.push_back(dbe->book1D("StaPhi", "#phi_{STAfromGLB}", phiBin, phiMin, phiMax));
  phiEfficiency[0]->setAxisTitle("rad"); 
  phiEfficiency.push_back(dbe->book1D("StaPhi_ifCombinedAlso", "#phi_{STAfromGLB} if the isGlb=true", phiBin, phiMin, phiMax));
  phiEfficiency[1]->setAxisTitle("rad"); 

  // monitoring of the momentum
  pBin = parameters.getParameter<int>("pBin");
  pMin = parameters.getParameter<double>("pMin");
  pMax = parameters.getParameter<double>("pMax");
  pGlbTrack.push_back(dbe->book1D(histname+"Glb_p", "p_{GLB}", pBin, pMin, pMax));
  pGlbTrack[0]->setAxisTitle("GeV"); 
  pGlbTrack.push_back(dbe->book1D(histname+"Tk_p", "p_{TKfromGLB}", pBin, pMin, pMax));
  pGlbTrack[1]->setAxisTitle("GeV");
  pGlbTrack.push_back(dbe->book1D(histname+"Sta_p", "p_{STAfromGLB}", pBin, pMin, pMax));
  pGlbTrack[2]->setAxisTitle("GeV");
  pTrack = dbe->book1D("TkMuon_p", "p_{TK}", pBin, pMin, pMax);
  pTrack->setAxisTitle("GeV"); 
  pStaTrack = dbe->book1D("StaMuon_p", "p_{STA}", pBin, pMin, pMax);
  pStaTrack->setAxisTitle("GeV"); 

  // monitoring of the transverse momentum
  ptBin = parameters.getParameter<int>("ptBin");
  ptMin = parameters.getParameter<double>("ptMin");
  ptMax = parameters.getParameter<double>("ptMax");
  ptGlbTrack.push_back(dbe->book1D(histname+"Glb_pt", "pt_{GLB}", ptBin, ptMin, ptMax));
  ptGlbTrack[0]->setAxisTitle("GeV"); 
  ptGlbTrack.push_back(dbe->book1D(histname+"Tk_pt", "pt_{TKfromGLB}", ptBin, ptMin, ptMax));
  ptGlbTrack[1]->setAxisTitle("GeV"); 
  ptGlbTrack.push_back(dbe->book1D(histname+"Sta_pt", "pt_{STAfromGLB}", ptBin, ptMin, ptMax));
  ptGlbTrack[2]->setAxisTitle("GeV"); 
  ptTrack = dbe->book1D("TkMuon_pt", "pt_{TK}", ptBin, ptMin, ptMax);
  ptTrack->setAxisTitle("GeV"); 
  ptStaTrack = dbe->book1D("StaMuon_pt", "pt_{STA}", ptBin, ptMin, pMax);
  ptStaTrack->setAxisTitle("GeV"); 

  // monitoring of the muon charge
  qGlbTrack.push_back(dbe->book1D(histname+"Glb_q", "q_{GLB}", 5, -2.5, 2.5));
  qGlbTrack.push_back(dbe->book1D(histname+"Tk_q", "q_{TKfromGLB}", 5, -2.5, 2.5));
  qGlbTrack.push_back(dbe->book1D(histname+"Sta_q", "q_{STAformGLB}", 5, -2.5, 2.5));
  qGlbTrack.push_back(dbe->book1D(histname+"qComparison", "comparison between q_{GLB} and q_{TKfromGLB}, q_{STAfromGLB}", 8, 0.5, 8.5));
  qGlbTrack[3]->setBinLabel(1,"qGlb=qSta");
  qGlbTrack[3]->setBinLabel(2,"qGlb!=qSta");
  qGlbTrack[3]->setBinLabel(3,"qGlb=qTk");
  qGlbTrack[3]->setBinLabel(4,"qGlb!=qTk");
  qGlbTrack[3]->setBinLabel(5,"qSta=qTk");
  qGlbTrack[3]->setBinLabel(6,"qSta!=qTk");
  qGlbTrack[3]->setBinLabel(7,"qGlb!=qSta,qGlb!=Tk");
  qGlbTrack[3]->setBinLabel(8,"qGlb=qSta,qGlb=Tk");
  qTrack = dbe->book1D("TkMuon_q", "q_{TK}", 5, -2.5, 2.5);
  qStaTrack = dbe->book1D("StaMuon_q", "q_{STA}", 5, -2.5, 2.5);

  // monitoring of the momentum resolution
  pResBin = parameters.getParameter<int>("pResBin");
  pResMin = parameters.getParameter<double>("pResMin");
  pResMax = parameters.getParameter<double>("pResMax");
  qOverpResolution.push_back(dbe->book1D("Res_TkGlb_qOverp", "(q/p)_{TKfromGLB} - (q/p)_{GLB}", pResBin*binFactor*2, pResMin/10, pResMax/10));
  qOverpResolution[0]->setAxisTitle("GeV^{-1}"); 
  qOverpResolution.push_back(dbe->book1D("Res_GlbSta_qOverp", "(q/p)_{GLB} - (q/p)_{STAfromGLB}", pResBin*binFactor, pResMin, pResMax));
  qOverpResolution[1]->setAxisTitle("GeV^{-1}"); 
  qOverpResolution.push_back(dbe->book1D("Res_TkSta_qOverp", "(q/p)_{TKfromGLB} - (q/p)_{STAfromGLB}", pResBin*binFactor, pResMin, pResMax));
  qOverpResolution[2]->setAxisTitle("GeV^{-1}"); 
  oneOverpResolution.push_back(dbe->book1D("Res_TkGlb_oneOverp", "(1/p)_{TKfromGLB} - (1/p)_{GLB}", pResBin*binFactor*2, pResMin/10, pResMax/10));
  oneOverpResolution[0]->setAxisTitle("GeV^{-1}"); 
  oneOverpResolution.push_back(dbe->book1D("Res_GlbSta_oneOverp", "(1/p)_{GLB} - (1/p)_{STAfromGLB}", pResBin*binFactor, pResMin, pResMax));
  oneOverpResolution[1]->setAxisTitle("GeV^{-1}");
  oneOverpResolution.push_back(dbe->book1D("Res_TkSta_oneOverp", "(q/p)_{TKfromGLB} - (q/p)_{STAfromGLB}", pResBin*binFactor, pResMin, pResMax));
  oneOverpResolution[2]->setAxisTitle("GeV^{-1}");
  qOverptResolution.push_back(dbe->book1D("Res_TkGlb_qOverpt", "(q/p_{t})_{TKfromGLB} - (q/p_{t})_{GLB}", pResBin*binFactor*2, pResMin/10, pResMax/10));
  qOverptResolution[0]->setAxisTitle("GeV^{-1}");  
  qOverptResolution.push_back(dbe->book1D("Res_GlbSta_qOverpt", "(q/p_{t})_{GLB} - (q/p_{t})_{STAfromGLB}", pResBin*binFactor, pResMin, pResMax));
  qOverptResolution[1]->setAxisTitle("GeV^{-1}"); 
  qOverptResolution.push_back(dbe->book1D("Res_TkSta_qOverpt", "(q/p_{t})_{TKfromGLB} - (q/p_{t})_{STAfromGLB}", pResBin*binFactor, pResMin, pResMax));
  qOverptResolution[1]->setAxisTitle("GeV^{-1}"); 
  oneOverptResolution.push_back(dbe->book1D("Res_TkGlb_oneOverpt", "(1/p_{t})_{TKfromGLB} - (1/p_{t})_{GLB}", pResBin*binFactor*2, pResMin/10, pResMax/10));
  oneOverptResolution[0]->setAxisTitle("GeV^{-1}");  
  oneOverptResolution.push_back(dbe->book1D("Res_GlbSta_oneOverpt", "(1/p_{t})_{GLB} - (1/p_{t})_{STAfromGLB}", pResBin*binFactor, pResMin, pResMax));
  oneOverptResolution[1]->setAxisTitle("GeV^{-1}");
  oneOverptResolution.push_back(dbe->book1D("Res_TkSta_oneOverpt", "(1/p_{t})_{TKfromGLB} - (1/p_{t})_{STAfromGLB}", pResBin*binFactor, pResMin, pResMax));
  oneOverptResolution[2]->setAxisTitle("GeV^{-1}");
  oneOverptResolution.push_back(dbe->book2D("ResVsEta_TkGlb_oneOverpt", "(#eta_{TKfromGLB} - #eta_{GLB}) vs (1/p_{t})_{GLB}", etaBin, etaMin, etaMax, pResBin*binFactor*2, pResMin/10, pResMax/10));
  oneOverptResolution[3]->setAxisTitle("GeV^{-1}",2);
  oneOverptResolution.push_back(dbe->book2D("ResVsEta_GlbSta_oneOverpt", "(#eta_{GLB} - #eta_{STAfromGLB} vs (1/p_{t})_{GLB}", etaBin, etaMin, etaMax, pResBin*binFactor, pResMin, pResMax));
  oneOverptResolution[4]->setAxisTitle("GeV^{-1}",2);
  oneOverptResolution.push_back(dbe->book2D("ResVsEta_TkSta_oneOverpt", "(#eta_{TKfromGLB} - #eta_{STAfromGLB}) vs (1/p_{t})_{TKfromGLB}", etaBin, etaMin, etaMax, pResBin*binFactor, pResMin, pResMax));
  oneOverptResolution[5]->setAxisTitle("GeV^{-1}",2);
  oneOverptResolution.push_back(dbe->book2D("ResVsPhi_TkGlb_oneOverpt", "(#phi_{TKfromGLB} - #phi_{GLB}) vs (1/p_{t})_{GLB}", phiBin, phiMin, phiMax, pResBin*binFactor*2, pResMin/10, pResMax/10));
  oneOverptResolution[6]->setAxisTitle("rad",1);
  oneOverptResolution[6]->setAxisTitle("GeV^{-1}",2);
  oneOverptResolution.push_back(dbe->book2D("ResVsPhi_GlbSta_oneOverpt", "(#phi_{GLB} - #phi_{STAfromGLB} vs (1/p_{t})_{GLB}", phiBin, phiMin, phiMax, pResBin*binFactor, pResMin, pResMax));
  oneOverptResolution[7]->setAxisTitle("rad",1);
  oneOverptResolution[7]->setAxisTitle("GeV^{-1}",2);
  oneOverptResolution.push_back(dbe->book2D("ResVsPhi_TkSta_oneOverpt", "(#phi_{TKfromGLB} - #phi_{STAfromGLB}) vs (1/p_{t})_{TKfromGLB}", phiBin, phiMin, phiMax, pResBin*binFactor, pResMin, pResMax));
  oneOverptResolution[8]->setAxisTitle("rad",1);
  oneOverptResolution[8]->setAxisTitle("GeV^{-1}",2);
  oneOverptResolution.push_back(dbe->book2D("ResVsPt_TkGlb_oneOverpt", "((1/p_{t})_{TKfromGLB} - (1/p_{t})_{GLB}) vs (1/p_{t})_{GLB}", ptBin/5, ptMin, ptMax/100, pResBin*binFactor*2, pResMin/10, pResMax/10));
  oneOverptResolution[9]->setAxisTitle("GeV^{-1}",1);
  oneOverptResolution[9]->setAxisTitle("GeV^{-1}",2);
  oneOverptResolution.push_back(dbe->book2D("ResVsPt_GlbSta_oneOverpt", "((1/p_{t})_{GLB} - (1/p_{t})_{STAfromGLB} vs (1/p_{t})_{GLB}", ptBin/5, ptMin, ptMax/100, pResBin*binFactor, pResMin, pResMax));
  oneOverptResolution[10]->setAxisTitle("GeV^{-1}",1);
  oneOverptResolution[10]->setAxisTitle("GeV^{-1}",2);
  oneOverptResolution.push_back(dbe->book2D("ResVsPt_TkSta_oneOverpt", "((1/p_{t})_{TKfromGLB} - (1/p_{t})_{STAfromGLB}) vs (1/p_{t})_{TKfromGLB}", ptBin/5, ptMin, ptMax/100, pResBin*binFactor, pResMin, pResMax));
  oneOverptResolution[11]->setAxisTitle("GeV^{-1}",1);
  oneOverptResolution[11]->setAxisTitle("GeV^{-1}",2);

  // monitoring of the recHits provenance
  rhBin=parameters.getParameter<int>("rhBin");
  rhMin=parameters.getParameter<double>("rhMin");
  rhMax=parameters.getParameter<double>("rhMax");
  rhAnalysis.push_back(dbe->book1D("StaRh_Frac_inGlb", "recHits_{STAinGLB} / recHits_{GLB}", rhBin, rhMin, rhMax));
  rhAnalysis.push_back(dbe->book1D("TkRh_Frac_inGlb", "recHits_{TKinGLB} / recHits_{GLB}", rhBin, rhMin, rhMax));
  rhAnalysis.push_back(dbe->book1D("StaRh_inGlb_Div_RhAssoSta", "recHits_{STAinGLB} / recHits_{STAfromGLB}", rhBin, rhMin, rhMax));
  rhAnalysis.push_back(dbe->book1D("TkRh_inGlb_Div_RhAssoTk", "recHits_{TKinGLB} / recHits_{TKfromGLB}", rhBin, rhMin, rhMax));
  rhAnalysis.push_back(dbe->book1D("GlbRh_Div_RhAssoStaTk", "recHits_{GLB} / (recHits_{TKfromGLB}+recHits_{STAfromGLB})", rhBin, rhMin, rhMax));
  rhAnalysis.push_back(dbe->book1D("invalidRh_Frac_inTk", "Invalid recHits / rechits_{GLB}", rhBin, rhMin, rhMax));

}


void MuonRecoAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup, const reco::Muon& recoMu) {

  LogTrace(metname)<<"[MuonRecoAnalyzer] Analyze the mu";

  if(recoMu.isGlobalMuon()) {

    LogTrace(metname)<<"[MuonRecoAnalyzer] The mu is global - filling the histos";
    if(recoMu.isTrackerMuon() && recoMu.isStandAloneMuon())
      muReco->Fill(1);
    if(!(recoMu.isTrackerMuon()) && recoMu.isStandAloneMuon())
      muReco->Fill(2);
    if(!recoMu.isStandAloneMuon())
      LogTrace(metname)<<"[MuonRecoAnalyzer] ERROR: the mu is global but not standalone!";

    // get the track combinig the information from both the Tracker and the Spectrometer
    reco::TrackRef recoCombinedGlbTrack = recoMu.combinedMuon();
    // get the track using only the tracker data
    reco::TrackRef recoTkGlbTrack = recoMu.track();
    // get the track using only the mu spectrometer data
    reco::TrackRef recoStaGlbTrack = recoMu.standAloneMuon();
  
    etaGlbTrack[0]->Fill(recoCombinedGlbTrack->eta());
    etaGlbTrack[1]->Fill(recoTkGlbTrack->eta());
    etaGlbTrack[2]->Fill(recoStaGlbTrack->eta());
    etaResolution[0]->Fill(recoTkGlbTrack->eta()-recoCombinedGlbTrack->eta());
    etaResolution[1]->Fill(-recoStaGlbTrack->eta()+recoCombinedGlbTrack->eta());
    etaResolution[2]->Fill(recoTkGlbTrack->eta()-recoStaGlbTrack->eta());
    etaResolution[3]->Fill(recoCombinedGlbTrack->eta(), recoTkGlbTrack->eta()-recoCombinedGlbTrack->eta());
    etaResolution[4]->Fill(recoCombinedGlbTrack->eta(), -recoStaGlbTrack->eta()+recoCombinedGlbTrack->eta());
    etaResolution[5]->Fill(recoCombinedGlbTrack->eta(), recoTkGlbTrack->eta()-recoStaGlbTrack->eta());

    thetaGlbTrack[0]->Fill(recoCombinedGlbTrack->theta());
    thetaGlbTrack[1]->Fill(recoTkGlbTrack->theta());
    thetaGlbTrack[2]->Fill(recoStaGlbTrack->theta());
    thetaResolution[0]->Fill(recoTkGlbTrack->theta()-recoCombinedGlbTrack->theta());
    thetaResolution[1]->Fill(-recoStaGlbTrack->theta()+recoCombinedGlbTrack->theta());
    thetaResolution[2]->Fill(recoTkGlbTrack->theta()-recoStaGlbTrack->theta());
    thetaResolution[3]->Fill(recoCombinedGlbTrack->theta(), recoTkGlbTrack->theta()-recoCombinedGlbTrack->theta());
    thetaResolution[4]->Fill(recoCombinedGlbTrack->theta(), -recoStaGlbTrack->theta()+recoCombinedGlbTrack->theta());
    thetaResolution[5]->Fill(recoCombinedGlbTrack->theta(), recoTkGlbTrack->theta()-recoStaGlbTrack->theta());
     
    phiGlbTrack[0]->Fill(recoCombinedGlbTrack->phi());
    phiGlbTrack[1]->Fill(recoTkGlbTrack->phi());
    phiGlbTrack[2]->Fill(recoStaGlbTrack->phi());
    phiResolution[0]->Fill(recoTkGlbTrack->phi()-recoCombinedGlbTrack->phi());
    phiResolution[1]->Fill(-recoStaGlbTrack->phi()+recoCombinedGlbTrack->phi());
    phiResolution[2]->Fill(recoTkGlbTrack->phi()-recoStaGlbTrack->phi());
    phiResolution[3]->Fill(recoCombinedGlbTrack->phi(), recoTkGlbTrack->phi()-recoCombinedGlbTrack->phi());
    phiResolution[4]->Fill(recoCombinedGlbTrack->phi(), -recoStaGlbTrack->phi()+recoCombinedGlbTrack->phi());
    phiResolution[5]->Fill(recoCombinedGlbTrack->phi(), recoTkGlbTrack->phi()-recoStaGlbTrack->phi());
    
    pGlbTrack[0]->Fill(recoCombinedGlbTrack->p());
    pGlbTrack[1]->Fill(recoTkGlbTrack->p());
    pGlbTrack[2]->Fill(recoStaGlbTrack->p());

    ptGlbTrack[0]->Fill(recoCombinedGlbTrack->pt());
    ptGlbTrack[1]->Fill(recoTkGlbTrack->pt());
    ptGlbTrack[2]->Fill(recoStaGlbTrack->pt());

    qGlbTrack[0]->Fill(recoCombinedGlbTrack->charge());
    qGlbTrack[1]->Fill(recoTkGlbTrack->charge());
    qGlbTrack[2]->Fill(recoStaGlbTrack->charge());
    if(recoCombinedGlbTrack->charge()==recoStaGlbTrack->charge()) qGlbTrack[3]->Fill(1);
    else qGlbTrack[3]->Fill(2);
    if(recoCombinedGlbTrack->charge()==recoTkGlbTrack->charge()) qGlbTrack[3]->Fill(3);
    else qGlbTrack[3]->Fill(4);
    if(recoStaGlbTrack->charge()==recoTkGlbTrack->charge()) qGlbTrack[3]->Fill(5);
    else qGlbTrack[3]->Fill(6);
    if(recoCombinedGlbTrack->charge()!=recoStaGlbTrack->charge() && recoCombinedGlbTrack->charge()!=recoTkGlbTrack->charge()) qGlbTrack[3]->Fill(7);
    if(recoCombinedGlbTrack->charge()==recoStaGlbTrack->charge() && recoCombinedGlbTrack->charge()==recoTkGlbTrack->charge()) qGlbTrack[3]->Fill(8);
    
    qOverpResolution[0]->Fill((recoTkGlbTrack->charge()/recoTkGlbTrack->p())-(recoCombinedGlbTrack->charge()/recoCombinedGlbTrack->p()));
    qOverpResolution[1]->Fill(-(recoStaGlbTrack->charge()/recoStaGlbTrack->p())+(recoCombinedGlbTrack->charge()/recoCombinedGlbTrack->p()));
    qOverpResolution[2]->Fill((recoTkGlbTrack->charge()/recoTkGlbTrack->p())-(recoStaGlbTrack->charge()/recoStaGlbTrack->p()));
    oneOverpResolution[0]->Fill((1/recoTkGlbTrack->p())-(1/recoCombinedGlbTrack->p()));
    oneOverpResolution[1]->Fill(-(1/recoStaGlbTrack->p())+(1/recoCombinedGlbTrack->p()));
    oneOverpResolution[2]->Fill((1/recoTkGlbTrack->p())-(1/recoStaGlbTrack->p()));
    qOverptResolution[0]->Fill((recoTkGlbTrack->charge()/recoTkGlbTrack->pt())-(recoCombinedGlbTrack->charge()/recoCombinedGlbTrack->pt()));
    qOverptResolution[1]->Fill(-(recoStaGlbTrack->charge()/recoStaGlbTrack->pt())+(recoCombinedGlbTrack->charge()/recoCombinedGlbTrack->pt()));
    qOverptResolution[2]->Fill((recoTkGlbTrack->charge()/recoTkGlbTrack->pt())-(recoStaGlbTrack->charge()/recoStaGlbTrack->pt()));
    oneOverptResolution[0]->Fill((1/recoTkGlbTrack->pt())-(1/recoCombinedGlbTrack->pt()));
    oneOverptResolution[1]->Fill(-(1/recoStaGlbTrack->pt())+(1/recoCombinedGlbTrack->pt()));
    oneOverptResolution[2]->Fill((1/recoTkGlbTrack->pt())-(1/recoStaGlbTrack->pt()));
    oneOverptResolution[3]->Fill(recoCombinedGlbTrack->eta(),(1/recoTkGlbTrack->pt())-(1/recoCombinedGlbTrack->pt()));
    oneOverptResolution[4]->Fill(recoCombinedGlbTrack->eta(),-(1/recoStaGlbTrack->pt())+(1/recoCombinedGlbTrack->pt()));
    oneOverptResolution[5]->Fill(recoCombinedGlbTrack->eta(),(1/recoTkGlbTrack->pt())-(1/recoStaGlbTrack->pt()));
    oneOverptResolution[6]->Fill(recoCombinedGlbTrack->phi(),(1/recoTkGlbTrack->pt())-(1/recoCombinedGlbTrack->pt()));
    oneOverptResolution[7]->Fill(recoCombinedGlbTrack->phi(),-(1/recoStaGlbTrack->pt())+(1/recoCombinedGlbTrack->pt()));
    oneOverptResolution[8]->Fill(recoCombinedGlbTrack->phi(),(1/recoTkGlbTrack->pt())-(1/recoStaGlbTrack->pt()));
    oneOverptResolution[9]->Fill(recoCombinedGlbTrack->pt(),(1/recoTkGlbTrack->pt())-(1/recoCombinedGlbTrack->pt()));
    oneOverptResolution[10]->Fill(recoCombinedGlbTrack->pt(),-(1/recoStaGlbTrack->pt())+(1/recoCombinedGlbTrack->pt()));
    oneOverptResolution[11]->Fill(recoCombinedGlbTrack->pt(),(1/recoTkGlbTrack->pt())-(1/recoStaGlbTrack->pt()));
    
    // valid hits Glb track
    double rhGlb = recoCombinedGlbTrack->found();
    // valid hits Glb track from Tracker
    double rhGlb_StaProvenance=0;
    // valid hits Glb track from Sta system
    double rhGlb_TkProvenance=0;
    for (trackingRecHit_iterator recHit = recoCombinedGlbTrack->recHitsBegin();
	 recHit!=recoCombinedGlbTrack->recHitsEnd(); ++recHit){
      if((*recHit)->isValid()){
	DetId id = (*recHit)->geographicalId();
	if (id.det() == DetId::Muon)
	  rhGlb_StaProvenance++;
	if (id.det() == DetId::Tracker)
	  rhGlb_TkProvenance++;
      }
    }
    // valid hits Sta track associated to Glb track
    double rhStaGlb = recoStaGlbTrack->recHitsSize();
    // valid hits Traker track associated to Glb track
    double rhTkGlb = recoTkGlbTrack->found();
    // invalid hits Traker track associated to Glb track
    double rhTkGlb_notValid = recoTkGlbTrack->lost();
   
    // fill the histos
    rhAnalysis[0]->Fill(rhGlb_StaProvenance/rhGlb);
    rhAnalysis[1]->Fill(rhGlb_TkProvenance/rhGlb);
    rhAnalysis[2]->Fill(rhGlb_StaProvenance/rhStaGlb);
    rhAnalysis[3]->Fill(rhGlb_TkProvenance/rhTkGlb);
    rhAnalysis[4]->Fill(rhGlb/(rhStaGlb+rhTkGlb));
    rhAnalysis[5]->Fill(rhTkGlb_notValid/rhGlb);
    
  }


  if(recoMu.isTrackerMuon() && !(recoMu.isGlobalMuon())) {
    LogTrace(metname)<<"[MuonRecoAnalyzer] The mu is tracker only - filling the histos";
     if(recoMu.isStandAloneMuon())
      muReco->Fill(3);
    if(!(recoMu.isStandAloneMuon()))
      muReco->Fill(4);

    // get the track using only the tracker data
    reco::TrackRef recoTrack = recoMu.track();

    etaTrack->Fill(recoTrack->eta());
    thetaTrack->Fill(recoTrack->theta());
    phiTrack->Fill(recoTrack->phi());
    pTrack->Fill(recoTrack->p());
    ptTrack->Fill(recoTrack->pt());
    qTrack->Fill(recoTrack->charge());
    
  }

  if(recoMu.isStandAloneMuon() && !(recoMu.isGlobalMuon())) {
    LogTrace(metname)<<"[MuonRecoAnalyzer] The mu is STA only - filling the histos";
    if(!(recoMu.isTrackerMuon()))
      muReco->Fill(5);
     
    // get the track using only the mu spectrometer data
    reco::TrackRef recoStaTrack = recoMu.standAloneMuon();

    etaStaTrack->Fill(recoStaTrack->eta());
    thetaStaTrack->Fill(recoStaTrack->theta());
    phiStaTrack->Fill(recoStaTrack->phi());
    pStaTrack->Fill(recoStaTrack->p());
    ptStaTrack->Fill(recoStaTrack->pt());
    qStaTrack->Fill(recoStaTrack->charge());

  }
    
  if(recoMu.isCaloMuon() && !(recoMu.isGlobalMuon()) && !(recoMu.isTrackerMuon()) && !(recoMu.isStandAloneMuon()))
    muReco->Fill(6);
  
  //efficiency plots
  
  // get the track using only the mu spectrometer data
  reco::TrackRef recoStaGlbTrack = recoMu.standAloneMuon();
  
  if(recoMu.isStandAloneMuon()){
    etaEfficiency[0]->Fill(recoStaGlbTrack->eta());
    phiEfficiency[0]->Fill(recoStaGlbTrack->phi());
  }
  if(recoMu.isStandAloneMuon() && recoMu.isGlobalMuon()){
    etaEfficiency[1]->Fill(recoStaGlbTrack->eta());
    phiEfficiency[1]->Fill(recoStaGlbTrack->phi());
  }




}
