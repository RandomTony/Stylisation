#include "bilateral.hpp"

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
