#include "quantif.hpp"

Mat LuminanceQuant(const Mat & src, int clusts) {
	//cv::namedWindow("Source", WINDOW_NORMAL);
	//cv::imshow("Source", src);
	Mat lum;
	cv::cvtColor(src, lum, COLOR_BGR2Lab);
	std::vector<cv::Mat> vec_lum(3);
	split(lum, vec_lum);

	//Kmeans quantif
	Mat colVec = lum.reshape(1, lum.rows*lum.cols); // change to a Nx3 column vector
	//std::cout << "colVec is of size: " << colVec.rows << "x" << colVec.cols << endl;
	Mat colVecD, bestLabels, centers, clustered;
	int attempts = 5;
	//int clusts = 4;
	double eps = 0.001;
	colVec.convertTo(colVecD, CV_32F); // convert to floating point
	cv::kmeans(colVecD, clusts, bestLabels,
			   TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, attempts, eps),
			   attempts, KMEANS_PP_CENTERS, centers);

	for (int y = 0; y < lum.rows; y++) {
		for (int x = 0; x < lum.cols; x++) {
			int cluster_idx = bestLabels.at<int>(y*lum.cols+x, 0);
			lum.at<Vec3b>(y, x)[0] = centers.at<float>(cluster_idx, 0);
		}
	}


	cv::cvtColor(lum, lum, COLOR_Lab2BGR);
	//cv::namedWindow("Luminance Quantif", WINDOW_NORMAL);
	//cv::imshow("Luminance Quantif", lum);
	return lum;
}
