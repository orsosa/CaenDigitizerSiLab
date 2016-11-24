#ifndef _CAENDIGITIZERSILAB_H_
#define _CAENDIGITIZERSILAB_H_
#include <stdio.h>
#include "CAENDigitizer.h"
#include <unistd.h>
#include <sys/time.h>
#include "math.h"
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include "TObject.h"
#include "TTree.h"
#include "TNtuple.h"
#include "TH1F.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TNtuple.h"

#define MaxNCh 8
#define MaxVpp 2.0
#define NBit 14

class CaenDigitizerSiLab: public TObject
{
  //private:
public:
  CAEN_DGTZ_ErrorCode ret;
  int32_t NCh;
  int32_t handle;
  int32_t trigthresh;
  uint32_t size;
  uint32_t bsize;
  CAEN_DGTZ_BoardInfo_t BoardInfo;
  CAEN_DGTZ_EventInfo_t eventInfo;
  CAEN_DGTZ_UINT16_EVENT_t *Evt;
  int8_t kPolarizationType;
  uint32_t kEnableMask;
  int32_t kOffset;
  CAEN_DGTZ_TriggerMode_t triggermode;
  CAEN_DGTZ_AcqMode_t acqmode;
  int32_t handler;
  char *buffer;
  char *evtptr; 
  int32_t kSamples;
  bool kDoCalibration;
  TFile *ofile;
  std::ofstream tempFile;
  TNtuple *data;
  float *meanTemp;
  float *varTemp;
  float *storedMeanTemp;
  float *storedVarTemp;
  int32_t kNSamplesTemp;
  float kDtTemp;
  
  
public:
 CaenDigitizerSiLab() : kPolarizationType(0), kEnableMask(0xff), kSamples(100), kDoCalibration(kFALSE) {init();}
 CaenDigitizerSiLab(int8_t p, uint32_t em, int32_t s, bool docal): kPolarizationType(p), kEnableMask(em), kSamples(s), kDoCalibration(docal) {init();}
  ~CaenDigitizerSiLab();
  int32_t init();
  int32_t getInfo();
  int32_t getPedestal(int32_t samples){return 0;}
  CAEN_DGTZ_ErrorCode  startSWAcq(){ret = CAEN_DGTZ_SWStartAcquisition(handle); return ret;}
  int32_t readEvents(int32_t evenst=100,bool automatic=kTRUE, int32_t start_event=0);
  int32_t storeData();
  int32_t waitTempStabilization(){return 0;}
  int32_t getTempMeanVar();
  uint32_t chTemp;
  int32_t setCoincidence(int32_t ch0=0, int32_t ch1=1, int32_t wd=2);//wd 16 ns
  int32_t readTemp(int32_t ch)
  {
    ret = CAEN_DGTZ_ReadTemperature(handle, ch, &chTemp);
    return 0;
  }
  
  int32_t readTempAll(bool print=true)
  {
    std::cout<<"ch\ttemp (°C)\n";
    for (int32_t i =0; i<MaxNCh; i++)
    {
      if ((kEnableMask>>i)&0x1)
      {
	readTemp(i);
	if (print) std::cout<<i<<"\t\t"<<chTemp<<std::endl;
      }
    }
    return 0;
  }
  int32_t storeTempAll(bool print=true)
  {
    tempFile<<"ch\ttemp (°C)\tdate\n";
    for (int32_t i =0; i<MaxNCh; i++)
    {
      if ((kEnableMask>>i)&0x1)
      {
	readTemp(i);
	if (print) tempFile<<i<<"\t"<<chTemp<<"\t"<<time(0)<<std::endl;
      }
    }
    return 0;
  }
  uint32_t th2int(float value)
  {
	uint32_t ret = 0;
	switch(kPolarizationType){
	case -1:	ret = (uint32_t) ((MaxVpp+value)*(0x1<<NBit)/MaxVpp);
			break;
	case 0:		ret = (uint32_t) ((MaxVpp/2+value)*(0x1<<NBit)/MaxVpp);
			break;
	case 1:		ret = (uint32_t) (value*(0x1<<NBit)/MaxVpp);
			break;
	default:	ret=0;
	}
	return ret;
  }
  void setPolarizationType(int pol=0)
  {
    kPolarizationType = pol;
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
      }
    for (int k =0; k<MaxNCh;k++)
      ret = CAEN_DGTZ_SetChannelDCOffset(handle,k,kOffset);
    calibrate();
  }
  void setNSamples(int32_t nsamples=100)
  {
    kSamples=nsamples;
  }

  void calibrate()
  {
    waitTempStabilization();// var <0.5
    std::cout<<"starting calibration\n";
    readTempAll();
    ret = CAEN_DGTZ_Calibrate(handle);
    std::cout<<"cal. ret: "<<ret<<std::endl;
    sleep(5);
  }
  

ClassDef(CaenDigitizerSiLab,1)
};
#endif
