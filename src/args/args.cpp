#include "args/args.hpp"
#include <iomanip>
#include <iostream>
#include <sstream>

Args::Args(int nbA): m_nbArgs(nbA)
{
    m_args = new float[m_nbArgs];
    m_argsDefaultVal = new float[m_nbArgs];
    m_argsName = new string[m_nbArgs];
}

void Args::setArgs(){
    std::cout << "Leave blank field if you want to use defaut value." << '\n';
    string mystr;
    for (int i = 0; i < m_nbArgs; i++) {
        mystr = "";
        cout << "Enter " << m_argsName[i] << " (defaut value " << this->m_argsDefaultVal[i] << "): ";
        getline(cin,mystr);
        if (mystr != "") {
            stringstream(mystr) >> this->m_args[i];
        } else {
            cout << "Using default value" << endl;
            this->m_args[i] = this->m_argsDefaultVal[i];
        }
    }
}

float Args::getArg(int parameterIndex) {
    if (parameterIndex >= this->m_nbArgs) {
        cout << "Incorrect number of parameter" << endl;
        return -1.0;
    } else {
        return this->m_args[parameterIndex];
    }
}

void Args::print(){
    for(int i = 0; i <m_nbArgs; i++)
    cout << "|" << m_argsName[i];
    cout << "|" << endl;
    for(int i = 0; i <m_nbArgs; i++){
        cout << "|"<< setfill(' ') << setw(m_argsName[i].length()) << left << m_args[i];
    }
    cout << "|" << endl;
}

string Args::csvFormatted(){
    string s = "";
    for(int i = 0; i < m_nbArgs; i++)
    s += to_string(m_args[i]) + ";";
    return s;
}
