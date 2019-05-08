#include "args/fdogArgs.hpp"
#include <iostream>

FdogArgs::FdogArgs(): Args(5) {
  this->argsName[0] = "SigmaM";
  this->argsName[1] = "SigmaC";
  this->argsName[2] = "rho";
  this->argsName[3] = "threshold";
  this->argsName[4] = "Max iteration";

  this->argsDefaultVal[0] = float(3.0);
  this->argsDefaultVal[1] = float(1.0);
  this->argsDefaultVal[2] = float(0.98);
  this->argsDefaultVal[3] = float(0.5);
  this->argsDefaultVal[4] = float(2);
}

void FdogArgs::setArgs(){
  std::cout << "FDoG args" << '\n';
  Args::setArgs();
}
