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
  imwrite("0percentDog.jpg",test);
  double min, max;
  minMaxLoc(dog,&min,&max);
  float threshold = max*4/100;
  Mat testThreshold = zeroCrossingMat(dog,threshold);
  imshow("passage par zero dog, threshold 4 percent", testThreshold);
  imwrite("4percentDog.jpg",testThreshold);
}

void testTensor2(Mat & img){
  Mat dx, dy;
  Mat doubleImg;

  img.convertTo(doubleImg,CV_32F,1/255.0);
  Sobel(doubleImg,dx, CV_32F, 1, 0, 1);
  Sobel(doubleImg,dy, CV_32F, 0, 1, 1);
  Mat gradientVector, normalVector;
  Mat imgToShow(img.rows, img.cols, CV_32F);

  Mat* tensorArray = tensorStructureArray(dx,dy);
  Mat isophote(img.rows, img.cols, CV_32F);
  Mat normal(img.rows, img.cols, CV_32F);
  Mat* smoothedTensor = new Mat[3];

  GaussianBlur(tensorArray[0],smoothedTensor[0],Size(0,0),3);
  GaussianBlur(tensorArray[1],smoothedTensor[1],Size(0,0),3);
  GaussianBlur(tensorArray[2],smoothedTensor[2],Size(0,0),3);

  Mat tensor(2,2,CV_32F);

  for (size_t i = 0; i < img.rows; i++) {
    for (size_t j = 0; j < img.cols; j++) {
      tensor.at<float>(0,0) = smoothedTensor[0].at<float>(i,j);
      tensor.at<float>(0,1) = smoothedTensor[2].at<float>(i,j);
      tensor.at<float>(1,0) = smoothedTensor[2].at<float>(i,j);
      tensor.at<float>(1,1) = smoothedTensor[1].at<float>(i,j);
      eigen(tensor,&gradientVector,&normalVector,&isophote.at<float>(i,j),&normal.at<float>(i,j));
    }
  }
  Mat u8Normal, u8Isophote;
  double min, max;
  minMaxLoc(normal,&min,&max);
  std::cout << "min: " << min << " max: " << max << '\n';
  normal.convertTo(u8Normal,CV_8UC1,255.0/max);
  minMaxLoc(isophote,&min,&max);
  std::cout << "min: " << min << " max: " << max << '\n';
  isophote.convertTo(u8Isophote,CV_8UC1,255.0/max);

  namedWindow("normale", WINDOW_NORMAL);
  imshow("normale",u8Normal);
  namedWindow("isophote", WINDOW_NORMAL);
  imshow("isophote",u8Isophote);

  waitKey();
}

void testTensor(Mat & img){
  Mat tensor, dx, dy;
  Mat doubleImg;
  img.convertTo(doubleImg,CV_32F,1/255.0);
  Sobel(doubleImg,dx, CV_32F, 1, 0, 1);
  Sobel(doubleImg,dy, CV_32F, 0, 1, 1);
  Mat gradient, normal;
  float gAmplitude, nAmplitude;
  Mat imgToShow(img.rows, img.cols, CV_32F);
  for (size_t i = 0; i < img.rows; i++) {
    for (size_t j = 0; j < img.cols; j++) {
      tensor = tensorStructure(dx,dy,i,j);
      eigen(tensor,&gradient,&normal,&gAmplitude,&nAmplitude);
      imgToShow.at<float>(i,j)=dpX(gAmplitude,nAmplitude,0.001,0.0001);
    }
  }
  namedWindow("normtest", WINDOW_NORMAL);
  double min, max;
  minMaxLoc(imgToShow,&min,&max);
  Mat imgTest;
  std::cout << "min: " << min << " max: " << max << '\n';
  imgToShow.convertTo(imgTest,CV_8UC1,255.0/max);
  imshow("normtest", imgTest);
  // if(imgToShow.cols>1920 || imgToShow.rows>1080){
  //   resizeWindow("normtest",1800,1000);
  // }
  // namedWindow("seuillage", WINDOW_NORMAL);
  // Mat imgThresholded(img.rows, img.cols, CV_32F);
  // for (size_t i = 0; i < imgToShow.rows; i++) {
  //   for (size_t j = 0; j < imgToShow.cols; j++) {
  //     if(imgToShow.at<float>(i,j)>(max*60/100)){
  //       imgThresholded.at<float>(i,j) = 0;
  //     }
  //     else{
  //       imgThresholded.at<float>(i,j) = imgToShow.at<float>(i,j);
  //     }
  //   }
  // }
  // imshow("seuillage",imgThresholded);
  waitKey();
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
  // testDoG(imGrayScale);
  //calculer les passages par 0 du DoG
  testTensor2(imGrayScale);
  return 0;
}
