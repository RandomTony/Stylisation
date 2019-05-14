#ifndef UTILS_HPP
#define UTILS_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <cmath> //for math functions

using namespace cv;

/**
Compute convolution between an picture and a kernel at a given pixel
@warn the two matrices pic and kernel need to be in float
@param pic, the picture that we want to convolve
@param kernel, the kernel that we want to convolve
@param r the row of the concerned pixel
@param c the column of the concerned pixel
@return the result of convolution
*/
float convolution(const Mat & pic, const Mat & kernel, const int r,
                  const int c);

/**
This function say if the pixel img(y,x) is a zero crossing pixel
A zero crossing pixel *p* implies that the signs of at least two
of its opposing neighboring pixels must differ.
@param img
@param y, the concerned row of p
@param x, the concerned col of p
@param threshold, the threshold that the absolute value of the neighbour difference must exceed
@return whether if img(y,x) is a zero crossing pixel
*/
bool isZeroCrossPixel(Mat & img, int y, int x, float threshold = 0.0);

/**
Compute the zero crossing map of the img Mat.
The border are avoided for a easier implementation
@param img, the provided picture to build the zeroCross map
@param threshold, the threshold choosen to filter the zero crossing pixels
@return The zero crossing map, with the zero cross pixels with a value of 255
*/
Mat zeroCrossingMat(Mat & img, float threshold = 0.0);

/**
Return a thresholded picture. The threshold is estimate with a percentage of the maximum\n
value of the picture. The value below the threshold are printed and the other ignored
@param img the picture that we want to threshold
@param thresholdPerc, the threshold between 0 and 1 represent a percentage of the maximum value
@return the thresholded picture
*/
Mat thresholdUpperValues(const Mat & img, const float thresholdPerc);

/**
Quantification

*/
Mat Quantification(const Mat & src, int k );

Mat Kmeans(const Mat & src, int k);

/**
Compute Gaussian size from standard deviation
Invert of computation found here https://docs.opencv.org/3.2.0/d4/d86/group__imgproc__filter.html#gac05a120c1ae92a6060dd0db190a61afa
For futher math explanation, see here https://stackoverflow.com/questions/14060017/calculate-the-gaussian-filters-sigma-using-the-kernels-size
*/
float kernelFromStandardDev(float standardDev);

/**
Return the sign of the value. if it is negative return -1 and 1 otherwise
@return -1 or 1 depending of the sign of value
*/
int sign(float value);

/**
Compute the gaussian value of x with a given standard deviation
@param x the value to compute
@param sigma, the standard deviation value
@return Gaussian value at x with sigma std
*/
float gaussianValue(float x, float sigma);

#endif
