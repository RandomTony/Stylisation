#ifndef ARGS_HPP
#define ARGS_HPP

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
  Args(int nbA);
  virtual void setArgs();
  virtual float getArg(int parameterIndex);
  virtual void print();
  virtual string csvFormatted();
};

#endif
