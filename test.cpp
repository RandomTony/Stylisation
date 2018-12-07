#include <iostream>
#include <gradient.h>
#include <opencv2/highgui.hpp>
#include <utils.h>
#include <tensor.h>
#include <dog.h>
#include <opencv2/core/mat.hpp>

using namespace cv;

void testConvol(){
  Mat test(9,9,CV_32F,0.0);
  test.at<float>(4,4)=1.0;
  test.at<float>(3,3)=1.0;
  Mat kernel(3,3,CV_32F);
  int i = 1;
  for(i=1; i<10; i++){
    kernel.at<float>(i-1)=i;
  }
  std::cout << kernel << '\n';
  std::cout << test << '\n';
  std::cout << "Convolution result: " << convolution(test,kernel,3,3) << '\n'; //must be 14 (5+9)
}

void testDoG(Mat & img){
  Mat imgdouble;
  img.convertTo(imgdouble,CV_32F,1/255.0);
  Mat dog = DoG(imgdouble,5.0,3.0);
  imshow("DoG",dog);
  // std::cout << dog << '\n';
  Mat test = zeroCrossingMat(dog);
  imshow("passage par zero dog, threshold 0 percent", test);
  imwrite("0percentDog.jpg",test);
  double min, max;
  minMaxLoc(dog,&min,&max);
  float threshold = max*4/100;
  Mat testThreshold = zeroCrossingMat(dog,threshold);
  imshow("passage par zero dog, threshold 4 percent", testThreshold);
  imwrite("4percentDog.jpg",testThreshold);
}

void testTensor(Mat & img){
  Mat dx, dy;
  Mat doubleImg;
  Mat gradientVector, normalVector;
  Mat isophote(img.rows, img.cols, CV_32F);
  Mat normal(img.rows, img.cols, CV_32F);
  Mat coherence(img.rows, img.cols, CV_32F);
  Mat dpx(img.rows, img.cols, CV_32F);
  Mat* smoothedTensor = new Mat[3];
  Mat* tensorArray;
  Mat tensor(2,2,CV_32F);
  Mat u8Normal, u8Isophote, u8Coherence, u8DpX;
  double min, max;

  img.convertTo(doubleImg,CV_32F,1/255.0);
  Sobel(doubleImg,dx, CV_32F, 1, 0, 1);
  Sobel(doubleImg,dy, CV_32F, 0, 1, 1);

  tensorArray = tensorStructureArray(dx,dy);

  GaussianBlur(tensorArray[0],smoothedTensor[0],Size(0,0),1);
  GaussianBlur(tensorArray[1],smoothedTensor[1],Size(0,0),1);
  GaussianBlur(tensorArray[2],smoothedTensor[2],Size(0,0),1);

  for (size_t i = 0; i < img.rows; i++) {
    for (size_t j = 0; j < img.cols; j++) {
      tensor.at<float>(0,0) = smoothedTensor[0].at<float>(i,j);
      tensor.at<float>(0,1) = smoothedTensor[2].at<float>(i,j);
      tensor.at<float>(1,0) = smoothedTensor[2].at<float>(i,j);
      tensor.at<float>(1,1) = smoothedTensor[1].at<float>(i,j);
      eigen(tensor,&gradientVector,&normalVector,&isophote.at<float>(i,j),&normal.at<float>(i,j));
      coherence.at<float>(i,j) = coherenceNorm(isophote.at<float>(i,j),normal.at<float>(i,j));
      dpx.at<float>(i,j) = dpX(isophote.at<float>(i,j),normal.at<float>(i,j),0.1,0.0001);
    }
  }
  minMaxLoc(normal,&min,&max);
  std::cout << "Normal:" << '\n';
  std::cout << "min: " << min << " max: " << max << '\n';
  normal.convertTo(u8Normal,CV_8UC1,255.0/max);
  minMaxLoc(isophote,&min,&max);
  std::cout << "Isophote" << '\n';
  std::cout << "min: " << min << " max: " << max << '\n';
  isophote.convertTo(u8Isophote,CV_8UC1,255.0/max);
  minMaxLoc(isophote,&min,&max);
  std::cout << "Coherence Norm:" << '\n';
  minMaxLoc(coherence,&min,&max);
  std::cout << "min: " << min << " max: " << max << '\n';
  coherence.convertTo(u8Coherence,CV_8UC1,255.0/max);
  std::cout << "DpX Norm:" << '\n';
  minMaxLoc(dpx,&min,&max);
  std::cout << "min: " << min << " max: " << max << '\n';
  dpx.convertTo(u8DpX,CV_8UC1,255.0/max);

  namedWindow("normale", WINDOW_NORMAL);
  imshow("normale",u8Normal);
  namedWindow("isophote", WINDOW_NORMAL);
  imshow("isophote",u8Isophote);
  namedWindow("Coherence", WINDOW_NORMAL);
  imshow("Coherence",coherence);
  namedWindow("dpx", WINDOW_NORMAL);
  imshow("dpx",u8DpX);

  //threshold
  Mat imgToShow = thresholdUpperValues(dpx,0.7);
  namedWindow("Norm thresholded", WINDOW_NORMAL);
  imshow("Norm thresholded", imgToShow);

  Mat imgModified = img.clone();
  for (size_t i = 0; i < img.rows; i++) {
    for (size_t j = 0; j < img.cols; j++) {
      if(imgToShow.at<uchar>(i,j)==255){
        imgModified.at<uchar>(i,j) = 0;
      }
    }
  }

  namedWindow("img modif", WINDOW_NORMAL);
  imshow("img modif", imgModified);

  waitKey();
}



int main(int argc, char const *argv[]) {
  Mat im;
  im = imread("../ressources/highDefinitionUnsplash/spacex-549328-unsplash.jpg",CV_LOAD_IMAGE_COLOR);
  namedWindow("test picture", WINDOW_NORMAL);
  imshow("test picture",im);
  waitKey();
  Mat imGrayScale;
  cvtColor(im,imGrayScale,COLOR_BGR2GRAY);
  // Mat sGradient;
  // sGradient = getSobelGradient(imGrayScale);
  // imshow("gradient", sGradient);
  // testDoG(imGrayScale);
  //calculer les passages par 0 du DoG
  testTensor(imGrayScale);
  return 0;
}
