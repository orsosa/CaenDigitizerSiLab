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
#include <sys/time.h>


//#define MaxVpp 2.0
//#define NBit 14

class CaenDigitizerSiLab: public TObject
{
  //private:
public:
  int32_t MaxNCh;
  float MaxVpp;
  int32_t NBit;
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
  int32_t kModel;
  bool kTempSupported;
  bool kCalibrationSupported;
  CAEN_DGTZ_TriggerPolarity_t kTriggerpolaritymode;
  Float_t kSamplingTime;


public:
 CaenDigitizerSiLab() : kPolarizationType(0), kEnableMask(0xff), kSamples(100), kDoCalibration(kFALSE) {}
 CaenDigitizerSiLab( int8_t p, uint32_t em, int32_t s, bool docal): kPolarizationType(p), kEnableMask(em), kSamples(s), kDoCalibration(docal) {init();}
  ~CaenDigitizerSiLab();
  int32_t init();
  int32_t finish();
  int32_t getInfo();
  int32_t getPedestal(int32_t samples){return 0;}
  CAEN_DGTZ_ErrorCode  startSWAcq(){ret = CAEN_DGTZ_SWStartAcquisition(handle); return ret;}
  CAEN_DGTZ_ErrorCode  stopSWAcq(){ret = CAEN_DGTZ_SWStopAcquisition(handle); return ret;}
  int32_t readEvents(int32_t evenst=100,bool automatic=kTRUE, int32_t start_event=0, uint32_t triggerSource=0);
  int32_t readEvents(int32_t maxEvents,bool automatic,int32_t start_event,double tm, uint32_t triggerSource=0);
  int32_t storeData();
  int32_t waitTempStabilization(){return 0;}
  int32_t getTempMeanVar();
  uint32_t chTemp;
  int32_t setTrigADC(int32_t trigthresh);
  int32_t setTrigmV(float mV);
  int32_t setCoincidence(int32_t ch0=0);//wd 4 ns
  int32_t setMajorCoincidence(int32_t blkmask=0x0e, int32_t wd=1,int32_t level=0);//wd 8 ns
  Float_t  adc2mV(int32_t adc) {return  (adc/( (1<<NBit) - 1.0 ) - 1.0 + (float)kOffset/(float)0xffff )*MaxVpp*1000;}
  int32_t  mV2adc(float mV) {return ( mV/1000.0 + MaxVpp*(1 - kOffset/(float)0xffff ) )*( (1<<NBit) - 1.0 )/(float)MaxVpp;}
  int32_t newFile(const char* name);
  int32_t closeLastFile();


  //configura el model de digitizer a utilizar. Por defecto se utiliza el DT5730
  //Para configurar, se reciben los 4 numeros del digitizer. ej: 'DT5730' => 5730
  int32_t setModel(int32_t model = 5730)
  {
    std::string msg = "Model not supported. Setting default model: DT5730";
    kModel = model;
    switch (kModel) {
      case 5730:  MaxNCh = 8;
                  MaxVpp = 2;
                  NBit = 14;
                  msg = "Model DT5730";
                  kTempSupported = kTRUE;
                  kCalibrationSupported = kTRUE;
                  kSamplingTime = 0.000000002;
                  break;
      case 5724:  MaxNCh = 4;
                  MaxVpp = 2.25;
                  NBit = 14;
                  msg = "Model DT5724";
                  kTempSupported = kFALSE;
                  kCalibrationSupported = kFALSE;
                  kSamplingTime = 0.0000001;
                  break;
      default:    MaxNCh = 8;
                  MaxVpp = 2;
                  NBit = 14;
                  kTempSupported = kTRUE;
                  kCalibrationSupported = kTRUE;
                  kSamplingTime = 0.000000002;
    }
    std::cout<<msg<<"\nChannels: "<<MaxNCh<<"\nVpp = "<<MaxVpp<<"\nADC bits = "<<NBit<<"\r"<<std::endl;
    return 0;
  }

