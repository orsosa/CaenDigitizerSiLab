#include "CaenDigitizerSiLab.h"

int main()
{
  CaenDigitizerSiLab *dig = new CaenDigitizerSiLab();

  dig->setPolarizationType(-1);
  //  dig->calibrate();
  dig->getInfo();
  dig->readTempAll();
  dig->readEvents(1000000);
  dig->storeData();

  return 0;
}
