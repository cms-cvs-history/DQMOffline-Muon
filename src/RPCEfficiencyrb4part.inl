std::cout <<"MB4 \t Getting the DT Geometry"<<std::endl;
edm::ESHandle<DTGeometry> dtGeo;
iSetup.get<MuonGeometryRecord>().get(dtGeo);
   
std::cout <<"MB4 \t Getting ALL the DT Segments"<<std::endl;
edm::Handle<DTRecSegment4DCollection> all4DSegments;
iEvent.getByLabel(dt4DSegments, all4DSegments);
    
if(all4DSegments->size()>0){
  
  std::map<DTChamberId,int> scounter;
  DTRecSegment4DCollection::const_iterator segment;  
  
  for (segment = all4DSegments->begin();segment!=all4DSegments->end(); ++segment){
    scounter[segment->chamberId()]++;
  }    
  
  for (segment = all4DSegments->begin(); segment != all4DSegments->end(); ++segment){ 
    
    DTChamberId DTId = segment->chamberId();
    
    
    if(scounter[DTId] == 1 && DTId.station()==4){
      int dtWheel = DTId.wheel();
      int dtStation = DTId.station();
      int dtSector = DTId.sector();
      
      LocalPoint segmentPosition= segment->localPosition();
      LocalVector segmentDirection=segment->localDirection();
            
      //check if the dimension of the segment is 2 and the station is 4
            
      std::cout<<"MB4 \t \t Is the segment 2D? and is in the Station 4?"<<std::endl;

      if(segment->dimension()==2){
	  
	LocalVector segmentDirectionMB4=segmentDirection;
	LocalPoint segmentPositionMB4=segmentPosition;
	
	bool compatiblesegments=false;
	float dx=segmentDirectionMB4.x();
	float dz=segmentDirectionMB4.z();
	
	const BoundPlane& DTSurface4 = dtGeo->idToDet(DTId)->surface();
	
	
	DTRecSegment4DCollection::const_iterator segMB3;  
	
	for(segMB3=all4DSegments->begin();segMB3!=all4DSegments->end();++segMB3){

	  DTChamberId dtid3 = segMB3->chamberId();  
	  if(dtid3.station()==3&&dtid3.wheel()==DTId.wheel()&&scounter[dtid3] == 1&&segMB3->dimension()==4){

	    const GeomDet* gdet3=dtGeo->idToDet(segMB3->geographicalId());
	    const BoundPlane & DTSurface3 = gdet3->surface();
	      
	    float dx3=segMB3->localDirection().x();
	    float dy3=segMB3->localDirection().y();
	    float dz3=segMB3->localDirection().z();
	    
	    LocalVector segDirMB4inMB3Frame=DTSurface3.toLocal(DTSurface4.toGlobal(segmentDirectionMB4));
	    
	    double cosAng=fabs(dx*dx3+dz*dz3/sqrt((dx3*dx3+dz3*dz3)*(dx*dx+dz*dz)));
	    assert(fabs(cosAng)<=1.);
	    
	    if(cosAng>MinCosAng){
	      compatiblesegments=true;
	      if(dtSector==13){
		dtSector=4;
	      }
	      if(dtSector==14){
		dtSector=10;
	      }
	    
	      std::set<RPCDetId> rollsForThisDT = rollstoreDT[DTStationIndex(0,dtWheel,dtSector,4)]; //It should be always 4
	      
	      assert(rollsForThisDT.size()>=1);
     	      
	      for (std::set<RPCDetId>::iterator iteraRoll=rollsForThisDT.begin();iteraRoll != rollsForThisDT.end(); iteraRoll++){
		const RPCRoll* rollasociated = rpcGeo->roll(*iteraRoll); //roll asociado a MB4
		const BoundPlane & RPCSurfaceRB4 = rollasociated->surface(); //surface MB4
		const GeomDet* gdet=dtGeo->idToDet(segMB3->geographicalId()); 
		const BoundPlane & DTSurfaceMB3 = gdet->surface(); // surface MB3
		
		GlobalPoint CenterPointRollGlobal=RPCSurfaceRB4.toGlobal(LocalPoint(0,0,0));
		
		LocalPoint CenterRollinMB3Frame = DTSurfaceMB3.toLocal(CenterPointRollGlobal);

		float D=CenterRollinMB3Frame.z();
		
		float Xo3=segMB3->localPosition().x();
		float Yo3=segMB3->localPosition().y();
		float Zo3=segMB3->localPosition().z();

		float X=Xo3+dx3*D/dz3;
		float Y=Yo3+dy3*D/dz3;
		float Z=D;

		
		const RectangularStripTopology* top_
		  =dynamic_cast<const RectangularStripTopology*>(&(rollasociated->topology())); //Topology roll asociated MB4
		LocalPoint xmin = top_->localPosition(0.);
		std::cout<<"MB4 \t \t \t xmin of this  Roll "<<xmin<<"cm"<<std::endl;
		LocalPoint xmax = top_->localPosition((float)rollasociated->nstrips());
		std::cout<<"MB4 \t \t \t xmax of this  Roll "<<xmax<<"cm"<<std::endl;
		float rsize = fabs( xmax.x()-xmin.x() );
		std::cout<<"MB4 \t \t \t Roll Size "<<rsize<<"cm"<<std::endl;
		float stripl = top_->stripLength();
		float stripw = top_->pitch();
		std::cout<<"MB4 \t \t \t Strip Lenght "<<stripl<<"cm"<<std::endl;
		std::cout<<"MB4 \t \t \t Strip Width "<<stripw<<"cm"<<std::endl;

		std::cout<<"MB4 \t \t \t X Predicted in MB3Local= "<<X<<"cm"<<std::endl;
		std::cout<<"MB4 \t \t \t Y Predicted in MB3DTLocal= "<<Y<<"cm"<<std::endl;
		std::cout<<"MB4 \t \t \t Z Predicted in MB3DTLocal= "<<Z<<"cm"<<std::endl;
		

		float extrapolatedDistance = sqrt((X-Xo3)*(X-Xo3)+(Y-Yo3)*(Y-Yo3)+(Z-Zo3)*(Z-Zo3));
		std::cout<<"MB4 \t \t \t Is the distance of extrapolation less than MaxD? ="<<extrapolatedDistance<<"cm"<<"MaxDrb4="<<MaxDrb4<<"cm"<<std::endl;

		if(extrapolatedDistance<=MaxDrb4){ 
		  std::cout<<"MB4 \t \t \t yes"<<std::endl;
		  
		  GlobalPoint GlobalPointExtrapolated = DTSurfaceMB3.toGlobal(LocalPoint(X,Y,Z));
		  std::cout<<"MB4 \t \t \t Point ExtraPolated in Global"<<GlobalPointExtrapolated<< std::endl;

		  LocalPoint PointExtrapolatedRPCFrame = RPCSurfaceRB4.toLocal(GlobalPointExtrapolated);
		  std::cout<<"MB4 \t \t \t Point Extrapolated in RPCLocal"<<PointExtrapolatedRPCFrame<< std::endl;
		  std::cout<<"MB4 \t \t \t Does the extrapolation go inside this roll?"<<std::endl;
	    
		
		  if(fabs(PointExtrapolatedRPCFrame.z()) < 0.01  &&
		   fabs(PointExtrapolatedRPCFrame.x()) < rsize*0.5 &&
		   fabs(PointExtrapolatedRPCFrame.y()) < stripl*0.5){ 
		    
		    std::cout<<"MB4 \t \t \t \t yes"<<std::endl;	
		    RPCDetId  rollId = rollasociated->id();
		    
		    const float stripPredicted=
		      rollasociated->strip(LocalPoint(PointExtrapolatedRPCFrame.x(),PointExtrapolatedRPCFrame.y(),0.)); 
		  
		    
		    std::cout<<"MB4 \t \t \t \t Candidate"<<rollId<<" "<<"(from DT Segment) STRIP---> "<<stripPredicted<< std::endl;
		    

		    //--------- HISTOGRAM STRIP PREDICTED FROM DT  -------------------
		  
		    RPCGeomServ rpcsrv(rollId);
		    std::string nameRoll = rpcsrv.name();
		    std::cout<<"MB4 \t \t \t \t The RPCName is "<<nameRoll<<std::endl;
		  
		    bool deja=false;
	            std::vector<std::string>::iterator meIt;
	            for(meIt = _idList.begin(); meIt != _idList.end(); ++meIt){
		      if(*meIt==nameRoll){ 
			deja=true;
			break;
		      }
		    }
		    if(!deja){
		      std::cout<<"MB4 \t \t \t \t NOT Found in Id List!"<<nameRoll<<std::endl;
		      _idList.push_back(nameRoll);
		      std::cout<<"MB4 \t \t \t \t Filling Id List with "<<nameRoll<<std::endl;
		      std::cout<<"MB4 \t \t \t \t Id List Size "<<_idList.size()<<std::endl;
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
		    
		    
		    //-------------------------------------------------
		    
		    totalcounter[0]++;
		    buff=counter[0];
		    buff[rollId]++;
		    counter[0]=buff;		
		    
		    bool anycoincidence=false;
		    double sumStripDetected = 0.;  
		    
		    int stripDetected = 0;
		    int stripCounter = 0;
		    std::cout<<"MB4 \t \t \t \t Getting Digis in Roll Asociated"<<std::endl;
		    RPCDigiCollection::Range rpcRangeDigi = rpcDigis->get(rollasociated->id());
		    
		    std::cout<<"MB4 \t \t \t \t \t Loop over the digis in this roll looking for the Average"<<std::endl;
		    
		    for (RPCDigiCollection::const_iterator digiIt = rpcRangeDigi.first;digiIt!=rpcRangeDigi.second;++digiIt){
		      stripDetected=digiIt->strip(); 
		      if(fabs((float)stripDetected-stripPredicted)<MaxStripToCountInAverageRB4){
			sumStripDetected=sumStripDetected+stripDetected;
			stripCounter++;
		      }
		      std::cout<<"MB4 \t \t \t \t \t \t Digi "<<*digiIt<<"\t Detected="<<stripDetected<<" Predicted="<<stripPredicted<<" tmpRes(strips)="<<fabs((float)stripDetected-stripPredicted)<<"\t SumStrip= "<<sumStripDetected<<std::endl;
		      
		      std::cout<<"MB4 \t \t \t \t \t \t Filling BX Distribution"<<std::endl;
		      sprintf(meIdRPC,"BXDistribution_%s",detUnitLabel);
		      meMap[meIdRPC]->Fill(digiIt->bx());

		      sprintf(meIdRPC,"RealDetectedOccupancyFromDT_%s",detUnitLabel);
		      meMap[meIdRPC]->Fill(stripDetected);
		      
		    }
		    
		    std::cout<<"MB4 \t \t \t \t \t Sum of strips "<<sumStripDetected<<std::endl;
		    
		    if(stripCounter!=0){
		      double meanStripDetected = meanStripDetected=sumStripDetected/((double)stripCounter);
		      
		      std::cout<<"MB4 \t \t \t \t \t Number of strips "<<stripCounter<<" Strip Average Detected="<<meanStripDetected<<std::endl;
		      
		      LocalPoint meanstripDetectedLocalPoint = top_->localPosition((float)(meanStripDetected)-0.5);
		      
		      float meanrescms = PointExtrapolatedRPCFrame.x()-meanstripDetectedLocalPoint.x();          
		      
		      std::cout<<"MB4 \t \t \t \t \t PointExtrapolatedRPCFrame.x="<<PointExtrapolatedRPCFrame.x()<<" meanstripDetectedLocalPoint.x="<<meanstripDetectedLocalPoint.x()<<std::endl;
		      
		      if(fabs(meanrescms) < MinimalResidualRB4 ){
			
			std::cout<<"MB4 \t \t \t \t \t MeanRes="<<meanrescms<<"cm  MinimalResidual="<<MinimalResidual<<"cm"<<std::endl;
			
			std::cout <<"MB4 \t \t \t \t \t COINCIDENCE Predict "<<stripPredicted<<"  (int)Predicted="<<(int)(stripPredicted)<<"  MeanDetected="<<meanStripDetected<<std::endl;
			anycoincidence=true;
			std::cout <<"MB4 \t \t \t \t \t Increassing DT4 counter"<<std::endl;
			totalcounter[1]++;
			buff=counter[1];
			buff[rollId]++;
			counter[1]=buff;		
			
			sprintf(meIdRPC,"RPCDataOccupancyFromDT_%s",detUnitLabel);
			meMap[meIdRPC]->Fill(stripPredicted);
			
		      }
		    }else{
		      std::cout <<"MB4 \t \t \t \t \t THIS ROLL DOESN'T HAVE ANY DIGI"<<std::endl;
		    }
		    if(anycoincidence==false){
		      std::cout <<"MB4 \t \t \t \t \t THIS PREDICTION DOESN'T MATCH WITH THE DATA"<<std::endl;
		      std::cout <<"MB4 \t \t \t \t \t Increassing DT4 bad counter"<<std::endl;	
		      totalcounter[2]++;
		      buff=counter[2];
		      buff[rollId]++;
		      counter[2]=buff;		
		      ofrej<<"MB4 \t Wh "<<dtWheel
			   <<"\t St "<<dtStation
			   <<"\t Se "<<dtSector
			   <<"\t Roll "<<rollasociated->id()
			   <<"\t Event "
			   <<iEvent.id().event()
			   <<"\t Run "
			   <<iEvent.id().run()
			   <<std::endl;
		    }
		  }else{
		    std::cout<<"MB4 \t \t \t \t No the prediction is outside of this roll"<<std::endl;
		  }
		}else{
		  std::cout<<"MB4 \t \t \t No, Exrtrapolation too long!, canceled"<<std::endl;
		}
	      }//loop over all the rollsasociated
	    }else{
	      compatiblesegments=false;
	      std::cout<<"MB4 \t \t \t \t I found segments in MB4 and MB3 same wheel and sector but not compatibles Diferent Directions"<<std::endl;
	    }
	  }else{//if dtid3.station()==3&&dtid3.sector()==DTId.sector()&&dtid3.wheel()==DTId.wheel()&&segMB3->dim()==4
	    std::cout<<"MB4 \t \t \t No the same station or same wheel or segment dim in mb3 not 4D"<<std::endl;
	  }
       	}//lood over all the segments looking for one in MB3 
      }else{
	std::cout<<"MB4 \t \t Is NOT a 2D Segment"<<std::endl;
      }
    }else{
      std::cout<<"MB4 \t \t There is not just one segment or is not in station 4"<<std::endl;
    }//De aca para abajo esta en dtpart.inl
  }
}else{
  std::cout<<"MB4 \t This event doesn't have 4D Segment"<<std::endl;
}
