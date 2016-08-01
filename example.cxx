#include "CaenDigitizerSiLab.h"

int main()
{
  CaenDigitizerSiLab *dig = new CaenDigitizerSiLab();

  dig->setPolarizationType(-1);
  //  dig->calibrate();
  dig->getInfo();
  dig->readTempAll();
  dig->readEvents(300000);
  dig->storeData();

  return 0;
}
