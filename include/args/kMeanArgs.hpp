#ifndef KMEAN_ARGS_HPP
#define KMEAN_ARGS_HPP

#include "args/args.hpp"

/*!
* \brief Class to hanfle KMean parameters
*/
class KMeanArgs : public Args {
public:
    /*!
    * \brief Default constructor
    *
    * KMean has just one parameter: the number of Luminance classes. Default: 8
    */
    KMeanArgs();
    /*!
    * \brief Ask the user KMeans value
    */
    void setArgs();
};

#endif /* end of include guard: KMEAN_ARGS_HPP */
