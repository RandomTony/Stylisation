#ifndef TESTS_H
#define TESTS_H

#include <opencv2/highgui.hpp>
#include <opencv2/core/mat.hpp>

void testConvol();

void testDoG(cv::Mat & img);

void testTensor(cv::Mat & img);

void testAbstraction(cv::Mat & color);


#endif
