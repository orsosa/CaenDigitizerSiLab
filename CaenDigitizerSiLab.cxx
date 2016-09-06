#include "CaenDigitizerSiLab.h"

ClassImp(CaenDigitizerSiLab)

int32_t CaenDigitizerSiLab::init()
{
  meanTemp= new float[MaxNCh];
  varTemp= new float[MaxNCh];
  storedMeanTemp= new float[MaxNCh];
  storedVarTemp= new float[MaxNCh];

  NCh=MaxNCh;
  kNSamplesTemp=30; // number of samples to determine mean an var. of channel temperature.
  kDtTemp=1;// samplinbg time for temperature measurements.
  switch (kPolarizationType)
  {
  case 0:
    kOffset=0x7FFF;
    break;
  case -1:
    kOffset=0x1000;
    break;
  case 1:
    kOffset=0xefff;
    break;
  }
  triggermode = CAEN_DGTZ_TRGMODE_ACQ_ONLY;
  Evt = NULL;
  acqmode = CAEN_DGTZ_SW_CONTROLLED;
  buffer = NULL;
  evtptr = NULL;
  ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_USB,0,0,0,&handle);
  if(ret != CAEN_DGTZ_Success) {
    printf("Can't open digitizer\n");
    return -1;
  }
  ret = CAEN_DGTZ_Reset(handle);// resetting Digitizer
  if (kDoCalibration)
  {
    calibrate();
  }
  std::cout<<"number of samnples per acq: "<<kSamples<<std::endl;
  ret = CAEN_DGTZ_SetRecordLength(handle,kSamples); // samples a grabar por acquisition windows CAMBIAR es decir tamaño de buffer
  ret = CAEN_DGTZ_SetChannelEnableMask(handle,kEnableMask);// no disponible en el DT5740
  TString branches;
  branches.Append(Form("Ch%d",0));
  for (int32_t k =1;k<NCh;k++)
  {
    branches.Append(":");    
    branches.Append(Form("Ch%d",k));
  }
  branches.Append(":time:event");
  ofile = new TFile("data_from_digitizer.root","recreate");
  data = new TNtuple("data","amp (V) and time (nsample)",branches.Data() );
    
  //  trigthresh = th2int(0.5);//threshold in volts.
  trigthresh=14000;
  std::cout<<"trihthreshold: "<<trigthresh<<std::endl;
  for (int32_t k=0;k<MaxNCh;k++)
  {
      ret = CAEN_DGTZ_SetChannelDCOffset(handle,k,kOffset);
      //ret = CAEN_DGTZ_SetChannelTriggerThreshold(handle,k,trigthresh);                /* Set selfTrigger threshold */
      ret = CAEN_DGTZ_SetChannelTriggerThreshold(handle,k,trigthresh);                  /* Set selfTrigger threshold */
      
      ret = CAEN_DGTZ_SetTriggerPolarity(handle, k, CAEN_DGTZ_TriggerOnFallingEdge);
      //ret = CAEN_DGTZ_SetTriggerPolarity(handle, k, CAEN_DGTZ_TriggerOnRisingEdge);
  }


  ret = CAEN_DGTZ_SetSWTriggerMode(handle, triggermode);//modo trigger
  ret = CAEN_DGTZ_SetPostTriggerSize(handle,2); 
  //% a grabar por cada trigger del record length
  ret = CAEN_DGTZ_SetAcquisitionMode(handle, acqmode);			// modo de adquisicion
  ret = CAEN_DGTZ_SetMaxNumEventsBLT(handle,1);//numero maximo de eventos por transferencia
  
  if(ret != CAEN_DGTZ_Success) {
    printf("Errors during Digitizer Configuration.\n");
  }
  

 
 
  //localiza buffer pointer en la memoria
  //the *buffer MUST be initialized to NULL
  //the size in byte of the buffer allocated
  return 0;
}

int32_t CaenDigitizerSiLab::getInfo()
{
  ret = CAEN_DGTZ_GetInfo(handle, &BoardInfo);
  printf("\nConnected to CAEN Digitizer Model %s\n", BoardInfo.ModelName);
  printf("\tROC FPGA Release is %s\n", BoardInfo.ROC_FirmwareRel);
  printf("\tAMC FPGA Release is %s\n", BoardInfo.AMC_FirmwareRel);
  return 0;
}

int32_t  CaenDigitizerSiLab::readEvents(int32_t events,bool automatic)
{
  int32_t count=0;
  ret = CAEN_DGTZ_MallocReadoutBuffer(handle,&buffer,(uint32_t *)&size);
  if (!automatic){
    ret = CAEN_DGTZ_SetChannelSelfTrigger(handle,CAEN_DGTZ_TRGMODE_ACQ_ONLY,0xFF); //Adjacent channels paired.
  }
  startSWAcq();

  while (count < events)
  {		
    if (automatic)
      ret = CAEN_DGTZ_SendSWtrigger(handle);
    ret = CAEN_DGTZ_ReadData(handle,CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT,buffer,(uint32_t *)&bsize);
    //entrega en numEvents el numero de eventos capturados
    uint32_t numEvents;
    ret = CAEN_DGTZ_GetNumEvents(handle,buffer,bsize,&numEvents);	
    printf("\revents: %d",count);
    fflush(stdout);
    Float_t *data_arr = new Float_t[NCh+2];
    for (int32_t i=0;i<numEvents;i++) 
    {
      ret = CAEN_DGTZ_GetEventInfo(handle,buffer,bsize,i,&eventInfo,(char **)&evtptr);
      ret = CAEN_DGTZ_DecodeEvent(handle,evtptr,(void **)&Evt);
      
      for (int32_t j=0;j<Evt->ChSize[0];j++)
      {
	for (int32_t k=0;k<NCh;k++)
	{
	  data_arr[k] = (int32_t)Evt->DataChannel[k][j];
	}
	data_arr[NCh]=j;
	data_arr[NCh+1]=count+i;
	data->Fill(data_arr);
      }
      ret = CAEN_DGTZ_FreeEvent(handle,(void **)&Evt);
    }
    count +=numEvents;
  }
  std::cout<<std::endl;
  return 0;
}

int32_t CaenDigitizerSiLab::getTempMeanVar()
  {
   
    for (int32_t i =0; i<MaxNCh; i++)
    {
      meanTemp[i]=0;
      varTemp[i]=0;
    }

    for (int32_t k=0;k<kNSamplesTemp;k++)
    {
      for (int32_t i =0; i<MaxNCh; i++)
      {
	if ((kEnableMask>>i)&0x1)
        {
	  readTemp(i);
	  meanTemp[i]+=chTemp;
	  varTemp[i]+=chTemp*chTemp;
	}
      }
      sleep(kDtTemp);
    }
    for (int32_t i =0; i<MaxNCh; i++)
    {
      meanTemp[i]/=kNSamplesTemp;
      varTemp[i]=(varTemp[i]/kNSamplesTemp-meanTemp[i]*meanTemp[i]);
    }

  }


int32_t CaenDigitizerSiLab::storeData()
{
  ofile->cd();
  data->Write("",TObject::kOverwrite);
  ofile->Close();
  return 0;
}

CaenDigitizerSiLab::~CaenDigitizerSiLab()
{
  delete [] meanTemp;
  delete [] varTemp;
  delete [] storedMeanTemp;
  delete [] storedVarTemp;
  //  CAEN_DGTZ_FreeEvent(handle,(void **)&Evt);
}
