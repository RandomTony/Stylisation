#ifndef UTILS_H
#define UTILS_H

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
float convolution(const Mat & pic, const Mat & kernel, const int r, const int c){
  float conv = 0;
  int posX, posY;
  int hMiddleKernel = kernel.rows/2;
  int vMiddleKernel = kernel.cols/2;
  for (int i = -hMiddleKernel; i <= hMiddleKernel; i++) {
    for (int j = -vMiddleKernel; j <= vMiddleKernel; j++) {
      if(((i+r)>=0 && (i+r)<pic.rows) && ((j+c)>=0 && (j+c)<pic.cols)){
        conv += pic.at<float>(i+r,j+c)*kernel.at<float>(i+hMiddleKernel,j+vMiddleKernel);
      }
    }
  }
  return conv;
}

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
bool isZeroCrossPixel(Mat & img, int y, int x, float threshold = 0.0){
  float neighboring[4][2];
  //diagonal 1
  neighboring[0][0] = img.at<float>(y-1,x-1);
  neighboring[0][1] = img.at<float>(y+1,x+1);
  //horizontal neighbour
  neighboring[1][0] = img.at<float>(y,x-1);
  neighboring[1][1] = img.at<float>(y,x+1);
  //diagonal 2
  neighboring[2][0] = img.at<float>(y+1,x-1);
  neighboring[2][1] = img.at<float>(y-1,x+1);
  //vertical neighbour
  neighboring[3][0] = img.at<float>(y+1,x);
  neighboring[3][1] = img.at<float>(y-1,x);
  for (size_t i = 0; i < 4; i++) {
    if(neighboring[i][0]*neighboring[i][1]<0 && abs(neighboring[i][0]-neighboring[i][1])>threshold){
      return true;
    }
  }
  return false;
}

/**
Compute the zero crossing map of the img Mat.
The border are avoided for a easier implementation
@param img, the provided picture to build the zeroCross map
@param threshold, the threshold choosen to filter the zero crossing pixels
@return The zero crossing map, with the zero cross pixels with a value of 255
*/
Mat zeroCrossingMat(Mat & img, float threshold = 0.0){
  Mat zeroCross(img.rows,img.cols,CV_8UC1);
  Mat dx, dy;
  //avoid the border
  for (int i = 1; i < (img.rows)-1; i++) {
    for (int j = 1; j < (img.cols)-1; j++) {
      zeroCross.at<uchar>(i,j) = isZeroCrossPixel(img,i,j,threshold) ? 255 : 0;
    }
  }
  return zeroCross;
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
  tensor.at<float>(1,0) = dx.at<float>(r,c) * dy.at<float>(r,c);
  tensor.at<float>(1,1) = dy.at<float>(r,c) * dy.at<float>(r,c);;
  return tensor;
}

/**
Compute eigen value and eigen vector with the tensor structure map
@param tensor, the tensor structure for one pixel. It is a 2x2 double mat
@param gradient, a pointer to a 2x1 Matrix representing the gradient
@param normal, a pointer to a 2x1 Matrix representing the isophote, the normal to the gradient
@param gAmplitude, the gradient amplitude
@param nAmplitude, the isophote amplitude
*/
void eigen(const Mat & tensor, Mat* gradient, Mat* normal, float* gAmplitude, float* nAmplitude){
  //first compute Delta as D=(g11-g22)²+4*(g12)²
  float delta = pow(tensor.at<float>(0,0)-tensor.at<float>(1,1),2)
               +4*pow(tensor.at<float>(0,1),2);
  *gAmplitude = (tensor.at<float>(0,0)*tensor.at<float>(1,1) + sqrt(delta))/2;
  *nAmplitude = (tensor.at<float>(0,0)*tensor.at<float>(1,1) - sqrt(delta))/2;
  gradient = new Mat(2,1,CV_32F);
  normal = new Mat(2,1,CV_32F);
  normal->at<float>(0,0) = 2*tensor.at<float>(0,1);
  normal->at<float>(0,1) = tensor.at<float>(0,0)-tensor.at<float>(1,1)-sqrt(delta);
  gradient->at<float>(0,0) = 2*tensor.at<float>(0,1);
  gradient->at<float>(0,1) = tensor.at<float>(0,0)-tensor.at<float>(1,1)+sqrt(delta);
}

double coherenceNorm(const float gAmplitude /*lambda+*/, const float nAmplitude /*lambda-*/){
  return pow((gAmplitude-nAmplitude)/(gAmplitude+nAmplitude),2);
}

double dpX(const float gAmplitude /*lambda+*/, const float nAmplitude /*lambda-*/, const float alpha, const float eta){
  return alpha+(1-alpha)*exp((-1/eta)/((gAmplitude-nAmplitude)/(gAmplitude+nAmplitude)));
}

/**
Compute Difference of Gaussian, an approximation of Laplacian of Gaussian

*/
Mat DoG(const Mat & pic, double s1, double s2){
  Mat firstGaussianBlur, secondGaussianBlur;
  if(s1<=s2){
    std::cerr << "S1<=S2: to compute DoG, s1 must be greater to s2" << '\n';
  }
  // if(double(s1)/double(s2)>=5.0/3.0){
  //   // printf("The two sigma have \n", );
  //   exit(1);
  // }
  GaussianBlur(pic,firstGaussianBlur,Size(0,0),s1);
  GaussianBlur(pic,secondGaussianBlur,Size(0,0),s2);
  imshow("gaussianBlur1",firstGaussianBlur);
  imshow("secondGaussianBlur",secondGaussianBlur);
  return firstGaussianBlur-secondGaussianBlur;
}

#endif
