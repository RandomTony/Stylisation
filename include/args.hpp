#ifndef ARGS_HPP
#define ARGS_HPP

#include <iostream>
#include <sstream>
#include <string>

using namespace std;

class Args{
protected:
  int nbArgs;
  // cast floa because I'm lazy
  float * args;
  float * argsDefaultVal;
  string * argsName;

public:
  Args(int nbA): nbArgs(nbA)
  {
    args = new float[nbArgs];
    argsDefaultVal = new float[nbArgs];
    argsName = new string[nbArgs];
  }

  virtual void setArgs(){
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

  virtual float getArg(int parameterIndex) {
    if (parameterIndex >= this->nbArgs) {
      cout << "Incorrect number of parameter" << endl;
      return -1.0;
    }
    else {
      return this->args[parameterIndex];
    }
  }

  virtual void print(){
    for(int i = 0; i <nbArgs; i++){
      cout << "|" << argsName[i];
    }
    cout << "|" << endl;
    for(int i = 0; i <nbArgs; i++){
      cout << "|"<< setfill(' ') << setw(argsName[i].length()) << left << args[i];
    }
    cout << "|" << endl;
  }

  virtual string csvFormatted(){
    string s = "";
    for(int i = 0; i < nbArgs; i++){
      s += to_string(args[i]) + ";";
    }
    return s;
  }
};

class FblArgs : public Args {
public:
  FblArgs() : Args(5) {
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

  void setArgs(){
    std::cout << "FBL args" << '\n';
    Args::setArgs();
  }
};

class FdogArgs : public Args {
public:
  FdogArgs(): Args(5) {
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

  void setArgs(){
    std::cout << "FDoG args" << '\n';
    Args::setArgs();
  }
};

class KMeanArgs : public Args {
public:
  KMeanArgs() : Args(1) {
    this->argsName[0] = "Luminance Class Number";
    this->argsDefaultVal[0] = 8.0;
  }

  void setArgs(){
    std::cout << "Luminance kmeans args" << '\n';
    Args::setArgs();
  }
};

#endif
