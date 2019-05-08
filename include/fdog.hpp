#ifndef FDOG_HPP
#define FDOG_HPP

#include <opencv2/opencv.hpp>

using namespace cv;


/** brief superImposing - superImpose a picture: add edges to a given picture and smooth it with a Gaussian kernel
param grayImg, the grayScale image in float between 0 and 1
param fDoG, the edge map. edges are black
param sigma, the value to smooth the two picture
return the superimposed picture
*/
Mat superImposing(const Mat & grayImg, const Mat & fDoG, float sigma = 1.0);

/** brief
Compute the difference of gaussian in the gradient way for one pixel
@param grayImg float gray img with value between 0 and 1
@param isophote, a Vector map containing the sense of edges
@param lxsT a point representing the value of current pixel
@param sKernelValues, the value of the DoG already computed
@return the 1d dog computed for given pixel in the gradient way
*/
float lineDoG(const Mat & grayImg, const Mat & isophote, Point2f & lxsT, Mat & tKernelValues);

/**
Compute the flow based DoG. Return a picture with black edges
@param img: a double grayScale picture with values btw 0 and 1.
@param isophote: a vector field map representing the flow with on vector at each pixel. Need to be in CV_32FC2
@param sigmaM, the standard deviation value used for the Gaussian Kernel at the flow. The bigger this value is, the more importance is given to long edges.
@param sigmaC. The std value for the kernel along the gradient. A more important value give bigger edges
@param rho, the noise sensibility value between 0.97 and 1. A big value gives more edges but can give noises.
@param threshold, the threshold used for binarisation step
@param maxIter, the maximim number of iteration. The first iteration supress noise, the other iterration inforce the edges already found a big number of iteration can give artifacts
*/
Mat fDoG(const Mat & img, const Mat & isophote, const float sigmaM = 3.0, const float sigmaC = 1.0, const float rho = 0.97, const float threshold = 0.5, int maxIter = 3);

#endif
