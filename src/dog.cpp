#include "dog.hpp"

Mat DoG(const Mat & pic, double s1, double s2){
  Mat firstGaussianBlur, secondGaussianBlur;
  if(s1<=s2){
    std::cerr << "S1<=S2: to compute DoG, s1 must be greater to s2" << '\n';
  }
  GaussianBlur(pic,firstGaussianBlur,Size(0,0),s1);
  GaussianBlur(pic,secondGaussianBlur,Size(0,0),s2);
  return firstGaussianBlur-secondGaussianBlur;
}
