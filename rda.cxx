#include "CaenDigitizerSiLab.h"
#include "TBenchmark.h"
#include <signal.h>
CaenDigitizerSiLab *dig;
TBenchmark *bench;
void signalHandler(int){ dig->storeData(); delete dig;  bench->Show("example"); delete bench; exit(0);}
int main(int argc, char *argv[])
{
  signal(SIGINT , signalHandler);
  bench = new TBenchmark();
  bench->Start("example");
  dig = new CaenDigitizerSiLab();  
  dig->setPolarizationType(-1);
  //dig->setTrigmV(-40);

  dig->getInfo();
  dig->readTempAll();
  int bunch_size=1000;
  int NBunch =1;
  if (argc==2) bunch_size = atoi(argv[1]);
  if (argc==3)
  {
    bunch_size = atoi(argv[1]);
    NBunch = atoi(argv[2]);
  }
  
  for (int k=0;k<NBunch;k++)
  {
    std::cout<<"On bunch: "<<k<<std::endl<<std::endl;
    dig->readEvents(bunch_size,false,k*bunch_size);
    dig->storeData();
    dig->storeTempAll();
    //dig->calibrate();
  }
  
  
  bench->Show("example");
  return 0;
}
