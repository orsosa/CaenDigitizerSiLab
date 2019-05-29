////////////////////////////////////////////////////////////////////////////////
////  Desarrollado por Orlando Soto.
////  Comentado, analizado y reparado por Jairo González.
////  Documentación disponible en:
////  http://silab.fis.utfsm.cl/wiki/Manuales_Equipos_Lab
////
////  SiLab 2018
////
////////////////////////////////////////////////////////////////////////////////


#include "CaenDigitizerSiLab.h"
#include "configuration.h"
#include "TBenchmark.h"
#include <signal.h>

CaenDigitizerSiLab *dig;
TBenchmark *bench;

//Variables para control de flujo del programa (signal handler)
int quit=0;
int stage = 0;

//Funcion para cerrar correctamente el script con ctrl+c
void signalHandler(int sig)
{
  quit = 1;
  if (stage == 0){
    dig->storeData();
    dig->stopSWAcq();
    delete dig;
    bench->Show("example");
    delete bench;
    exit(sig);
  }
}

int main()
{
  //signal(SIGINT , signalHandler);

  struct sigaction act;
  act.sa_handler = signalHandler;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  
  printf("Exit: Ctrl+c\n");
  printf("Force Exit: Ctrl+\\\n");
  //benchmark mide el desempeño del programa, no es vital
  bench = new TBenchmark();
  bench->Start("example");

  //Se lee el archivo config.conf para configurar el digitizer
  if(readconfig("config.conf"))
  	{
  		printf("Error al leer archivo de configuracion, tomando valores por defecto...");
  	}

  dig = new CaenDigitizerSiLab();

  dig->setModel(model);
  dig->setTriggerPolarity(triggerpolaritymode);
  if (dig->init()>=0) {
    printf("test:  handle=%d\n",dig->handle);
    printf("start: dig->getInfo()\n");
    dig->getInfo();
    printf("final: dig->getInfo()\n");

    dig->setPolarizationType(polarization);//rango de polarizacion
    printf("polarization: %d\n",polarization);
    
    printf("start: setNSamples(acqsamples, ptriggersize)\n");
    dig->setNSamples(acqsamples, ptriggersize);//samples por evento y post trigger size
    printf("start: dig->setTrigmV(vthreshold)\n");
    //dig->setTrigmV(vthreshold);//threshold en milivolts

    int bunch_size=nevents;  //numero de eventos
    int NBunch=bunches; //numero de grupos de eventos
    
    //obtener algunos parametros
    //int* myLvl;CAEN_DGTZ_GetIOLevel (sig, myLvl);printf("myLvl=%d\n",*myLvl);

    //Medición
    printf("\ndig->newFile(\"data_from_digitizer.root\")\n");
    dig->newFile("data_from_digitizer.root");
    sigaction(SIGINT, &act, 0); 
    for (int k=0;k<NBunch;k++)
    {
      std::cout<<"On bunch: "<<k<<std::endl<<std::endl;
      
      printf("start: triggerSource=%d\n",triggerSource);
      stage = 1;
      dig->readEvents(bunch_size,false,k*bunch_size,timeout,triggerSource); //lectura con selftrigger y timeout
      dig->storeData();
      dig->storeTempAll();
      dig->calibrate();
      
      printf("test:  handle=%d\n",dig->handle);
    }
    dig->closeLastFile();

    dig->finish();

    bench->Show("example");

    delete dig;
    delete bench;
  }
  else
  {
    bench->Show("example");
    delete dig;
    delete bench;
  }
  

  return 0;
}
