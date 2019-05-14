#ifndef FBL_ARGS_HPP
#define FBL_ARGS_HPP

#include "args/args.hpp"

/*!
* \brief Parameter for FBL (Flow based bilateral filter)
*
* This class consist of five parameters for the FBL: sigmaE, sigmaG, sigmaRE,
* sigma RG and the maximum iteration number. The respectives default values are
* 2.0; 2.0, 50.0, 10.0, 4
*/
class FblArgs : public Args {
public:
    /*!
    * \brief Constructor of FBL parameters
    */
    FblArgs();
    /*!
    * \brief set fbl args
    * \sa Args::setArgs()
    */
    void setArgs();
};

#endif /* end of include guard: FBL_ARGS_HPP */
