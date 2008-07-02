std::cout <<"\t Getting the DT Geometry"<<std::endl;
edm::ESHandle<DTGeometry> dtGeo;
iSetup.get<MuonGeometryRecord>().get(dtGeo);
    
std::cout <<"\t Getting the DT Segments"<<std::endl;
edm::Handle<DTRecSegment4DCollection> all4DSegments;
iEvent.getByLabel(dt4DSegments, all4DSegments);

if(all4DSegments->size()>0){
  std::cout<<"\t Number of Segments in this event = "<<all4DSegments->size()<<std::endl; 
  
  std::map<DTChamberId,int> scounter;
  DTRecSegment4DCollection::const_iterator segment;  
  
  for (segment = all4DSegments->begin();segment!=all4DSegments->end(); ++segment){
    scounter[segment->chamberId()]++;
  }    
  
  std::cout<<"\t Loop over all the 4D Segments"<<std::endl;
  for (segment = all4DSegments->begin(); segment != all4DSegments->end(); ++segment){ 
    
    DTChamberId DTId = segment->chamberId();
    
    std::cout<<"\t \t This Segment is in Chamber id: "<<DTId<<std::endl;
    std::cout<<"\t \t Number of segments in this DT = "<<scounter[DTId]<<std::endl;
    std::cout<<"\t \t Is the only one in this DT? and is not in the 4th Station?"<<std::endl;
    
    if(scounter[DTId]==1 && DTId.station()!=4){	
      std::cout<<"\t \t yes"<<std::endl;
 
      int dtWheel = DTId.wheel();
      int dtStation = DTId.station();
      int dtSector = DTId.sector();      

      LocalPoint segmentPosition= segment->localPosition();
      LocalVector segmentDirection=segment->localDirection();
      
      const GeomDet* gdet=dtGeo->idToDet(segment->geographicalId());
      const BoundPlane & DTSurface = gdet->surface();
      
      //check if the dimension of the segment is 4 
      std::cout<<"\t \t Is the segment 4D?"<<std::endl;
      
      if(segment->dimension()==4){
	std::cout<<"\t \t yes"<<std::endl;
	std::cout<<"\t \t DT Segment Dimension "<<segment->dimension()<<std::endl; 
	
	float Xo=segmentPosition.x();
	float Yo=segmentPosition.y();
	float Zo=segmentPosition.z();
	float dx=segmentDirection.x();
	float dy=segmentDirection.y();
	float dz=segmentDirection.z();

	std::cout<<"\t \t Loop over all the rolls asociated to this DT"<<std::endl;
	std::set<RPCDetId> rollsForThisDT = rollstoreDT[DTStationIndex(0,dtWheel,dtSector,dtStation)];
	std::cout<<"\t \t Number of rolls for this DT = "<<rollsForThisDT.size()<<std::endl;
       
	assert(rollsForThisDT.size()>=1);

	//Loop over all the rolls
	for (std::set<RPCDetId>::iterator iteraRoll = rollsForThisDT.begin();iteraRoll != rollsForThisDT.end(); iteraRoll++){
	  const RPCRoll* rollasociated = rpcGeo->roll(*iteraRoll);
	  RPCDetId rpcId = rollasociated->id();
	  std::cout<<"\t \t \t We are in the roll getting the surface"<<rpcId<<std::endl;
	  const BoundPlane & RPCSurface = rollasociated->surface(); 
	  
	  std::cout<<"\t \t \t RollID: "<<rollasociated->id()<<std::endl;

	  std::cout<<"\t \t \t Doing the extrapolation to this roll"<<std::endl;
	  std::cout<<"\t \t \t DT Segment Direction in DTLocal "<<segmentDirection<<std::endl;
	  std::cout<<"\t \t \t DT Segment Point in DTLocal "<<segmentPosition<<std::endl;  
	  GlobalPoint CenterPointRollGlobal = RPCSurface.toGlobal(LocalPoint(0,0,0));
	  std::cout<<"\t \t \t Center (0,0,0) of the Roll in Global"<<CenterPointRollGlobal<<std::endl;
	  
	  LocalPoint CenterRollinDTFrame = DTSurface.toLocal(CenterPointRollGlobal);
	  std::cout<<"\t \t \t Center (0,0,0) Roll In DTLocal"<<CenterRollinDTFrame<<std::endl;
	    
	  float D=CenterRollinDTFrame.z();
	  
	  float X=Xo+dx*D/dz;
	  float Y=Yo+dy*D/dz;
	  float Z=D;
	
	  const RectangularStripTopology* top_= dynamic_cast<const RectangularStripTopology*> (&(rollasociated->topology()));
	  LocalPoint xmin = top_->localPosition(0.);
	  std::cout<<"\t \t \t xmin of this  Roll "<<xmin<<"cm"<<std::endl;
	  LocalPoint xmax = top_->localPosition((float)rollasociated->nstrips());
	  std::cout<<"\t \t \t xmax of this  Roll "<<xmax<<"cm"<<std::endl;
	  float rsize = fabs( xmax.x()-xmin.x() );
	  std::cout<<"\t \t \t Roll Size "<<rsize<<"cm"<<std::endl;
	  float stripl = top_->stripLength();
	  float stripw = top_->pitch();
	  std::cout<<"\t \t \t Strip Lenght "<<stripl<<"cm"<<std::endl;
	  std::cout<<"\t \t \t Strip Width "<<stripw<<"cm"<<std::endl;
	  	  
	  std::cout<<"\t \t \t X Predicted in DTLocal= "<<X<<"cm"<<std::endl;
	  std::cout<<"\t \t \t Y Predicted in DTLocal= "<<Y<<"cm"<<std::endl;
	  std::cout<<"\t \t \t Z Predicted in DTLocal= "<<Z<<"cm"<<std::endl;
	  
	  float extrapolatedDistance = sqrt((X-Xo)*(X-Xo)+(Y-Yo)*(Y-Yo)+(Z-Zo)*(Z-Zo));
	  std::cout<<"\t \t \t Is the distance of extrapolation less than MaxD? ="<<extrapolatedDistance<<"cm"<<"MaxD="<<MaxD<<"cm"<<std::endl;
	  if(extrapolatedDistance<=MaxD){ 
	    std::cout<<"\t \t \t yes"<<std::endl;
	    
	    GlobalPoint GlobalPointExtrapolated = DTSurface.toGlobal(LocalPoint(X,Y,Z));
	    std::cout<<"\t \t \t Point ExtraPolated in Global"<<GlobalPointExtrapolated<< std::endl;
	    
	    LocalPoint PointExtrapolatedRPCFrame = RPCSurface.toLocal(GlobalPointExtrapolated);
	    std::cout<<"\t \t \t Point Extrapolated in RPCLocal"<<PointExtrapolatedRPCFrame<< std::endl;
	    std::cout<<"\t \t \t Does the extrapolation go inside this roll?"<<std::endl;
	    

	    if(fabs(PointExtrapolatedRPCFrame.z()) < 0.01 && 
	       fabs(PointExtrapolatedRPCFrame.x()) < rsize*0.5 && 
	       fabs(PointExtrapolatedRPCFrame.y()) < stripl*0.5){
	      
	      std::cout<<"\t \t \t \t yes"<<std::endl;	
	      RPCDetId  rollId = rollasociated->id();

	      const float stripPredicted = 
		rollasociated->strip(LocalPoint(PointExtrapolatedRPCFrame.x(),PointExtrapolatedRPCFrame.y(),0.)); 
		
	      std::cout<<"\t \t \t \t Candidate"<<rollId<<" "<<"(from DT Segment) STRIP---> "<<stripPredicted<< std::endl;
		
	      		
	      //--------- HISTOGRAM STRIP PREDICTED FROM DT  -------------------
		
	      RPCGeomServ rpcsrv(rollId);
	      std::string nameRoll = rpcsrv.name();
	      std::cout<<"\t \t \t \t The RPCName is "<<nameRoll<<std::endl;
	      //if(_idList.at(nameRoll)==null) 
	      bool deja=false;
	      std::vector<std::string>::iterator meIt;
	      for(meIt = _idList.begin(); meIt != _idList.end(); ++meIt){
		if(*meIt==nameRoll){ 
		  deja=true;
		  break;
		}
	      }
	      if(!deja){
		std::cout<<"\t \t \t \t NOT Found in Id List!"<<nameRoll<<std::endl;
		_idList.push_back(nameRoll);
		std::cout<<"\t \t \t \t Filling Id List with "<<nameRoll<<std::endl;
		std::cout<<"\t \t \t \t Id List Size "<<_idList.size()<<std::endl;
	      }
		
	      char detUnitLabel[128];
	      sprintf(detUnitLabel ,"%s",nameRoll.c_str());
	      sprintf(layerLabel ,"%s",nameRoll.c_str());
		
	      std::map<std::string, std::map<std::string,MonitorElement*> >::iterator meItr = meCollection.find(nameRoll);
				
	      if (meItr == meCollection.end()){
		meCollection[nameRoll] = bookDetUnitSeg(rollId,rollasociated->nstrips());
	      }
		
	      std::map<std::string, MonitorElement*> meMap=meCollection[nameRoll];
		
	      sprintf(meIdDT,"ExpectedOccupancyFromDT_%s",detUnitLabel);
	      meMap[meIdDT]->Fill(stripPredicted);
		
	      //-----------------------------------------------------
		
	      totalcounter[0]++;
	      buff=counter[0];
	      buff[rollId]++;
	      counter[0]=buff;
		
	      bool anycoincidence=false;
	      double sumStripDetected = 0.;  

	      int stripDetected = 0;
	      int stripCounter = 0;
	      std::cout<<"\t \t \t \t Getting Digis in Roll Asociated"<<std::endl;	
	      RPCDigiCollection::Range rpcRangeDigi=rpcDigis->get(rollasociated->id());

	      std::cout<<"\t \t \t \t \t Loop over the digis in this roll looking for the Average"<<std::endl;

	      for (RPCDigiCollection::const_iterator digiIt = rpcRangeDigi.first;digiIt!=rpcRangeDigi.second;++digiIt){
		stripDetected=digiIt->strip(); 
		if(fabs((float)stripDetected-stripPredicted)<MaxStripToCountInAverage){
		  sumStripDetected=sumStripDetected+stripDetected;
		  stripCounter++;
		}
		std::cout<<"\t \t \t \t \t \t digi "<<*digiIt<<"\t Detected="<<stripDetected<<" Predicted="<<stripPredicted<<" tmpRes(strips)="<<fabs((float)stripDetected-stripPredicted)<<"\t SumStrip= "<<sumStripDetected<<std::endl;
		
		std::cout<<"\t \t \t \t \t \t Filling BX Distribution"<<std::endl;
		sprintf(meIdRPC,"BXDistribution_%s",detUnitLabel);
		meMap[meIdRPC]->Fill(digiIt->bx());
		
		sprintf(meIdRPC,"RealDetectedOccupancyFromDT_%s",detUnitLabel);
		meMap[meIdRPC]->Fill(stripDetected); //have a look to this!
	      }
	      
	      std::cout<<"\t \t \t \t \t Sum of strips "<<sumStripDetected<<std::endl;
	      
     	      if(stripCounter!=0){
		double meanStripDetected = sumStripDetected/((double)stripCounter);
		
		std::cout<<"\t \t \t \t \t Number of strips "<<stripCounter<<" Strip Average Detected"<<meanStripDetected<<std::endl;
	      
		LocalPoint meanstripDetectedLocalPoint = top_->localPosition((float)(meanStripDetected)-0.5);
	      
		float meanrescms = PointExtrapolatedRPCFrame.x()-meanstripDetectedLocalPoint.x();          
			      
		std::cout<<"\t \t \t \t \t PointExtrapolatedRPCFrame.x="<<PointExtrapolatedRPCFrame.x()<<" meanstripDetectedLocalPoint.x="<<meanstripDetectedLocalPoint.x()<<std::endl;

		if(fabs(meanrescms) < MinimalResidual ){
		
		  std::cout<<"\t \t \t \t \t MeanRes="<<meanrescms<<"cm  MinimalResidual="<<MinimalResidual<<"cm"<<std::endl;
		
		  std::cout <<"\t \t \t \t \t COINCIDENCE Predict "<<stripPredicted<<"  (int)Predicted="<<(int)(stripPredicted)<<"  MeanDetected="<<meanStripDetected<<std::endl;
		  anycoincidence=true;
		  std::cout <<"\t \t \t \t \t Increassing DT counter"<<std::endl;
		  totalcounter[1]++;
		  buff=counter[1];
		  buff[rollId]++;
		  counter[1]=buff;
		  
		  sprintf(meIdRPC,"RPCDataOccupancyFromDT_%s",detUnitLabel);
		  meMap[meIdRPC]->Fill(stripPredicted);
		}
	      }else{
		std::cout <<"\t \t \t \t \t THIS ROLL DOESN'T HAVE ANY DIGI"<<std::endl;
	      }
	      
	      if(anycoincidence==false) {
		std::cout <<"\t \t \t \t \t THIS PREDICTION DOESN'T MATCH WITH THE DATA"<<std::endl;
		totalcounter[2]++;
		buff=counter[2];
		buff[rollId]++;
		counter[2]=buff;		
		std::cout << "\t \t \t \t \t One for counterFAIL"<<std::endl;
		  
		ofrej<<"DTs \t Wh "<<dtWheel
		     <<"\t St "<<dtStation
		     <<"\t Se "<<dtSector
		     <<"\t Roll "<<rollasociated->id()
		     <<"\t Event "
		     <<iEvent.id().event()
           	     <<"\t Run "	
		     <<iEvent.id().run()	
		     <<std::endl;
	      }
	    }else {
	      std::cout<<"\t \t \t \t No the prediction is outside of this roll"<<std::endl;
	    }//Condition for the right match
	  }else{
	    std::cout<<"\t \t \t No, Exrtrapolation too long!, canceled"<<std::endl;
	  }//D so big
	}//loop over all the rolls asociated
      }//Is the segment 4D?
    }else {
      std::cout<<"\t \t More than one segment in this chamber, or we are in Station 4"<<std::endl;
    }
  }
}
else {
  std::cout<<"This Event doesn't have any DT4DDSegment"<<std::endl; //is ther more than 1 segment in this event?
}
