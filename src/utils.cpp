#include "utils.hpp"

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

bool isZeroCrossPixel(Mat & img, int y, int x, float threshold) {
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

Mat zeroCrossingMat(Mat & img, float threshold) {
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

Mat Quantification(const Mat & src, int k){
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

float kernelFromStandardDev(float standardDev){
  return (standardDev-0.8)*(20.0/3.0)+3.0;
}

int sign(float value){
  return value < 0? -1 : 1;
}

float gaussianValue(float x, float sigma){
  return 1.0/(sqrt(2.0*M_PI)*sigma)*exp(-(x*x)/(2.0*sigma*sigma));
}
