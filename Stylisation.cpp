#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/core/mat.hpp>
#include "include/gradient.h"
#include "include/utils.h"
#include "include/dog.h"
#include "include/tensor.h"
#include "include/etf.h"
#include"lib/lime-master/sources/lime.hpp"

using namespace cv;
using namespace std;

void testConvol() {
	Mat test(9, 9, CV_32F, 0.0);
	test.at<float>(4, 4) = 1.0;
	test.at<float>(3, 3) = 1.0;
	Mat kernel(3, 3, CV_32F);
	int i = 1;
	for (i = 1; i < 10; i++) {
		kernel.at<float>(i - 1) = i;
	}
	std::cout << kernel << '\n';
	std::cout << test << '\n';
	std::cout << "Convolution result: " << convolution(test, kernel, 3, 3) << '\n'; //must be 14 (5+9)
}

Mat testDoG(Mat & img) {
	Mat imgdouble;
	img.convertTo(imgdouble, CV_32F, 1 / 255.0);
	Mat dog = DoG(imgdouble, 5.0, 3.0);
	cv::namedWindow("DoG", WINDOW_NORMAL);
	cv::imshow("DoG", dog);
	// std::cout << dog << '\n';
	Mat test = zeroCrossingMat(dog);
	cv::namedWindow("passage par zero dog, threshold 0 percent", WINDOW_NORMAL);
	cv::imshow("passage par zero dog, threshold 0 percent", test);
	double min, max;
	minMaxLoc(dog, &min, &max);
	float threshold = max * 4 / 100;
	Mat testThreshold = zeroCrossingMat(dog, threshold);
	cv::namedWindow("passage par zero dog, threshold 4 percent", WINDOW_NORMAL);
	cv::imshow("passage par zero dog, threshold 4 percent", testThreshold);
	return testThreshold;
}


Mat testColorAndDoG(Mat & img, Mat & color) {
	Mat imgdouble;
	img.convertTo(imgdouble, CV_32F, 1 / 255.0);
	Mat dog = DoG(imgdouble, 5.0, 3.0);
	//namedWindow("DoG",WINDOW_NORMAL);
	//imshow("DoG",dog);
	double min, max;
	minMaxLoc(dog, &min, &max);
	float threshold = max * 4 / 100;
	Mat edges = zeroCrossingMat(dog, threshold);
	float an = 3;
	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(an * 2 + 1, an * 2 + 1), Point(an, an));
	//dilate(edges,edges,element);
	//namedWindow("passage par zero dog, threshold 4 percent",WINDOW_NORMAL);
	//imshow("passage par zero dog, threshold 4 percent", edges);
	Mat coloredEdges(color.rows,color.cols,color.type());// = Quantification(color, 20);
	float pas(255. / 50);
	//dilate(coloredEdges,coloredEdges,element);
	//GaussianBlur(coloredEdges, coloredEdges, Size(an * 2 + 1, an * 2 + 1), 3.5);
	//Mat coloredEdges(color);
	cvtColor(color, coloredEdges, CV_BGR2Lab);
	for (int i(0);i < color.rows;i++) {
		for (int j(0);j < color.cols;j++) {
			if (edges.at<uchar>(i, j) == 255) {
				coloredEdges.at<Vec3b>(i, j)[0] *= 0.75;//color.at<Vec3b>(i, j)[0] * 1.25;
			}/*
			else {
				coloredEdges.at<Vec3b>(i, j) = color.at<Vec3b>(i, j);
/*				coloredEdges.at<Vec3b>(i, j)[1] = round((float)color.at<Vec3b>(i, j)[1] / pas)*pas;
				coloredEdges.at<Vec3b>(i, j)[2] = round((float)color.at<Vec3b>(i, j)[2] / pas)*pas;
			}*/
		}
	}
	//namedWindow("Colored edges",WINDOW_NORMAL);
	//imshow("Colored edges", coloredEdges);
	cvtColor(coloredEdges, coloredEdges, CV_Lab2BGR);
	return coloredEdges;
}

