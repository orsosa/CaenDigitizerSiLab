#include <stdio.h>
#include "CAENDigitizer.h"
#include <unistd.h>
#include <sys/time.h>
#include <TFile.h>
#include <TH1I.h>
#include <TH1F.h>
#include <TNtuple.h>
#include "math.h"
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TCanvas.h>
#include "TApplication.h"

char filename[50];
int events;
int ped1=0;
int ped2=0;
int rms1=0;
int rms2=0;
TH1I *datos1;
TH1I *datos2;
TNtuple* datos;

///////////// ACA SETTINGS IMPORTANTES
int offset=0x7FFF;
int trigthresh=8250;
int samples=40;
///////////////////

uint32_t th2int(int pol,float value){
	uint32_t ret = 0;
	switch(pol){
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

int pedestal(int eventos)
{
	
	timeval ti,tf;
	CAEN_DGTZ_ErrorCode ret;
	int	handle;
	int i,x,j;
	int c = 0,conta=0;
	uint32_t habilitados=0xf; 
	CAEN_DGTZ_BoardInfo_t BoardInfo;
	CAEN_DGTZ_EventInfo_t eventInfo;
	CAEN_DGTZ_UINT16_EVENT_t *Evt = NULL;
	CAEN_DGTZ_TriggerMode_t triggermode = CAEN_DGTZ_TRGMODE_ACQ_ONLY;	//para uso con el proyecto
	CAEN_DGTZ_AcqMode_t acqmode = CAEN_DGTZ_SW_CONTROLLED;			//control por software
	char *buffer = NULL;
	char *evtptr = NULL;
	int size,bsize;
	int numEvents;
	i = sizeof(CAEN_DGTZ_TriggerMode_t);


	TH1I* pedestal0= new TH1I("pedestal0","pedestal ch0",32766,0,655320);
	TH1I* pedestal1= new TH1I("pedestal1","pedestal ch1",32766,0,655320);


	ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_USB,0,0,0,&handle);		 //configuracion para uso con USB
		if(ret != CAEN_DGTZ_Success) {
            printf("Can't open digitizer\n");
            return -1;
        }

		ret = CAEN_DGTZ_Reset(handle);// resetting Digitizer	
	ret = CAEN_DGTZ_SetRecordLength(handle,samples); // samples a grabar por acquisition windows CAMBIAR es decir tamaño de buffer
	ret = CAEN_DGTZ_SetChannelEnableMask(handle,habilitados); 		// no disponible en el DT5740
 	ret = CAEN_DGTZ_SetChannelDCOffset(handle,0,offset); //ch0 0 a -Vpp
 	ret = CAEN_DGTZ_SetChannelDCOffset(handle,1,offset); //ch0 0 a -Vpp
   ret = CAEN_DGTZ_SetChannelTriggerThreshold(handle,0,trigthresh);                  /* Set selfTrigger threshold */
   ret = CAEN_DGTZ_SetChannelTriggerThreshold(handle,1,trigthresh);                  /* Set selfTrigger threshold */
	ret = CAEN_DGTZ_SetSWTriggerMode(handle, triggermode);			//modo trigger 
	ret = CAEN_DGTZ_SetPostTriggerSize(handle,90); 
	//% a grabar por cada trigger del record length
	ret = CAEN_DGTZ_SetAcquisitionMode(handle, acqmode);			// modo de adquisicion

	ret = CAEN_DGTZ_SetMaxNumEventsBLT(handle,1);			//numero maximo de eventos por transferencia

   if(ret != CAEN_DGTZ_Success) {
            printf("Errors during Digitizer Configuration.\n");
        }

  	ret = CAEN_DGTZ_MallocReadoutBuffer(handle,&buffer,(uint32_t *)&size);	//localiza buffer pointer en la memoria
										//the *buffer MUST be initialized to NULL
										//the size in byte of the buffer allocated

	x = 0;

	int state=0;
	int aux0=0;
	int aux1=0;
	printf("Getting pedestal, by %d events\n",eventos);
	gettimeofday(&ti,NULL);

	ret = CAEN_DGTZ_SWStartAcquisition(handle);	


while( conta <= eventos) 
	{
		ret = CAEN_DGTZ_SendSWtrigger(handle);
  
		ret = CAEN_DGTZ_ReadData(handle,CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT,buffer,(uint32_t *)&bsize);
		//entrega en numEvents el numero de eventos capturados
		ret = CAEN_DGTZ_GetNumEvents(handle,buffer,bsize,(uint32_t *)&numEvents);	

		conta +=numEvents;

		printf("\revents: %d",conta);
		fflush(stdout);
		  
		for (i=0;i<numEvents;i++) {
		
			ret = CAEN_DGTZ_GetEventInfo(handle,buffer,bsize,i,&eventInfo,(char **)&evtptr);
	
			ret = CAEN_DGTZ_DecodeEvent(handle,evtptr,(void **)&Evt);	
			
			for (j=0;j<Evt->ChSize[0];j++)
			{
	
						aux0+= *(Evt->DataChannel[0]+j);
						aux1+= *(Evt->DataChannel[1]+j);

			}
				pedestal0->Fill(aux0);
				pedestal1->Fill(aux1);
				ret = CAEN_DGTZ_FreeEvent(handle,(void **)&Evt);
				aux0=0;
				aux1=0;
		
		}
	
	}
	gettimeofday(&tf,NULL);
	putchar('\n');

	ret = CAEN_DGTZ_FreeReadoutBuffer(&buffer);	//Frees memory allocated by the CAEN_DGTZ_MallocReadoutBuffer
	ret = CAEN_DGTZ_CloseDigitizer(handle);
	double time_elapsed = tf.tv_sec-ti.tv_sec + tf.tv_usec/1e6-ti.tv_usec/1e6;
	printf("time elapsed: %5.2f\n<rate>: %3.2f\n",time_elapsed,conta/time_elapsed);
	printf("Retrieved %d Event\n",conta);
	ped1=floor(pedestal0->GetMean(1));
	ped2=floor(pedestal1->GetMean(1));
	rms1=ceil(pedestal0->GetRMS(1));
	rms2=ceil(pedestal1->GetRMS(1));


	return 0;
}

int execute(int events, int ped1, int ped2, int rms1, int rms2)
{

	datos1= new TH1I("datos1","histograma1",32766,0,655320); // pueden arreglar el bining si quieren como quieran jaja
   datos2= new TH1I("datos2","histograma2",32766,0,655320);

	datos= new TNtuple("datos", "datos digitizer", "evento:ch0:ch1"); // LOS DATOS BRUTOS DE LAS AMPLITUDES DE LOS PULSOS

	timeval ti,tf;
	CAEN_DGTZ_ErrorCode ret;
	int	handle;
	int i,x,j,k;
	int c = 0,conta=0;
	uint32_t habilitados=0xf; 
	CAEN_DGTZ_BoardInfo_t BoardInfo;
	CAEN_DGTZ_EventInfo_t eventInfo;
	CAEN_DGTZ_UINT16_EVENT_t *Evt = NULL;
	CAEN_DGTZ_TriggerMode_t triggermode = CAEN_DGTZ_TRGMODE_ACQ_ONLY;	//para uso con el proyecto
	CAEN_DGTZ_AcqMode_t acqmode = CAEN_DGTZ_SW_CONTROLLED;			//control por software
	char *buffer = NULL;
	char *evtptr = NULL;
	int size,bsize;
	int numEvents;
	i = sizeof(CAEN_DGTZ_TriggerMode_t);

	int trigthreshCh0 = trigthresh;
	int trigthreshCh1 = trigthresh;
	
  printf("Threshold settings ch0: %d, ch1: %d \n",trigthreshCh0,trigthreshCh1);	

	ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_USB,0,0,0,&handle);		 //configuracion para uso con USB
	if(ret != CAEN_DGTZ_Success) {
            printf("Can't open digitizer\n");
            return -1;
        }

	ret = CAEN_DGTZ_Reset(handle);// resetting Digitizer	

	ret = CAEN_DGTZ_SetRecordLength(handle,samples);		// samples a grabar por acquisition windows CAMBIAR es decir tamaño de buffer
	ret = CAEN_DGTZ_SetChannelEnableMask(handle,habilitados); 		// no disponible en el DT5740

	ret = CAEN_DGTZ_SetChannelSelfTrigger(handle,CAEN_DGTZ_TRGMODE_ACQ_ONLY,0x1); //Ch0
//	ret = CAEN_DGTZ_SetChannelSelfTrigger(handle,CAEN_DGTZ_TRGMODE_ACQ_ONLY,0x2); //Ch1

	ret = CAEN_DGTZ_SetPostTriggerSize(handle, 90); 	//% a grabar por cada trigger, despues del trigger, del total del record length
	ret = CAEN_DGTZ_SetAcquisitionMode(handle, acqmode);			// modo de adquisicion
 	ret = CAEN_DGTZ_SetChannelDCOffset(handle,0,offset); //ch0 
 	ret = CAEN_DGTZ_SetChannelDCOffset(handle,1,offset); //ch1 
	ret = CAEN_DGTZ_SetChannelTriggerThreshold(handle,0,trigthreshCh0);		// 14 bits, distribuidos según el rango. [-1.125 , 1.125], [0 , 2.25]
	ret = CAEN_DGTZ_SetChannelTriggerThreshold(handle,1,trigthreshCh1);		
	ret = CAEN_DGTZ_SetChannelTriggerThreshold(handle,2,trigthresh);	
	ret = CAEN_DGTZ_SetChannelTriggerThreshold(handle,3,trigthresh);	

	ret = CAEN_DGTZ_SetTriggerPolarity(handle, 0, CAEN_DGTZ_TriggerOnFallingEdge);
	ret = CAEN_DGTZ_SetTriggerPolarity(handle, 1, CAEN_DGTZ_TriggerOnFallingEdge);
	ret = CAEN_DGTZ_SetTriggerPolarity(handle, 2, CAEN_DGTZ_TriggerOnFallingEdge);
	ret = CAEN_DGTZ_SetTriggerPolarity(handle, 3, CAEN_DGTZ_TriggerOnFallingEdge);
// Set trigger on channel 0 to be ACQ_ONLY

	ret = CAEN_DGTZ_SetMaxNumEventsBLT(handle,1);			//numero maximo de eventos por transferencia

  	ret = CAEN_DGTZ_MallocReadoutBuffer(handle,&buffer,(uint32_t *)&size);	//localiza buffer pointer en la memoria
										//the *buffer MUST be initialized to NULL
										//the size in byte of the buffer allocated
	printf("tamaño buffer : %d\n",size);
	ret = CAEN_DGTZ_SWStartAcquisition(handle);	
	x = 0;

	int line=0;
	int state=0;
	int n1p, aux0, aux1;
	n1p=0;
	aux0=0;
	aux1=0;
	gettimeofday(&ti,NULL);


while(1) {
		ret = CAEN_DGTZ_ReadData(handle,CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT,buffer,(uint32_t *)&bsize);
		//entrega en numEvents el numero de eventos capturados
		ret = CAEN_DGTZ_GetNumEvents(handle,buffer,bsize,(uint32_t *)&numEvents);	

		conta +=numEvents;
		printf("\reventos: %d mediciones:%d",conta,n1p);
		fflush(stdout);


		for (i=0;i<numEvents;i++) {
		
			ret = CAEN_DGTZ_GetEventInfo(handle,buffer,bsize,i,&eventInfo,(char **)&evtptr);
			ret = CAEN_DGTZ_DecodeEvent(handle,evtptr,(void **)&Evt);	
			int tam=Evt->ChSize[0]; //Aqui se obtiene el tamaño del EVENTO
	
			for (j=0;j<tam;j++)
			{
// ACA INTEGRA LOS PULSOS MEDIANTE LA SUMA SIMPLE DE LAS AMPLITUDES EN CADA CANAL			
						aux0+= *(Evt->DataChannel[0]+j);
						aux1+= *(Evt->DataChannel[1]+j);
// ACA LLENO UNA TUPLA PARA TENER LOS DATOS BRUTOS
						datos->Fill(conta,*(Evt->DataChannel[0]+j),*(Evt->DataChannel[1]+j));
			}
//			if ((aux0<integ1)&&(aux1>integ2))
//			{ 
		
				datos1->Fill(aux0);
				datos2->Fill(aux1);
				n1p++;
		//	}	
			ret = CAEN_DGTZ_FreeEvent(handle,(void **)&Evt);
			aux0=0;
			aux1=0;

		}

		if(n1p >= events) break;
	}
	gettimeofday(&tf,NULL);
	putchar('\n');


	ret = CAEN_DGTZ_FreeReadoutBuffer(&buffer);	//Frees memory allocated by the CAEN_DGTZ_MallocReadoutBuffer
	ret = CAEN_DGTZ_CloseDigitizer(handle);
	double time_elapsed = tf.tv_sec-ti.tv_sec + tf.tv_usec/1e6-ti.tv_usec/1e6;
	printf("time elapsed: %5.2f\n<rate>: %3.2f\n",time_elapsed,conta/time_elapsed);
	printf("Eventos totales: %d \n",conta);
	printf("Mediciones totales: %d rate de mediciones: %3.2f\n",n1p,n1p/time_elapsed);
	return 0;
}

int main(int argc, char* argv[]){


if (argc>3){
		printf("you just can insert only two parameters \n\
			densimetro [<num_events>] [<filename>]\n"); 
		return 0;
	}
	else if(argc>2){
		sprintf(filename,"%s.root",argv[2]);	
		events=atoi(argv[1]);
		}
	else if(argc>1){
		sprintf(filename,"datos.root");
		events=atoi(argv[1]);

	}
	else {
		sprintf(filename,"datos.root");
		events=1000;
	}

	printf("eventos a medir: %d \n", events);

	TFile *file = new TFile(filename,"RECREATE");


	
pedestal(10000);

printf("Calculated pedestal ch0: %d ch1: %d \n", ped1,ped2);

execute(events,ped1,ped2,rms1,rms2);

	file->Write();
	file->Close();
	delete file;

return 0;
}






