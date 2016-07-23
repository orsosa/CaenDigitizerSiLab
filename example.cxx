#include "CaenDigitizerSiLab.h"

int main()
{
  CaenDigitizerSiLab *dig = new CaenDigitizerSiLab();
  dig->getInfo();
  dig->readTempAll();
  dig->readEvents(500);
  dig->storeData();
  return 0;
}
