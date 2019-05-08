#include "fdog.hpp"
#include <opencv2/core/mat.hpp>
#include <cmath> //for math functions
#include "utils.hpp"

Mat superImposing(const Mat & grayImg, const Mat & fDoG, float sigma){
  Mat sI = grayImg.clone();
  Mat out;

  for(int y = 0; y < grayImg.rows; y++){
    for(int x = 0; x < grayImg.cols; x++){
      if(fDoG.at<float>(y,x) == 0.0)
        sI.at<float>(y,x) = 0.0;
    }
  }
  GaussianBlur(sI,out,Size(0,0),sigma);
  return out;
}

float lineDoG(const Mat & grayImg, const Mat & isophote, Point2f & lxsT, Mat & tKernelValues){
  int tRadius = floor(tKernelValues.cols / 2);
  // get the normal to isophote = gradient. Just take orthogonal vector. Already normalized
  Point2f gradient(-isophote.at<float>(lxsT.y, lxsT.x * 2 + 1), isophote.at<float>(lxsT.y, lxsT.x * 2 + 0));
  float result = 0.0;
  int tx, ty;

  // along this normal and from -tRadius to tRadius
  for(int t = -tRadius; t <= tRadius; t++){
    // technique d'arondi avec le + 0.5 et le floor
    ty = floor(lxsT.y + t * gradient.y + 0.5);
    tx = floor(lxsT.x + t * gradient.x + 0.5);

    if( ty > -1 && ty < grayImg.rows && tx > -1 && tx < grayImg.cols){
      result += tKernelValues.at<float>(0,t+tRadius)*grayImg.at<float>(ty, tx);
    }
  }

  return result;
}

Mat fDoG(const Mat & img, const Mat & isophote, const float sigmaM, const float sigmaC, const float rho, const float threshold, int maxIter){
  int cx, cy;
  int iteration = maxIter;
  const float stepSize = 1.0;
  float hX = 0.0;
  Point2f lxct, tangent;
  Mat out = Mat::zeros(img.rows, img.cols, CV_32FC1);
  Mat dog = Mat::zeros(img.rows, img.cols, CV_32FC1);
  Mat grayImg = img.clone();

  // according to DoG difference btw sigma
  const float sigmaS = sigmaC * 1.6;
  // get sRadius and tRadius from sigma
  int sRadius = floor(kernelFromStandardDev(sigmaM));
  int tRadius = floor(kernelFromStandardDev(sigmaS));

  /* Because the kernel value depends of spacial distance, we can initialize them when we know the kernel size */
  Mat sKernelValues(1, sRadius*2+1, CV_32FC1);
  Mat tKernelValues(1, tRadius*2+1, CV_32FC1);
  for(int i = -sRadius; i <= sRadius; i++){
    sKernelValues.at<float>(0, i+sRadius) = gaussianValue(i, sigmaM);
  }
  for(int i = -tRadius; i <= tRadius; i++){
    tKernelValues.at<float>(0, i+tRadius) = gaussianValue(i, sigmaC) - rho*gaussianValue(i, sigmaS);
  }

  while (iteration) {
    std::cout << "fDoG: " << "pass n°" << (maxIter-iteration)+1 << "/" << maxIter << std::endl;

    // avoid superImposing if we haven't yet the fDoG
    if(iteration != maxIter){
      // superImposing the grayscale with edges found and use it again
      grayImg = superImposing(grayImg, out);
    }


    std::cout << "Compute DoG along gradient foreach pixel..." << std::flush;
    // first pass, compute DoG at each pixel in the gradient
    for (int y = 0; y < grayImg.rows; y++) {
      for (int x = 0; x < grayImg.cols; x++) {
        lxct = Point2f(x, y);
        dog.at<float>(y,x) = lineDoG(grayImg, isophote, lxct, tKernelValues);
      }
    }
    std::cout << "Done" << std::endl;
    std::cout << "Sum DoG along isophote curve..." << std::flush;
    // compute fDoG, use the DoG along the line found for each pixel
    for (int y = 0; y < grayImg.rows; y++) {
      for (int x = 0; x < grayImg.cols; x++) {
        // Compute value at init point
        hX = sKernelValues.at<float>(0, sRadius) * dog.at<float>(y,x);
        // take current point
        lxct = Point2f(x, y);
        for (float sign = -1; sign <= 1; sign+=2) { // Go in one way on the curve and on the other way with sign value
          // take tangent at the init point (say X)
          tangent = Point2f(isophote.at<float>(y, x * 2 + 0), isophote.at<float>(y, x * 2 + 1));
          // round the value of the next pixel to visit. Just use the tangent to know where to go
          cy = y + floor(sign * stepSize * tangent.y + 0.5);
          cx = x + floor(sign * stepSize * tangent.x + 0.5);
          // along the curve
          for (int c = 1; c <= sRadius; c++){
            if (cy > -1 && cy < isophote.rows && cx > -1 && cx < isophote.cols) {
              lxct = Point2f(cx, cy);
              // sum values along the curve
              hX += sKernelValues.at<float>(0,c+sRadius) * dog.at<float>(cy,cx);
              // find next point to visit
              tangent = Point2f(isophote.at<float>(cy, cx * 2 + 0), isophote.at<float>(cy, cx * 2 + 1));
              cy = cy+floor(sign*stepSize*tangent.y + 0.5);
              cx = cx+floor(sign*stepSize*tangent.x + 0.5);
            }
            else
              break;
          }
        }
        // threshold the value
        if(hX >= 0.0 || 1+tanh(hX) < threshold){
          out.at<float>(y, x) = 1.0;
        } else {
          out.at<float>(y, x) = 0.0;
        }
      }
    }
    std::cout << "Done" << std::endl;

    iteration--;
  }
  return out;
}
