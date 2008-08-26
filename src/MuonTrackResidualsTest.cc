
/*
 *  See header file for a description of this class.
 *
 *  $Date: 2008/07/16 13:20:43 $
 *  $Revision: 1.6 $
 *  \author G. Mila - INFN Torino
 */


#include <DQMOffline/Muon/src/MuonTrackResidualsTest.h>

// Framework
#include <FWCore/Framework/interface/Event.h>
#include "DataFormats/Common/interface/Handle.h" 
#include <FWCore/Framework/interface/ESHandle.h>
#include <FWCore/Framework/interface/MakerMacros.h>
#include <FWCore/Framework/interface/EventSetup.h>
#include <FWCore/ParameterSet/interface/ParameterSet.h>


#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/MonitorElement.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include <iostream>
#include <stdio.h>
#include <string>
#include <math.h>
#include "TF1.h"

using namespace edm;
using namespace std;


MuonTrackResidualsTest::MuonTrackResidualsTest(const edm::ParameterSet& ps){

  cout << "[MuonTrackResidualsTest]: Constructor called!"<<endl;
  parameters = ps;

  theDbe = edm::Service<DQMStore>().operator->();
 
  prescaleFactor = parameters.getUntrackedParameter<int>("diagnosticPrescale", 1);

}


MuonTrackResidualsTest::~MuonTrackResidualsTest(){

  LogTrace(metname) << "DTResolutionTest: analyzed " << nevents << " events";

}


void MuonTrackResidualsTest::beginJob(const edm::EventSetup& context){

  metname = "trackResidualsTest";
  theDbe->setCurrentFolder("Muons/Tests/trackResidualsTest");

  cout<<"[MuonTrackResidualsTest] Parameters initialization"<<endl;
 

  string histName, MeanHistoName, SigmaHistoName;
  vector<string> type;
  type.push_back("eta");
  type.push_back("theta");
  type.push_back("phi");


  for(unsigned int c=0; c<type.size(); c++){

    MeanHistoName =  "MeanTest_" + type[c]; 
    SigmaHistoName =  "SigmaTest_" + type[c];
 
    histName = "Res_GlbSta_"+type[c];
    histoNames[type[c]].push_back(histName);
    histName = "Res_TkGlb_"+type[c];
    histoNames[type[c]].push_back(histName);
    histName = "Res_TkSta_"+type[c];
    histoNames[type[c]].push_back(histName);

    
    MeanHistos[type[c]] = theDbe->book1D(MeanHistoName.c_str(),MeanHistoName.c_str(),3,0.5,3.5);
    (MeanHistos[type[c]])->setBinLabel(1,"Res_StaGlb",1);
    (MeanHistos[type[c]])->setBinLabel(2,"Res_TkGlb",1);
    (MeanHistos[type[c]])->setBinLabel(3,"Res_TkSta",1);
    
    
    SigmaHistos[type[c]] = theDbe->book1D(SigmaHistoName.c_str(),SigmaHistoName.c_str(),3,0.5,3.5);
    (SigmaHistos[type[c]])->setBinLabel(1,"Res_StaGlb",1);  
    (SigmaHistos[type[c]])->setBinLabel(2,"Res_TkGlb",1);
    (SigmaHistos[type[c]])->setBinLabel(3,"Res_TkSta",1);
    
  }

  nevents = 0;

}


void MuonTrackResidualsTest::beginLuminosityBlock(LuminosityBlock const& lumiSeg, EventSetup const& context) {

  LogTrace(metname)<<"[MuonTrackResidualsTest]: Begin of LS transition";

  // Get the run number
  run = lumiSeg.run();

}


void MuonTrackResidualsTest::analyze(const edm::Event& e, const edm::EventSetup& context){

  nevents++;
  LogTrace(metname)<< "[MuonTrackResidualsTest]: "<<nevents<<" events";

}



