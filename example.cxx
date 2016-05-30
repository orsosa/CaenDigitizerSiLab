#include "CaenDigitizerSiLab.h"

int main()
{
  CaenDigitizerSiLab *dig = new CaenDigitizerSiLab();
  dig->getInfo();
  dig->readTempAll();
  return 0;
}
