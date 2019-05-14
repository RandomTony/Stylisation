#ifndef FDOG_ARGS_HPP
#define FDOG_ARGS_HPP

#include "args/args.hpp"

/*!
* \brief Class to handle flow based difference of gaussian (fDoG)
*/
class FdogArgs : public Args {
public:
    /*!
    * \brief Default constructor
    *
    * The fdog parameters consist of the five following parameter: sigmaM,
    * sigmaC, rho, a threshold, a max number of iteration.
    * The respectives default values are: 3,1,0.98,0.5,2
    */
    FdogArgs();
    /*!
    * \brief Ask the user about fdog arguments
    */
    void setArgs();
};

#endif /* end of include guard: FDOG_ARGS_HPP */