  void setTriggerPolarity(int32_t mode)
  {
      if (mode == 1)
        kTriggerpolaritymode = CAEN_DGTZ_TriggerOnRisingEdge;
      else if (mode == 0)
        kTriggerpolaritymode = CAEN_DGTZ_TriggerOnFallingEdge;
      else
        {
          std::cout<<"Wrong trigger polarity mode. Setting default: Risign edge";
          kTriggerpolaritymode = CAEN_DGTZ_TriggerOnRisingEdge;
        }
  }

  void setTime2Nsamples(Float_t tm, int32_t trigger_size=60)
  {
    kSamples = (tm/kSamplingTime);
    ret = CAEN_DGTZ_SetRecordLength(handle,kSamples);
    ret = CAEN_DGTZ_SetPostTriggerSize(handle,trigger_size);
    std::cout<<"New number of samples per aqc = "<<kSamples<<std::endl;
  }

  int32_t readTemp(int32_t ch)
  {
    if (kTempSupported)
      ret = CAEN_DGTZ_ReadTemperature(handle, ch, &chTemp);
    else {
      std::cout<<"Temperature measurement not supported"<<std::endl;
      return -1;
    }
    return 0;
  }

  int32_t readTempAll(bool print=true)
  {
    if (kTempSupported)
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
    }
    else {
      std::cout<<"Temperature measurement not supported"<<std::endl;
      return -1;
    }
    return 0;
  }


  int32_t storeTempAll(bool print=true)
  {
    if (kTempSupported)
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
    }
    else {
      std::cout<<"Temperature measurement not supported"<<std::endl;
      return -1;
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
            	kOffset=0x08c0; //offset adaptado para que el cero calze con lo esperado
            	break;
      case 1:
              kOffset=0xf7a3; //offset adaptado para que el cero calze con lo esperado
              //kOffset=0xffff;
      }
    uint32_t dat = 0;
    for (int k =0; k<MaxNCh;k++)
    {
      ret = CAEN_DGTZ_ReadRegister(handle,(0x1088+k*0x100),&dat);
      ret = CAEN_DGTZ_SetChannelDCOffset(handle,k,kOffset);
    }
    calibrate();
  }

  void setNSamples(int32_t nsamples=50, uint32_t trigger_size=60)
  {
    kSamples=nsamples;
    ret = CAEN_DGTZ_SetRecordLength(handle,kSamples);
    uint32_t configured_kSamples = 0;
    ret = CAEN_DGTZ_GetRecordLength(handle, &configured_kSamples);
    std::cout<<"New number of samples per aqc = "<<configured_kSamples<<std::endl;
    
    //El posttrigger size se calibra segun error de 75 muestras de desfase, propio del Digitizer
    int calibracion = 7500/configured_kSamples;
    ret = CAEN_DGTZ_SetPostTriggerSize(handle,(trigger_size-calibracion));

    uint32_t configured_trigger_size = 0;
    ret = CAEN_DGTZ_GetPostTriggerSize(handle, &configured_trigger_size);
    std::cout<<"New postTrigger Size= "<<configured_trigger_size+calibracion<< "\% | Equal to: "<< kSamples*(configured_trigger_size+calibracion)/100<<" samples."<<std::endl;
    
    ret = CAEN_DGTZ_FreeReadoutBuffer(&buffer);
    ret = CAEN_DGTZ_MallocReadoutBuffer(handle,&buffer,(uint32_t *)&size);
  
  }


  void calibrate()
  {
    if (kCalibrationSupported)
    {
      waitTempStabilization();// var <0.5
      std::cout<<"starting calibration\n";
      readTempAll();
      ret = CAEN_DGTZ_Calibrate(handle);
      std::cout<<"cal. ret: "<<ret<<std::endl;
      sleep(5);
    }
    else
    {
      std::cout<<"Calibration not supported"<<std::endl;
    }
  }


ClassDef(CaenDigitizerSiLab,1)
};
#endif
