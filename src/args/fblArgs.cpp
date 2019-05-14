#include "args/fblArgs.hpp"
#include <iostream>

FblArgs::FblArgs() : Args(5) {
    // set var name
    this->m_argsName[0] = "SigmaE";
    this->m_argsName[1] = "SigmaG";
    this->m_argsName[2] = "SigmaRE";
    this->m_argsName[3] = "SigmaRG";
    this->m_argsName[4] = "Max iteration";

    this->m_argsDefaultVal[0] = float(2.0);
    this->m_argsDefaultVal[1] = float(2.0);
    this->m_argsDefaultVal[2] = float(50.0);
    this->m_argsDefaultVal[3] = float(10.0);
    this->m_argsDefaultVal[4] = float(4);
}

void FblArgs::setArgs(){
    std::cout << "FBL args" << '\n';
    Args::setArgs();
}
