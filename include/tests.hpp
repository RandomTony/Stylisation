#ifndef TESTS_HPP
#define TESTS_HPP

#include <opencv2/highgui.hpp>
#include <opencv2/core/mat.hpp>
#include <string>

using namespace cv;

Mat testColorAndDoG(Mat & img, Mat & color);

void testConvol();

void testDoG(cv::Mat & img);

void testTensor(cv::Mat & img);

Mat testDrawing(Mat & img, Mat & color);

void testQuantif(Mat & img);

void testAbstraction(cv::Mat & color, std::string imgName);


#endif
