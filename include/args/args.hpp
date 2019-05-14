#ifndef ARGS_HPP
#define ARGS_HPP

#include <string>

using namespace std;

/*!
* \brief Class to handle decimal parameters
*
* This class has three tables, one for argument values, one which handles
* argument default value, and third handles argument names. All three tables
* must have the same size
*/
class Args{
protected:
    int m_nbArgs; /*!< The number of parameter handled by the class */
    float * m_args; /*!< A table of parameter values */
    float * m_argsDefaultVal; /*!< A table of parameter default values */
    string * m_argsName; /*!< A table of parameter names */

public:
    /*!
    * \brief Constructor of Args object
    *
    * The default constructor of argument class. It allocates memory for
    * the three tables depending of the value given in the constructor
    * \param nbA an positive integer indicate the number of argument in this class
    */
    Args(int nbA);
    /*!
    * \brief Ask user argument values
    *
    * Ask the user the different argument values an print the default one.
    * If the user gives a blank value, the default value for the parameter will
    * be used.
    * \sa getline, stringstream
    */
    virtual void setArgs();
    /*!
    * \brief Return the argument value at specific index
    * \param parameterIndex the index for the instersting value
    * in the m_args table
    */
    virtual float getArg(int parameterIndex);
    /*!
    * \brief Display argument names and value with a table
    */
    virtual void print();
    /*!
    * \brief Give a string of csv formatted argument values
    * The csv formatted value are separated
    */
    virtual string csvFormatted();
};

#endif
