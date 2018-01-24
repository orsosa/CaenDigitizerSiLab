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
#include "TBenchmark.h"
#include <signal.h>

CaenDigitizerSiLab *dig;
TBenchmark *bench;

//Funcion para cerrar correctamente el script con ctrl+c
void signalHandler(int)
{
  dig->storeData();
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

  dig = new CaenDigitizerSiLab();
  dig->setModel(5730);
  dig->init();
  dig->getInfo();


  dig->setPolarizationType(0);//rango de -1V hasta 1V
  dig->setNSamples(100);//samples por evento
  dig->setTrigmV(50);//threshold en milivolts

  int bunch_size=10; //numero de eventos
  int NBunch=1; //numero de tuplas guardadas¿?

  //Medición
  for (int k=0;k<NBunch;k++)
  {
    std::cout<<"On bunch: "<<k<<std::endl<<std::endl;
    dig->readEvents(bunch_size,false,k*bunch_size); //lectura con selftrigger
    dig->storeData();
    dig->storeTempAll();
    dig->calibrate();
  }

  dig->finish();

  bench->Show("example");

  delete dig;
  delete bench;

  return 0;
}
