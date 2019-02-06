#ifndef GRADIENT_HPP
#define GRADIENT_HPP

#include <opencv2/opencv.hpp>

using namespace cv;

Mat getSobelGradient(const Mat & img) {
	Mat dx, dy;
	Mat ang, mag;
	//compute x gradient
	Mat doubleImg;
	img.convertTo(doubleImg, CV_32F, 1 / 255.0);
	Sobel(doubleImg, dx, CV_32F, 1, 0, 1);
	imshow("gradientX", dx);
	//compute y gradient
	Sobel(doubleImg, dy, CV_32F, 0, 1, 1);
	imshow("gradientY", dy);
	waitKey();
	cartToPolar(dx, dy, mag, ang);
	//get picture intensity
	return mag;
}



#endif
