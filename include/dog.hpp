#ifndef DOG_HPP
#define DOG_HPP

#include <opencv2/opencv.hpp>

using namespace cv;

/**
Compute Difference of Gaussian, an approximation of Laplacian of Gaussian.
Paid attention about the two sigma: s1 must be greater than s2
@param Pic, the picture where we compute the DOg
@param s1, the greater sigma
@param s2, the lower sigma
@return the difference between two Gaussian blurs
*/
Mat DoG(const Mat & pic, double s1, double s2){
  Mat firstGaussianBlur, secondGaussianBlur;
  if(s1<=s2){
    std::cerr << "S1<=S2: to compute DoG, s1 must be greater to s2" << '\n';
  }
  GaussianBlur(pic,firstGaussianBlur,Size(0,0),s1);
  GaussianBlur(pic,secondGaussianBlur,Size(0,0),s2);
  return firstGaussianBlur-secondGaussianBlur;
}

#endif