void testQuantif(Mat & img) {
	float an = 10;
	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(an * 2 + 1, an * 2 + 1), Point(an, an));
	Mat color = Quantification(img, 10);
	dilate(color, color, element);
	GaussianBlur(color, color, Size(an * 2 + 1, an * 2 + 1), 3.5);
	cv::namedWindow("Colored edges", WINDOW_NORMAL);
	cv::imshow("Colored edges", color);
}

Mat testDrawing(Mat & img, Mat & color) {
	Mat imgdouble;
	img.convertTo(imgdouble, CV_32F, 1 / 255.0);
	Mat dog = DoG(imgdouble, 5.0, 3.0);
	//namedWindow("DoG",WINDOW_NORMAL);
	//imshow("DoG",dog);
	double min, max;
	minMaxLoc(dog, &min, &max);
	float threshold = max * 4 / 100;
	Mat edges = zeroCrossingMat(dog, threshold);
	Mat canvas(color.rows, color.cols, color.type());
	for (int i(0);i < canvas.rows;i++) {
		for (int j(0);j < canvas.cols;j++) {
			canvas.at<Vec3b>(i, j)[0] = 255;
			canvas.at<Vec3b>(i, j)[1] = 255;
			canvas.at<Vec3b>(i, j)[2] = 255;
		}
	}
	int length(5);
	for (int i(5);i < edges.rows-5;i++) {
		for (int j(5);j < edges.cols-5;j++) {
			if (edges.at<uchar>(i, j) == 255) {
				Scalar c(color.at<Vec3b>(i, j)[0], color.at<Vec3b>(i, j)[1], color.at<Vec3b>(i, j)[2]);
				if (edges.at<uchar>(i, j + 2) == 255) {
					if (edges.at<uchar>(i + 2, j + 2) == 255) {
						line(canvas, Point(j - length, i - length), Point(j + length, i + length), c);
					}
					else if (edges.at<uchar>(i - 2, j + 2) == 255) {
						line(canvas, Point(j - length, i + length), Point(j + length, i - length), c);
					}
					else {
						line(canvas, Point(j - length, i), Point(j + length, i), c);
					}
				}
				else if (edges.at<uchar>(i + 2, j) == 255) {
					line(canvas, Point(j, i - length), Point(j, i + length), c);
				}
			}

			//line(color,Point(j,i-5.5),Point(j+1.5,i+5),Scalar(12.,12.,12.));
		}
	}
	//namedWindow("Drawing test", WINDOW_NORMAL);
	//imshow("Drawing test", color);

	return canvas;
}

