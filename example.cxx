////////////////////////////////////////////////////////////////////////////////
////  Desarrollado por Orlando Soto.
////  Comentando, analizado y reparado por Jairo González.
////  Documentación dispoible en:
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

//Funcion para cerrar correctamente el script con ctrl+c
void signalHandler(int)
{
  dig->storeData();
  dig->stopSWAcq();
  delete dig;
  bench->Show("example");
  delete bench;
  exit(0);
}

int main()
{
  signal(SIGINT , signalHandler);

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
    dig->getInfo();


    dig->setPolarizationType(polarization);//rango de polarizacion
    dig->setNSamples(acqsamples, ptriggersize);//samples por evento y post trigger size
    dig->setTrigmV(vthreshold);//threshold en milivolts

    int bunch_size=nevents;  //numero de eventos
    int NBunch=bunches; //numero de grupos de eventos

    //Medición
    dig->newFile("data_from_digitizer.root");
    for (int k=0;k<NBunch;k++)
    {
      std::cout<<"On bunch: "<<k<<std::endl<<std::endl;
      dig->readEvents(bunch_size,false,k*bunch_size); //lectura con selftrigger
      dig->storeData();
      dig->storeTempAll();
      dig->calibrate();
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
