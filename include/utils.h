#ifndef UTILS_H
#define UTILS_H

#include <opencv2/opencv.hpp>

using namespace cv;

/**
Compute convolution between an picture and a kernel at a given pixel
@warn the two matrices pic and kernel need to be in float
@param pic, the picture that we want to convolve
@param kernel, the kernel that we want to convolve
@param r the row of the concerned pixel
@param l the line of the concerned pixel
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


#endif
