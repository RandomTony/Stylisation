#include "args/args.hpp"
#include <iomanip>
#include <iostream>
#include <sstream>

Args::Args(int nbA): nbArgs(nbA)
{
  args = new float[nbArgs];
  argsDefaultVal = new float[nbArgs];
  argsName = new string[nbArgs];
}

void Args::setArgs(){
  std::cout << "Leave blank field if you want to use defaut value." << '\n';
  string mystr;
  for (int i = 0; i < nbArgs; i++) {
    mystr = "";
    cout << "Enter " << argsName[i] << " (defaut value " << this->argsDefaultVal[i] << "): ";
    getline(cin,mystr);
    if (mystr != "") {
      stringstream(mystr) >> this->args[i];
    }
    else{
      cout << "Using default value" << endl;
      this->args[i] = this->argsDefaultVal[i];
    }
  }
}

float Args::getArg(int parameterIndex) {
  if (parameterIndex >= this->nbArgs) {
    cout << "Incorrect number of parameter" << endl;
    return -1.0;
  }
  else {
    return this->args[parameterIndex];
  }
}

void Args::print(){
  for(int i = 0; i <nbArgs; i++){
    cout << "|" << argsName[i];
  }
  cout << "|" << endl;
  for(int i = 0; i <nbArgs; i++){
    cout << "|"<< setfill(' ') << setw(argsName[i].length()) << left << args[i];
  }
  cout << "|" << endl;
}

string Args::csvFormatted(){
  string s = "";
  for(int i = 0; i < nbArgs; i++){
    s += to_string(args[i]) + ";";
  }
  return s;
}
