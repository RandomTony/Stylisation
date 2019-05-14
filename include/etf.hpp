#ifndef ETF_HPP
#define ETF_HPP

#include <opencv2/opencv.hpp>
#include <cmath>

using namespace cv;

/**
Compute the edge tangent flow for a given pixel.
the etf for a given pixel respect this formula
for all the pixel is the radius (sum),
compute his spacial weight*directionweight*magnitudeWeight
*/
Point2f edgeTangentFlow(const Mat& isophote, const Mat& gHat, const int oRow,
                        const int oColumn, const int radius = 5);

/**
Compute an ETF (Edge tangent Flow) field of vector.
Computing the ETF is in different step:
- For each pixel compute the edge tangent flow using an given isophote
  This part is detailled above the edgeTangentFlow() function
- Before iterating once again, normalize the finded etf and use it as isophote in the next iteration
This function is as light as possible to provide a freedom of parameters
@param iVectMap, a map of vector (CV_32FC2) containing the isophote (orthogonal to the gradient)
@param gHat, the magnitude of the gradient (CV_32FC1)
@param radius, the radius for the spacial weight
@param iteration, the number of iteration to smooth the etf
*/
Mat computeETF(const Mat& iVectMap, const Mat& gHat, const int radius = 5,
               int iteration = 3);

#endif
