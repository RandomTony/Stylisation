#include "args/kMeanArgs.hpp"
#include <iostream>

KMeanArgs::KMeanArgs() : Args(1) {
  this->argsName[0] = "Luminance Class Number";
  this->argsDefaultVal[0] = 8.0;
}

void KMeanArgs::setArgs(){
  std::cout << "Luminance kmeans args" << '\n';
  Args::setArgs();
}
