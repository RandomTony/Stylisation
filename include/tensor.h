#ifndef TENSOR_H
#define TENSOR_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <cmath> //for math functions

using namespace cv;

/**
Compute eigen value and eigen vector with the tensor structure map. Due to multiple
return, the eigen values need to be given as pointers.
@param tensor, the tensor structure for one pixel. It is a 2x2 double mat
@param gradient, a pointer to a 2x1 Matrix representing the gradient
@param normal, a pointer to a 2x1 Matrix representing the isophote, the normal to the gradient
@param gAmplitude, the gradient amplitude
@param nAmplitude, the isophote amplitude
*/
void eigen(const Mat & tensor, Mat* gradient, Mat* normal, float* gAmplitude, float* nAmplitude){
  gradient = new Mat(2,1,CV_32F);
  normal = new Mat(2,1,CV_32F);
  //compute Delta as D=(g11-g22)²+4*(g12)²
  float delta = pow(tensor.at<float>(0,0)-tensor.at<float>(1,1),2)
                + 4*pow(tensor.at<float>(0,1),2);

  *gAmplitude = (tensor.at<float>(0,0)*tensor.at<float>(1,1) + sqrt(delta))/2.0;
  *nAmplitude = (tensor.at<float>(0,0)*tensor.at<float>(1,1) - sqrt(delta))/2.0;

  normal->at<float>(0,0) = 2*tensor.at<float>(0,1);
  normal->at<float>(0,1) = tensor.at<float>(0,0)-tensor.at<float>(1,1)-sqrt(delta);
  gradient->at<float>(0,0) = 2*tensor.at<float>(0,1);
  gradient->at<float>(0,1) = tensor.at<float>(0,0)-tensor.at<float>(1,1)+sqrt(delta);
}

/**
Compute the tensor structure matrice of a given pixel
@param dx, the vertical gradient
@param dy, the horizontal gradient
@param r, the row of concerned pixel
@param c, the column of the concerned pixel
@return the tensor structure for the given pixel
*/
Mat tensorStructure(const Mat &  dx, const Mat & dy, const int r, const int c){
  Mat tensor(2,2,CV_32F);
  tensor.at<float>(0,0) = dx.at<float>(r,c)*dx.at<float>(r,c);
  tensor.at<float>(0,1) = dx.at<float>(r,c) * dy.at<float>(r,c);
  tensor.at<float>(1,0) = tensor.at<float>(0,1);
  tensor.at<float>(1,1) = dy.at<float>(r,c) * dy.at<float>(r,c);;
  return tensor;
}

/**
Compute the tensor structure for the entire picture
@param dx, a float matrix for the vertical gradient
@param dy, a float matrix for the horizontal gradient
@return A 3 Mat array. Each matrix has the same number of column and rows that the given gradients.\n
        The first element of the array is G11 (dx²)\n
        The second element is G22 (dy²)\n
        The third element is G12 or G21 (dx*dy)
*/
Mat* tensorStructureArray(const Mat & dx, const Mat & dy){
  Mat* tensorArray = new Mat[3];
  Mat tensorIJ;
  tensorArray[0] = Mat(dx.rows,dx.cols,CV_32F);
  tensorArray[1] = Mat(dx.rows,dx.cols,CV_32F);
  tensorArray[2] = Mat(dx.rows,dx.cols,CV_32F);

  for (size_t i = 0; i < dx.rows; i++) {
    for (size_t j = 0; j < dx.cols; j++) {
      tensorIJ = tensorStructure(dx,dy,i,j); //compute tensor structure for given pixel
      tensorArray[0].at<float>(i,j) = tensorIJ.at<float>(0,0); //g11
      tensorArray[1].at<float>(i,j) = tensorIJ.at<float>(1,1); //g22
      tensorArray[2].at<float>(i,j) = tensorIJ.at<float>(0,1); //g12
    }
  }

  return tensorArray;
}

/**
Compute the coherence norm for a given pixel. We need the eigen values obtained from \n
the tensor structure matrix.
@param gAmplitude, the gradient amplitude also called isophote or lambda+
@param nAmplitude, the Normal amplitude, also called lambda-
@return the coherence norm
*/
double coherenceNorm(const float gAmplitude /*lambda+*/, const float nAmplitude /*lambda-*/){
  return pow((gAmplitude-nAmplitude)/(gAmplitude+nAmplitude),2);
}

/**
DpX norm. The value of alpha define the minimum value of the norm.\n
This function needs the eigen values obtained from the tensor structure matrix.
eta determine the importance of gradient and normal amplitude. The lower it is the more importance is according.
@param gAmplitude, the gradient amplitude also called isophote or lambda+
@param nAmplitude, the normal amplitude also called lambda-
@param alpha, the regulation term for the minimum value of the norm
@param eta, the regulation term according more or less importance to the amplitude terms
@return the dpx norm
*/
double dpX(const float gAmplitude /*lambda+*/, const float nAmplitude /*lambda-*/, const float alpha, const float eta){
  return alpha+(1-alpha)*exp((-eta)/(pow((gAmplitude-nAmplitude),2)));
}

#endif
