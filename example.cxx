#include "CaenDigitizerSiLab.h"
#include "TBenchmark.h"
#include <signal.h>
CaenDigitizerSiLab *dig;
TBenchmark *bench;
void signalHandler(int){ dig->storeData(); delete dig;  bench->Show("example"); delete bench; exit(0);}
int main()
{
  signal(SIGINT , signalHandler);
  bench = new TBenchmark();
  bench->Start("example");
  dig = new CaenDigitizerSiLab();
  
  dig->setPolarizationType(-1);
  //  dig->calibrate();
  dig->getInfo();
  dig->readTempAll();
  //dig->setCoincidence(0,1);
  int bunch_size=100;
  int NBunch =1;
  
  for (int k=0;k<NBunch;k++)
  {
    std::cout<<"On bunch: "<<k<<std::endl<<std::endl;
    dig->readEvents(bunch_size,false,k*bunch_size);
    dig->storeData();
    dig->storeTempAll();
    dig->calibrate();
  }
  
  
  bench->Show("example");
  return 0;
}
