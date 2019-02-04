#include <iostream>
#include <gradient.h>
#include <opencv2/highgui.hpp>
#include <utils.h>
#include <tensor.h>
#include <dog.h>
#include <opencv2/core/mat.hpp>
#include <etf.h>
#include <lime-master/sources/lime.hpp>
#include <fdog.h>

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
  Point2d gradientVector, normalVector;
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

void testETF(Mat & grayImg){
  Mat floatImg;
  Mat gradX, gradY;
  Mat gVectMap = Mat::zeros(grayImg.rows, grayImg.cols, CV_32FC2);
  Mat isoVectMap = Mat::zeros(grayImg.rows, grayImg.cols, CV_32FC2);
  Mat gHat = cv::Mat::zeros(grayImg.rows, grayImg.cols, CV_32FC1);

  grayImg.convertTo(floatImg,CV_32F,1/255.0);

  Sobel(floatImg, gradX, CV_32F, 1, 0);
  Sobel(floatImg, gradY, CV_32F, 0, 1);

  for (int y = 0; y < grayImg.rows; y++) {
    for (int x = 0; x < grayImg.cols; x++) {
      float gx = gradX.at<float>(y, x);
      float gy = gradY.at<float>(y, x);

      float mag = sqrt(gx*gx + gy* gy);

      gHat.at<float>(y, x) = mag;

      isoVectMap.at<float>(y, x * 2 + 0) = static_cast<float>(-gy / (mag + 1.0e-8));
      isoVectMap.at<float>(y, x * 2 + 1) = static_cast<float>(gx / (mag + 1.0e-8));
    }
  }

  Mat etf = computeETF(isoVectMap, gHat, 5, 3);
  // Mat lic, noise;
  // lime::randomNoise(noise, cv::Size(grayImg.cols, grayImg.rows));
  // lime::LIC(noise, lic, etf, 20, lime::LIC_EULERIAN);
  // namedWindow("LIC homeMade", WINDOW_NORMAL);
  // imshow("LIC homeMade", lic);
  Mat fdog = fDoG(floatImg, etf, 3.0, 1.0, 0.99, 0.5, 2);

  Mat BnW;
  double min, max;
  minMaxLoc(fdog, &min, &max);
  printf("%f, %f\n", max, 255.0/max);
  fdog.convertTo(BnW, CV_8UC1, 255.0/max);
  namedWindow("fdog", WINDOW_NORMAL);
  imshow("fdog", BnW);
  imwrite("testFDOG.png",BnW);
  waitKey();

  // Mat lic2, etf2;
  // lime::calcETF(floatImg, etf2);
  // lime::LIC(noise, lic2, etf2, 20, lime::LIC_EULERIAN);
  // namedWindow("LIC lime", WINDOW_NORMAL);
  // imshow("LIC lime", lic);
  // waitKey();

  // Mat edges, cEdges;
  // lime::edgeFDoG(floatImg, edges, etf, lime::DoGParams(2.5, 3, 0.99, 15.0, lime::NPR_EDGE_FDOG));
  // namedWindow("edges", WINDOW_NORMAL);
  //
  // edges.convertTo(cEdges, CV_8UC1, 255);
  // imshow("edges", cEdges);
  // waitKey();

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
  testETF(imGrayScale);
  return 0;
}
