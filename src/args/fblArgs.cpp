#include "args/fblArgs.hpp"
#include <iostream>

FblArgs::FblArgs() : Args(5) {
  // set var name
  this->argsName[0] = "SigmaE";
  this->argsName[1] = "SigmaG";
  this->argsName[2] = "SigmaRE";
  this->argsName[3] = "SigmaRG";
  this->argsName[4] = "Max iteration";

  this->argsDefaultVal[0] = float(2.0);
  this->argsDefaultVal[1] = float(2.0);
  this->argsDefaultVal[2] = float(50.0);
  this->argsDefaultVal[3] = float(10.0);
  this->argsDefaultVal[4] = float(4);
}

void FblArgs::setArgs(){
  std::cout << "FBL args" << '\n';
  Args::setArgs();
}
