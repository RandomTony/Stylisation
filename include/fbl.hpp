#ifndef FBL_HPP
#define FBL_HPP

#include <opencv2/opencv.hpp>

using namespace cv;
/**
Compute the similarity color value between two colors. A big weight is given if
the two color are close
@param oValue, the origin color value
@param cValue, the current color value
@param sigma, the std value
@return the weight that current point value as compared to origin point
*/
float similarityWeight(const Vec3f & oValue, const Vec3f & cValue, float sigma);

/**
Compute the flow based bilateral filter. To preserve the edges, we pass
a first bilteral filter along the gradient for each pixel and we iterate with a
second bilteral in the edge curve
@param img, the colored picture
@param isophote, the vector map representing the edges
@param sigmaE, the std value used for the spacial part along the isophote
@param sigmaG, the std value used for the spacial part along the gradient
@param sigmaRE, the std value used for the color part along the isophote
@param sigmaRG, the std value used for the color part alogn the gradient
@param maxIter, the number of iteration. One iteration pass a bilteral filter along the gradient and the edges
@return the input image treated with a flow based bilteral filter
*/
Mat computeFBL(const Mat & img, const Mat & isophote, const float sigmaE, const float sigmaG, const float sigmaRE, const float sigmaRG, const int maxIter = 4);

#endif
