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
#define MaxNCh 8

class CaenDigitizerSiLab
{
  //private:
public:
  CAEN_DGTZ_ErrorCode ret;
  int32_t handle;
  int32_t trigthresh;
  int32_t size;
  CAEN_DGTZ_BoardInfo_t BoardInfo;
  CAEN_DGTZ_EventInfo_t eventInfo;
  CAEN_DGTZ_UINT16_EVENT_t *Evt;
  int8_t kPolarizationType;
  uint32_t kEnableMask;
  CAEN_DGTZ_TriggerMode_t triggermode;
  CAEN_DGTZ_AcqMode_t acqmode;
  int32_t handler;
  char *buffer;
  char *evtptr; 
  int32_t kSamples;


public:
 CaenDigitizerSiLab() : kPolarizationType(0), kEnableMask(0xff), kSamples(40) {init();}
  CaenDigitizerSiLab(int8_t p, uint32_t em, int32_t s): kPolarizationType(p), kEnableMask(em), kSamples(s){init();}
  int32_t init();
  int32_t getInfo();
  int32_t getPedestal(int32_t samples){return 0;}
  CAEN_DGTZ_ErrorCode  startSWAcq(){ret = CAEN_DGTZ_SWStartAcquisition(handle); return ret;}
  int32_t readEvent(){return 0;}
  int32_t waitTempStabilization(){return 0;}
  uint32_t chTemp;
  int32_t readTemp(int32_t ch)
  {
    ret = CAEN_DGTZ_ReadTemperature(handle, ch, &chTemp);
    return 0;
  }
  
  int32_t readTempAll()
  {
    std::cout<<"ch\ttemp (°C)\n";
    for (int32_t i =0; i<MaxNCh; i++)
    {
      if ((kEnableMask>>i)&0x1)
      {
	readTemp(i);
	std::cout<<i<<"\t\t"<<chTemp<<std::endl;
      }
    }
    return 0;
  }
  uint32_t th2int(float value){
	uint32_t ret = 0;
	switch(kPolarizationType){
	case -1:	ret = (uint32_t) ((2.25+value)*0x4000/2.25);
			break;
	case 0:		ret = (uint32_t) ((1.25+value)*0x4000/2.25);
			break;
	case 1:		ret = (uint32_t) (value*0x4000/2.25);
			break;
	default:	ret=0;
	}
	return ret;
  }


};
#endif
