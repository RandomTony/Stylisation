#ifndef BILATERAL_HPP
#define BILATERAL_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>

using namespace cv;

Mat bilateral(Mat & src) {
  Mat result = src.clone();
  Mat temp = src.clone();
  for (int i(1);i < 21;i+=2) {
    temp = result.clone();
    bilateralFilter(temp, result, i, i*2, i/2);
  }
  cv::namedWindow("Abstraction", WINDOW_NORMAL);
  cv::imshow("Abstraction", result);
  return result;
}

#endif /* end of include guard: BILATERAL_HPP */
