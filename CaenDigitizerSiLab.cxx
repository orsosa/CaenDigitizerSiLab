#include "CaenDigitizerSiLab.h"

ClassImp(CaenDigitizerSiLab)

int32_t CaenDigitizerSiLab::init()
{
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
  waitTempStabilization();
  // ret = CAEN_DGTZ_Calibrate(handle);
  //sleep(10);
  ret = CAEN_DGTZ_SetRecordLength(handle,kSamples); // samples a grabar por acquisition windows CAMBIAR es decir tamaño de buffer
  ret = CAEN_DGTZ_SetChannelEnableMask(handle,kEnableMask);// no disponible en el DT5740
  // ret = CAEN_DGTZ_SetChannelDCOffset(handle,0,offset); //ch0 0 a -Vpp
  // ret = CAEN_DGTZ_SetChannelDCOffset(handle,1,offset); //ch0 0 a -Vpp
  ret = CAEN_DGTZ_SetChannelTriggerThreshold(handle,0,trigthresh);                  /* Set selfTrigger threshold */
  ret = CAEN_DGTZ_SetChannelTriggerThreshold(handle,1,trigthresh);                  /* Set selfTrigger threshold */
  ret = CAEN_DGTZ_SetSWTriggerMode(handle, triggermode);//modo trigger
  ret = CAEN_DGTZ_SetPostTriggerSize(handle,90); 
  //% a grabar por cada trigger del record length
  ret = CAEN_DGTZ_SetAcquisitionMode(handle, acqmode);			// modo de adquisicion
  ret = CAEN_DGTZ_SetMaxNumEventsBLT(handle,1);//numero maximo de eventos por transferencia
  
  if(ret != CAEN_DGTZ_Success) {
    printf("Errors during Digitizer Configuration.\n");
  }
  
  ret = CAEN_DGTZ_MallocReadoutBuffer(handle,&buffer,(uint32_t *)&size);
 
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

