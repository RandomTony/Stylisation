#ifndef QUANTIF_HPP
#define QUANTIF_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>

using namespace cv;

Mat LuminanceQuant(const Mat& src, int clusts);

#endif
