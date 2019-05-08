#include "tensor.hpp"
#include <opencv2/core/mat.hpp>
#include <cmath> //for math functions

void eigen(const Mat & tensor, Point2f* gradient, Point2f* normal, float* gAmplitude, float* iAmplitude){
  //compute Delta as D=(g11-g22)²+4*(g12)²
  float delta = pow(tensor.at<float>(0,0)-tensor.at<float>(1,1),2)
                + 4*pow(tensor.at<float>(0,1),2);

  *gAmplitude = (tensor.at<float>(0,0)*tensor.at<float>(1,1) + sqrt(delta))/2.0;
  *iAmplitude = (tensor.at<float>(0,0)*tensor.at<float>(1,1) - sqrt(delta))/2.0;

  normal->x = 2*tensor.at<float>(0,1);
  normal->y = tensor.at<float>(0,0)-tensor.at<float>(1,1)-sqrt(delta);
  gradient->x = 2*tensor.at<float>(0,1);
  gradient->y = tensor.at<float>(0,0)-tensor.at<float>(1,1)+sqrt(delta);
}

Mat tensorStructure(const Mat &  dx, const Mat & dy, const int r, const int c){
  Mat tensor(2,2,CV_32F);
  tensor.at<float>(0,0) = dx.at<float>(r,c)*dx.at<float>(r,c);
  tensor.at<float>(0,1) = dx.at<float>(r,c) * dy.at<float>(r,c);
  tensor.at<float>(1,0) = tensor.at<float>(0,1);
  tensor.at<float>(1,1) = dy.at<float>(r,c) * dy.at<float>(r,c);;
  return tensor;
}

Mat* tensorStructureArray(const Mat & dx, const Mat & dy){
  Mat* tensorArray = new Mat[3];
  Mat tensorIJ;
  tensorArray[0] = Mat(dx.rows,dx.cols,CV_32F);
  tensorArray[1] = Mat(dx.rows,dx.cols,CV_32F);
  tensorArray[2] = Mat(dx.rows,dx.cols,CV_32F);

  for (int i = 0; i < dx.rows; i++) {
    for (int j = 0; j < dx.cols; j++) {
      tensorIJ = tensorStructure(dx,dy,i,j); //compute tensor structure for given pixel
      tensorArray[0].at<float>(i,j) = tensorIJ.at<float>(0,0); //g11
      tensorArray[1].at<float>(i,j) = tensorIJ.at<float>(1,1); //g22
      tensorArray[2].at<float>(i,j) = tensorIJ.at<float>(0,1); //g12
    }
  }

  return tensorArray;
}

double coherenceNorm(const float gAmplitude /*lambda+*/, const float iAmplitude /*lambda-*/){
  return pow((gAmplitude-iAmplitude)/(gAmplitude+iAmplitude),2);
}

double dpX(const float gAmplitude /*lambda+*/, const float iAmplitude /*lambda-*/, const float alpha, const float eta){
  return alpha+(1-alpha)*exp((-eta)/(pow((gAmplitude-iAmplitude),2)));
}
