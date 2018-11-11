#include <iostream>
#include <gradient.h>
#include <opencv2/highgui.hpp>
#include <utils.h>
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
  double min, max;
  minMaxLoc(dog,&min,&max);
  float threshold = max*4/100;
  Mat testThreshold = zeroCrossingMat(dog,threshold);
  imshow("passage par zero dog, threshold 4 percent", testThreshold);
}


int main(int argc, char const *argv[]) {
  Mat im;
  if(argc == 2)
    im = imread(argv[1],CV_LOAD_IMAGE_COLOR);
  else{
    std::cout << "Provide an picture path as an argument." << '\n';
    std::cout << "Usage: " << argv[0] << " <pathToPicture>" << '\n';
    return 1;
  }
  imshow("test picture",im);
  waitKey();
  Mat imGrayScale;
  cvtColor(im,imGrayScale,COLOR_BGR2GRAY);
  // Mat sGradient;
  // sGradient = getSobelGradient(imGrayScale);
  // imshow("gradient", sGradient);
  testDoG(imGrayScale);
  //calculer les passages par 0 du DoG
  waitKey();
  waitKey();
  testConvol();
  return 0;
}
