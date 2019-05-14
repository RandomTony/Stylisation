#include "args/fdogArgs.hpp"
#include <iostream>

FdogArgs::FdogArgs(): Args(5) {
    this->m_argsName[0] = "SigmaM";
    this->m_argsName[1] = "SigmaC";
    this->m_argsName[2] = "rho";
    this->m_argsName[3] = "threshold";
    this->m_argsName[4] = "Max iteration";

    this->m_argsDefaultVal[0] = float(3.0);
    this->m_argsDefaultVal[1] = float(1.0);
    this->m_argsDefaultVal[2] = float(0.98);
    this->m_argsDefaultVal[3] = float(0.5);
    this->m_argsDefaultVal[4] = float(2);
}

void FdogArgs::setArgs(){
    std::cout << "FDoG args" << '\n';
    Args::setArgs();
}
