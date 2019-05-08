#ifndef TENSOR_HPP
#define TENSOR_HPP

#include <opencv2/opencv.hpp>

using namespace cv;

/**
Compute eigen value and eigen vector with the tensor structure map. Due to multiple
return, the eigen values need to be given as pointers.
@param tensor, the tensor structure for one pixel. It is a 2x2 double mat
@param gradient, a pointer to a 2x1 Matrix representing the gradient
@param normal, a pointer to a 2x1 Matrix representing the isophote, the normal to the gradient
@param gAmplitude, the gradient amplitude
@param iAmplitude, the isophote amplitude
*/
void eigen(const Mat & tensor, Point2f* gradient, Point2f* normal, float* gAmplitude, float* iAmplitude);

/**
Compute the tensor structure matrice of a given pixel
@param dx, the vertical gradient
@param dy, the horizontal gradient
@param r, the row of concerned pixel
@param c, the column of the concerned pixel
@return the tensor structure for the given pixel
*/
Mat tensorStructure(const Mat &  dx, const Mat & dy, const int r, const int c);

/**
Compute the tensor structure for the entire picture
@param dx, a float matrix for the vertical gradient
@param dy, a float matrix for the horizontal gradient
@return A 3 Mat array. Each matrix has the same number of column and rows that the given gradients.\n
        The first element of the array is G11 (dx²)\n
        The second element is G22 (dy²)\n
        The third element is G12 or G21 (dx*dy)
*/
Mat* tensorStructureArray(const Mat & dx, const Mat & dy);

/**
Compute the coherence norm for a given pixel. We need the eigen values obtained from \n
the tensor structure matrix.
@param gAmplitude, the gradient amplitude also called isophote or lambda+
@param iAmplitude, the Normal amplitude, also called lambda-
@return the coherence norm
*/
double coherenceNorm(const float gAmplitude /*lambda+*/, const float iAmplitude /*lambda-*/);

/**
DpX norm. The value of alpha define the minimum value of the norm.\n
This function needs the eigen values obtained from the tensor structure matrix.
eta determine the importance of gradient and normal amplitude. The lower it is the more importance is according.
@param gAmplitude, the gradient amplitude also called isophote or lambda+
@param iAmplitude, the normal amplitude also called lambda-
@param alpha, the regulation term for the minimum value of the norm
@param eta, the regulation term according more or less importance to the amplitude terms
@return the dpx norm
*/
double dpX(const float gAmplitude /*lambda+*/, const float iAmplitude /*lambda-*/, const float alpha, const float eta);

#endif
