#ifndef GRADIENT_HPP
#define GRADIENT_HPP

#include <opencv2/opencv.hpp>

using namespace cv;

Mat getSobelGradient(const Mat& img);

Mat getLapacian(const Mat& im);

#endif
