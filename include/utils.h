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
float convolution(const Mat & pic, const Mat & kernel, const int r, const int c) {
	float conv = 0;
	int hMiddleKernel = kernel.rows / 2;
	int vMiddleKernel = kernel.cols / 2;
	for (int i = -hMiddleKernel; i <= hMiddleKernel; i++) {
		for (int j = -vMiddleKernel; j <= vMiddleKernel; j++) {
			if (((i + r) >= 0 && (i + r) < pic.rows) && ((j + c) >= 0 && (j + c) < pic.cols)) {
				conv += pic.at<float>(i + r, j + c)*kernel.at<float>(i + hMiddleKernel, j + vMiddleKernel);
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
bool isZeroCrossPixel(Mat & img, int y, int x, float threshold = 0.0) {
	float neighboring[4][2];
	//diagonal 1
	neighboring[0][0] = img.at<float>(y - 1, x - 1);
	neighboring[0][1] = img.at<float>(y + 1, x + 1);
	//horizontal neighbour
	neighboring[1][0] = img.at<float>(y, x - 1);
	neighboring[1][1] = img.at<float>(y, x + 1);
	//diagonal 2
	neighboring[2][0] = img.at<float>(y + 1, x - 1);
	neighboring[2][1] = img.at<float>(y - 1, x + 1);
	//vertical neighbour
	neighboring[3][0] = img.at<float>(y + 1, x);
	neighboring[3][1] = img.at<float>(y - 1, x);
	for (size_t i = 0; i < 4; i++) {
		if (neighboring[i][0] * neighboring[i][1]<0 && abs(neighboring[i][0] - neighboring[i][1])>threshold) {
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
Mat zeroCrossingMat(Mat & img, float threshold = 0.0) {
	Mat zeroCross(img.rows, img.cols, CV_8UC1);
	Mat dx, dy;
	//avoid the border
	for (int i = 1; i < (img.rows) - 1; i++) {
		for (int j = 1; j < (img.cols) - 1; j++) {
			zeroCross.at<uchar>(i, j) = isZeroCrossPixel(img, i, j, threshold) ? 255 : 0;
		}
	}
	return zeroCross;
}


/**
Return a thresholded picture. The threshold is estimate with a percentage of the maximum\n
value of the picture. The value below the threshold are printed and the other ignored
@param img the picture that we want to threshold
@param thresholdPerc, the threshold between 0 and 1 represent a percentage of the maximum value
@return the thresholded picture
*/
Mat thresholdUpperValues(const Mat & img, const float thresholdPerc) {
	Mat imgThresholded(img.rows, img.cols, CV_8UC1);
	double min, max;
	float threshold;

	minMaxLoc(img, &min, &max);
	threshold = max * thresholdPerc;

	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			imgThresholded.at<uchar>(i, j) = img.at<float>(i, j) > threshold ? 255 : 0;
		}
	}

	return imgThresholded;
}

/**
Quantification

*/
Mat Quantification(const Mat & src, int k ){
  Mat quantif(src.rows,src.cols,src.type());
  float pas(255./k);
  for (int i= 0; i < (src.rows); i++) {
    for (int j = 0; j < (src.cols); j++) {
      quantif.at<Vec3b>(i,j)[0] = round((float)src.at<Vec3b>(i,j)[0]/pas)*pas;
      quantif.at<Vec3b>(i,j)[1] = round((float)src.at<Vec3b>(i,j)[1]/pas)*pas;
      quantif.at<Vec3b>(i,j)[2] = round((float)src.at<Vec3b>(i,j)[2]/pas)*pas;
      //std::cout<<quantif.at<uchar>(i,j)<<"\t";
    }
  }
  return quantif;
}

Mat Kmeans(const Mat & src, int k) {
	Mat quantif(src.rows, src.cols, CV_32F);
	Mat result = src.clone();
	Mat labels;
	for (int i = 0; i < (src.rows); i++) {
		for (int j = 0; j < (src.cols); j++) {
			quantif.at<float>(i, j) = (float)src.at<Vec3b>(i, j)[0];
		}
	}
	kmeans(quantif, k, labels,
		TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 10, 1.0),
		3, KMEANS_PP_CENTERS);
	for (int i = 0; i < (src.rows); i++) {
		for (int j = 0; j < (src.cols); j++) {
			result.at<Vec3b>(i, j)[0] = labels.at<float>(i,j);
		}
	}
	return result;
}

/**
Compute Gaussian size from standard deviation
Invert of computation found here https://docs.opencv.org/3.2.0/d4/d86/group__imgproc__filter.html#gac05a120c1ae92a6060dd0db190a61afa
For futher math explanation, see here https://stackoverflow.com/questions/14060017/calculate-the-gaussian-filters-sigma-using-the-kernels-size
*/
float kernelFromStandardDev(float standardDev){
  return (standardDev-0.8)*(20.0/3.0)+3.0;
}

/**
Return the sign of the value. if it is negative return -1 and 1 otherwise
@return -1 or 1 depending of the sign of value
*/
int sign(float value){
  return value < 0? -1 : 1;
}

/**
Compute the gaussian value of x with a given standard deviation
@param x the value to compute
@param sigma, the standard deviation value
@return Gaussian value at x with sigma std
*/
float gaussianValue(float x, float sigma){
  return 1.0/(sqrt(2.0*M_PI)*sigma)*exp(-(x*x)/(2.0*sigma*sigma));
}

#endif
