#include "args/kMeanArgs.hpp"
#include <iostream>

KMeanArgs::KMeanArgs() : Args(1) {
    this->m_argsName[0] = "Luminance Class Number";
    this->m_argsDefaultVal[0] = 8.0;
}

void KMeanArgs::setArgs(){
    std::cout << "Luminance kmeans args" << '\n';
    Args::setArgs();
}