void MuonTrackResidualsTest::endLuminosityBlock(LuminosityBlock const& lumiSeg, EventSetup const& context) {

  cout<<"[MuonTrackResidualsTest]: End of LS transition, performing the DQM client operation"<<endl;

  // counts number of lumiSegs 
  nLumiSegs = lumiSeg.id().luminosityBlock();
  
  // prescale factor
  if ( nLumiSegs%prescaleFactor != 0 ) return;
  
  
  for(map<string, vector<string> > ::const_iterator histo = histoNames.begin();
      histo != histoNames.end();
      histo++) {
   
    for (unsigned int type=0; type< (*histo).second.size(); type++){

      string path = "Muons/MuonRecoAnalyzer/" + (*histo).second[type];
      MonitorElement * res_histo = theDbe->get(path);
      if (res_histo) {
 
	// gaussian test
	string GaussianCriterionName = 
	  parameters.getUntrackedParameter<string>("resDistributionTestName",
						   "ResidualsDistributionGaussianTest");
	const QReport * GaussianReport = res_histo->getQReport(GaussianCriterionName);
	if(GaussianReport){
	  cout<< "-------- histo : "<<(*histo).second[type]<<"  "<<GaussianReport->getMessage()<<" ------- "<<GaussianReport->getStatus()<<endl;
	}
	int BinNumber = type+1;
	float mean = (*res_histo).getMean(1);
	float sigma = (*res_histo).getRMS(1);
	cout<<"mean: "<<mean<<endl;
	cout<<"sigma: "<<sigma<<endl;
	MeanHistos.find((*histo).first)->second->setBinContent(BinNumber, mean);	
	SigmaHistos.find((*histo).first)->second->setBinContent(BinNumber, sigma);
      }
    }
  }


  // Mean test 
  string MeanCriterionName = parameters.getUntrackedParameter<string>("meanTestName","ResidualsMeanInRange"); 
  for(map<string, MonitorElement*>::const_iterator hMean = MeanHistos.begin();
      hMean != MeanHistos.end();
      hMean++) {
    const QReport * theMeanQReport = (*hMean).second->getQReport(MeanCriterionName);
    if(theMeanQReport) {
      vector<dqm::me_util::Channel> badChannels = theMeanQReport->getBadChannels();
      for (vector<dqm::me_util::Channel>::iterator channel = badChannels.begin(); 
	   channel != badChannels.end(); channel++) {
	cout << "type:"<<(*hMean).first<<" Bad mean channels: "<<(*channel).getBin()<<"  Contents : "<<(*channel).getContents()<<endl;
      }
      cout << "-------- type: "<<(*hMean).first<<"  "<<theMeanQReport->getMessage()<<" ------- "<<theMeanQReport->getStatus()<<endl; 
    }
  }
  
  // Sigma test
  string SigmaCriterionName = parameters.getUntrackedParameter<string>("sigmaTestName","ResidualsSigmaInRange"); 
  for(map<string, MonitorElement*>::const_iterator hSigma = SigmaHistos.begin();
      hSigma != SigmaHistos.end();
      hSigma++) {
    const QReport * theSigmaQReport = (*hSigma).second->getQReport(SigmaCriterionName);
    if(theSigmaQReport) {
      vector<dqm::me_util::Channel> badChannels = theSigmaQReport->getBadChannels();
      for (vector<dqm::me_util::Channel>::iterator channel = badChannels.begin(); 
	   channel != badChannels.end(); channel++) {
	cout << "type:"<<(*hSigma).first<<" Bad sigma channels: "<<(*channel).getBin()<<" Contents : "<<(*channel).getContents()<<endl;
      }
      cout<< "-------- type: "<<(*hSigma).first<<"  "<<theSigmaQReport->getMessage()<<" ------- "<<theSigmaQReport->getStatus()<<endl;
    }
  }
}




void MuonTrackResidualsTest::endJob(){
  
  LogTrace(metname)<< "[MuonTrackResidualsTest] endjob called!";
  theDbe->rmdir("Muons/Tests/trackResidualsTest");
  
}