Mat testTensor(Mat & img,Mat & color) {
	Mat dx, dy;
	Mat doubleImg;
	Mat gradientVector(2,1,CV_32F), normalVector(2, 1, CV_32F);
	Mat isophote(img.rows, img.cols, CV_32F);
	Mat normal(img.rows, img.cols, CV_32F);
	Mat coherence(img.rows, img.cols, CV_32F);
	Mat dpx(img.rows, img.cols, CV_32F);
	Mat* smoothedTensor = new Mat[3];
	Mat* tensorArray;
	Mat tensor(2, 2, CV_32F);
	Mat u8Normal, u8Isophote, u8Coherence, u8DpX;
	double min, max;

	Mat drawing(img.rows, img.cols, color.type());
	for (size_t i = 0; i < img.rows; i++) {
		for (size_t j = 0; j < img.cols; j++) {
			drawing.at<Vec3b>(i, j)[0] = 255.;
			drawing.at<Vec3b>(i, j)[1] = 255.;
			drawing.at<Vec3b>(i, j)[2] = 255.;
		}
	}
	img.convertTo(doubleImg, CV_32F, 1 / 255.0);
	Sobel(doubleImg, dx, CV_32F, 1, 0, 1);
	Sobel(doubleImg, dy, CV_32F, 0, 1, 1);

	tensorArray = tensorStructureArray(dx, dy);

	GaussianBlur(tensorArray[0], smoothedTensor[0], Size(0, 0), 1);
	GaussianBlur(tensorArray[1], smoothedTensor[1], Size(0, 0), 1);
	GaussianBlur(tensorArray[2], smoothedTensor[2], Size(0, 0), 1);
	
	for (size_t i = 0; i < img.rows; i++) {
		for (size_t j = 0; j < img.cols; j++) {
			Scalar c(color.at<Vec3b>(i,j)[0], color.at<Vec3b>(i, j)[1], color.at<Vec3b>(i, j)[2]);
			tensor.at<float>(0, 0) = smoothedTensor[0].at<float>(i, j);
			tensor.at<float>(0, 1) = smoothedTensor[2].at<float>(i, j);
			tensor.at<float>(1, 0) = smoothedTensor[2].at<float>(i, j);
			tensor.at<float>(1, 1) = smoothedTensor[1].at<float>(i, j);
			eigen(tensor, &gradientVector, &normalVector, &isophote.at<float>(i, j), &normal.at<float>(i, j));
			coherence.at<float>(i, j) = coherenceNorm(isophote.at<float>(i, j), normal.at<float>(i, j));
			dpx.at<float>(i, j) = dpX(isophote.at<float>(i, j), normal.at<float>(i, j), 0.1, 0.0001);
			
			
		}
	}
	minMaxLoc(normal, &min, &max);
	std::cout << "Normal:" << '\n';
	std::cout << "min: " << min << " max: " << max << '\n';
	normal.convertTo(u8Normal, CV_8UC1, 255.0 / max);
	minMaxLoc(isophote, &min, &max);
	std::cout << "Isophote" << '\n';
	std::cout << "min: " << min << " max: " << max << '\n';
	isophote.convertTo(u8Isophote, CV_8UC1, 255.0 / max);
	minMaxLoc(isophote, &min, &max);
	std::cout << "Coherence Norm:" << '\n';
	minMaxLoc(coherence, &min, &max);
	std::cout << "min: " << min << " max: " << max << '\n';
	coherence.convertTo(u8Coherence, CV_8UC1, 255.0 / max);
	std::cout << "DpX Norm:" << '\n';
	minMaxLoc(dpx, &min, &max);
	std::cout << "min: " << min << " max: " << max << '\n';
	dpx.convertTo(u8DpX, CV_8UC1, 255.0 / max);
	/*
	namedWindow("normale", WINDOW_NORMAL);
	imshow("normale", u8Normal);
	namedWindow("isophote", WINDOW_NORMAL);
	imshow("isophote", u8Isophote);
	namedWindow("Coherence", WINDOW_NORMAL);
	imshow("Coherence", coherence);
	namedWindow("dpx", WINDOW_NORMAL);
	imshow("dpx", u8DpX);*/
	
	//threshold
	Mat imgToShow = thresholdUpperValues(dpx, 0.7);
	//namedWindow("Norm thresholded", WINDOW_NORMAL);
	//imshow("Norm thresholded", imgToShow);

	Mat imgModified = color.clone();
	//imgToShow.convertTo(imgToShow, CV_32F);
	for (size_t i = 0; i < img.rows; i++) {
		for (size_t j = 0; j < img.cols; j++) {
			if (imgToShow.at<uchar>(i, j) == 255) {
				imgModified.at<Vec3b>(i, j)[0] = 0;
				imgModified.at<Vec3b>(i, j)[1] = 0;
				imgModified.at<Vec3b>(i, j)[2] = 0;
			}
		}
	}

	cv::namedWindow("img modif", WINDOW_NORMAL);
	cv::imshow("img modif", imgModified);
	return imgModified;
	//waitKey();
}

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
	double compactness = cv::kmeans(colVecD, clusts, bestLabels,
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

Mat bilateral(Mat & src) {
	Mat result = src.clone();
	Mat temp = src.clone();
	for (int i(1);i < 21;i+=2) {
		bilateralFilter(temp, result, i, i*2, i/2);
		temp = result.clone();
	}
	cv::namedWindow("Abstraction", WINDOW_NORMAL);
	cv::imshow("Abstraction", result);
	return result;
}


Mat colorETF(Mat & grayImg, Mat & src) {
	Mat floatImg;
	Mat gradX, gradY;
	Mat gVectMap = Mat::zeros(grayImg.rows, grayImg.cols, CV_32FC2);
	Mat isoVectMap = Mat::zeros(grayImg.rows, grayImg.cols, CV_32FC2);
	Mat gHat = cv::Mat::zeros(grayImg.rows, grayImg.cols, CV_32FC1);

	grayImg.convertTo(floatImg, CV_32F, 1 / 255.0);

	Sobel(floatImg, gradX, CV_32F, 1, 0);
	Sobel(floatImg, gradY, CV_32F, 0, 1);
	/*
	for (int y = 0; y < grayImg.rows; y++) {
		for (int x = 0; x < grayImg.cols; x++) {
			float gx = gradX.at<float>(y, x);
			float gy = gradY.at<float>(y, x);
			gVectMap.at<float>(y, x * 2 + 0) = static_cast<float>(gy);
			gVectMap.at<float>(y, x * 2 + 1) = static_cast<float>(gx);

			float mag = sqrt(gx*gx + gy * gy);

			gHat.at<float>(y, x) = mag;

			isoVectMap.at<float>(y, x * 2 + 0) = static_cast<float>(-gy / (mag + 1.0e-8));
			isoVectMap.at<float>(y, x * 2 + 1) = static_cast<float>(gx / (mag + 1.0e-8));
		}
	}*/
	
	Mat etf,noise,lic;
	std::cout << "ETF\n";
	lime::randomNoise(noise, cv::Size(grayImg.cols, grayImg.rows));
	//lime::calcETF(floatImg, etf);
	//lime::LIC(noise, lic, etf, 20, lime::LIC_CLASSICAL);
	//GaussianBlur(lic, lic, Size(5, 5), 0);
	//imshow("LIC lime", lic);
	//std::cout << lic.rows << "  " << lic.cols;
	etf = computeETF(gVectMap, isoVectMap, gHat, 5, 3);
	Mat lab = src.clone();
	vector<Mat> vec_mat;
	//cv::cvtColor(src, lab, CV_BGR2Lab);
	
	Mat temp = lab.clone();
	int kern_size = 2;
	Mat kernel = getGaussianKernel(2*kern_size+1, -1,CV_32F);
	
	std::cout << "Abstraction\n";
	for (int a = 0;a < 4;a++) {
		for (int y = 0; y < src.rows; y++) {
			for (int x = 0; x < src.cols; x++) {
				lab.at<Vec3b>(y, x)[0] = 0;
				lab.at<Vec3b>(y, x)[1] = 0;
				lab.at<Vec3b>(y, x)[2] = 0;
				float vec_y = etf.at<float>(y, x * 2 + 0);
				float vec_x = etf.at<float>(y, x * 2 + 1);
				for (int k(-kern_size);k < kern_size+1;k++) {
					int i = (int)(x + k * vec_x);
					int j = (int)(y + k * vec_y);
					if (j > 0 && j < src.rows && i>0 && i < src.cols) {
						lab.at<Vec3b>(y, x)[0] += static_cast<float>(kernel.at<float>(kern_size + k)) * (float)temp.at<Vec3b>(j, i)[0];
						lab.at<Vec3b>(y, x)[1] += static_cast<float>(kernel.at<float>(kern_size  + k)) * (float)temp.at<Vec3b>(j, i)[1];
						lab.at<Vec3b>(y, x)[2] += static_cast<float>(kernel.at<float>(kern_size + k)) * (float)temp.at<Vec3b>(j, i)[2];
					}
				}
			}
		}
		temp = lab.clone();
		for (int y = 0; y < src.rows; y++) {
			for (int x = 0; x < src.cols; x++) {
				lab.at<Vec3b>(y, x)[0] = 0;
				lab.at<Vec3b>(y, x)[1] = 0;
				lab.at<Vec3b>(y, x)[2] = 0;
				float vec_x = etf.at<float>(y, x * 2 + 0);
				float vec_y = etf.at<float>(y, x * 2 + 1);
				for (int k(-kern_size);k < kern_size+1;k++) {
					int i = (int)(x + k * vec_x);
					int j = (int)(y + k * vec_y);
					if (j > 0 && j < src.rows && i>0 && i < src.cols) {
						lab.at<Vec3b>(y, x)[0] += static_cast<float>(kernel.at<float>(kern_size + k)) * (float)temp.at<Vec3b>(j, i)[0];
						lab.at<Vec3b>(y, x)[1] += static_cast<float>(kernel.at<float>(kern_size + k)) * (float)temp.at<Vec3b>(j, i)[1];
						lab.at<Vec3b>(y, x)[2] += static_cast<float>(kernel.at<float>(kern_size + k)) * (float)temp.at<Vec3b>(j, i)[2];
					}
				}
				
			}
		}
		temp = lab.clone();
		//GaussianBlur(lab, lab, Size(3, 3), 0);
	}
	
	//cv::cvtColor(lab, lab, CV_Lab2BGR);
	cv::imshow("Abstracted", lab);
	std::cout << "Quantification\n";
	namedWindow("end", WINDOW_NORMAL);
	cv::imshow("end", LuminanceQuant(lab,4));
	return lab;

	/*
	
	Mat lic, noise;
	lime::randomNoise(noise, cv::Size(grayImg.cols, grayImg.rows));
	lime::LIC(noise, lic, etf, 20, lime::LIC_EULERIAN);
	namedWindow("LIC homeMade", WINDOW_NORMAL);
	imshow("LIC homeMade", lic);
	Mat BnW;
	lic.convertTo(BnW, CV_8UC1, 255.0);
	imwrite("testLIC.png", BnW);
	waitKey();

	Mat lic2, etf2;
	lime::calcETF(floatImg, etf2);
	lime::LIC(noise, lic2, etf2, 20, lime::LIC_EULERIAN);
	namedWindow("LIC lime", WINDOW_NORMAL);
	imshow("LIC lime", lic);
	waitKey();
	*/
}

char *itoa_simple(char *dest, int i) {
	char *s = dest;
	if (i < 0) {
		*s++ = '-';
	}
	else {
		i = -i;
	}
//	*itoa_simple_helper(s, i) = '\0';
	return dest;
}
int main(int argc, char const *argv[]) {
  Mat im;
  string path = "C:/Users/antho/Pictures/Wallpapers/";
  //string path = "C:/Users/antho/source/repos/Style/Style/ressources/highDefinitionUnsplash/";
  //string file = "0.jpg";
  //im = imread(path+file,CV_LOAD_IMAGE_COLOR);
  //im = imread("C:/Users/antho/source/repos/Style/Style/ressources/PersoData/sample3.jpg",CV_LOAD_IMAGE_COLOR);
  im = imread("C:/Users/antho/source/repos/Style/Style/res/me.jpg",CV_LOAD_IMAGE_COLOR);

  
  /*
  char name[4];
  
  for (int i(0);i < 40;i++) {
	  sprintf_s(name, "%d", i);
	  im = imread(path + name+".jpg", CV_LOAD_IMAGE_COLOR);
	  
	  if (!im.data)                              // Check for invalid input
	  {
		  cout << "Could not open or find the image" << name << std::endl;
	  }
	  else {
		  Mat imGrayScale;
		  cv::cvtColor(im, imGrayScale, COLOR_BGR2GRAY);
		  Mat abstracted = bilateral(im);
		  Mat color;
		  
		  Mat quantif = LuminanceQuant(abstracted,4);
		  
		  cv::cvtColor(quantif, imGrayScale, COLOR_BGR2GRAY);
		  GaussianBlur(quantif, color, Size(5, 5), 0);

		  quantif = testColorAndDoG(imGrayScale, color);
		  imwrite("C:/Users/antho/source/repos/Style/Style/res/test/" + string(name )+".jpg", quantif);
		  std::cout << path + name + ".jpg\n";
	  }
  }*/

  
  Mat imGrayScale;
  cv::cvtColor(im, imGrayScale, COLOR_BGR2GRAY);
  //Mat quantified_pic = bilateral(im);
  //quantified_pic = LuminanceQuant(quantified_pic);
  //Mat abstracted = colorETF(imGrayScale, im);
  Mat abstracted = colorETF(imGrayScale,im);
  cv::cvtColor(abstracted, imGrayScale, COLOR_BGR2GRAY);
  //Mat edges = testDoG(imGrayScale);
  //Mat quantif = LuminanceQuant(abstracted,4);
  
  //GaussianBlur(quantif, quantif, Size(5, 5), 0);
  //quantif = testColorAndDoG(imGrayScale, quantif);
  
  namedWindow("quantif", WINDOW_NORMAL);
  imshow("quantif", LuminanceQuant(im,4));
//  Mat final = testTensor(imGrayScale, quantified_pic);
  //imwrite("C:/Users/antho/source/repos/Style/Style/res/ass2.jpg", quantified_pic);
  cv::waitKey();
 
  return 0;
}



void doVid(string source) {
	const bool askOutputType = true;  // If false it will use the inputs codec type

	VideoCapture inputVideo(source);              // Open input
	if (!inputVideo.isOpened())
	{
		std::cout << "Could not open the input video: " << source << endl;
		return ;
	}

	string::size_type pAt = source.find_last_of('.');                  // Find extension point
	const string NAME = source.substr(0, pAt) + "quantifgresult" + ".mp4";   // Form the new name with container
	int ex = static_cast<int>(inputVideo.get(CV_CAP_PROP_FOURCC));     // Get Codec Type- Int form

	// Transform from int to char via Bitwise operators
	char EXT[] = { (char)(ex & 0XFF) , (char)((ex & 0XFF00) >> 8),(char)((ex & 0XFF0000) >> 16),(char)((ex & 0XFF000000) >> 24), 0 };

	Size S = Size((int)inputVideo.get(CV_CAP_PROP_FRAME_WIDTH),    // Acquire input size
		(int)inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT));

	VideoWriter outputVideo;                                        // Open the output
	if (askOutputType)
		outputVideo.open(NAME, ex = -1, inputVideo.get(CV_CAP_PROP_FPS), S, true);
	else
		outputVideo.open(NAME, ex, inputVideo.get(CV_CAP_PROP_FPS), S, true);

	if (!outputVideo.isOpened())
	{
		std::cout << "Could not open the output video for write: " << source << endl;
		return ;
	}

	std::cout << "Input frame resolution: Width=" << S.width << "  Height=" << S.height
		<< " of nr#: " << inputVideo.get(CV_CAP_PROP_FRAME_COUNT) << endl;
	std::cout << "Input codec type: " << EXT << endl;

	int n(inputVideo.get(CV_CAP_PROP_FRAME_COUNT));
	int i(0);
	
	Mat src, res;
	vector<Mat> spl;
	int end = n > 00 ? 500 : n;
	int k(0);
	for (int i(0);i < end;i++) //Show the image captured in the window and repeat
	{
		inputVideo >> src;              // read
		if (src.empty()) break;         // check if at end

		if (i>100 && k<10 ) {
			res = LuminanceQuant(bilateral(src),4);
		}
		else {
			res = src.clone();
			k = k > 20 ? k : 0;
		}

		outputVideo << res;
		std::cout << "[" << i << "/" << n << "]" << endl;
		k++;
	}

}
/*
int main(int argc, char *argv[])
{
	const string source = "C:/Users/antho/source/repos/Style/Style/ressources/video/becky.mp4";           // the source file name
	const bool askOutputType = true;  // If false it will use the inputs codec type

	//doVid("C:/Users/antho/source/repos/Style/Style/ressources/video/becky.mp4");
	doVid("C:/Users/antho/source/repos/Style/Style/ressources/video/dance.mp4");
	cout << "Finished writing" << endl;
	return 0;
}
*/