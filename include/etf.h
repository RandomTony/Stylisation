#ifndef ETF_H
#define ETF_H

#include <cmath>

float magnitudeWeight(float orginGradMagn, float currentGradMagn){
  return (originGradMagn-currentGradMagn + 1.0)/2.0;
}

float directionWeight(Point2d originTangent, Point2d currentTangent){
  return originTangent.dot(currentTangent);
}

Point2d edgeTangentFlow(const Mat & gradient, const Mat & isophote, const Mat & gAmplitude, const Mat & iAmplitude, const int originR, const int originC, const int radius = 5) {
  int currentR, currentC;
  float oGradMag = gAmplitude.at<float>(originR, originC);
  Point2d oTan(isophote.at<float>(originR, originC * 2 + 0), isophote.at<float>(originR, originC * 2 + 1));
  Point2d etfVector(0.0,0.0);
  float vectorNormalizingTerm = 0.0;
  float etf;

  for (size_t i = -radius; i <= radius; i++) {
    for (size_t j = -radius; j <= radius; j++) {
      currentR = originR + i;
      currentC = originC + j;
      //border verification
      if (currentR > -1 && currentR < img.rows && currentC > -1 && currentC < img.cols) {
        //if we respect the radius (pythagore) a²+b² = c²
        if ( (i*i+j*j) < radius*radius ) { //same as if abs(i-j)<radius, here we just use pytagore
          //compute ETF
          float cGradMag = gAmplitude.at<float>(currentR, currentC);
          Point2d cTan(isophote.at<float>(currentR, currentC * 2 + 0), isophote.at<float>(currentR, currentC * 2 + 1));
          etf = magnitudeWeight(oGradMag, cGradMag)
                *directionWeight(oTan, cTan);

          etfVector += cTan * etf;
          vectorNormalizingTerm += etf;
        }
      }
    }
  }

  //normalize etf
  etfVector /= vectorNormalizingTerm;

  return etfVector
}

Mat computeETF(const Mat & gradient, const Mat & isophote, const Mat & gAmplitude, const Mat & iAmplitude, const int radius = 5, int iteration = 3) {
  Mat temp(gAmplitude.height, gAmplitude.width, CV_32FC2); //picture of vectors
  Point2d etfPoint;

  Mat etf(isophote);

  while (iteration--) {
    for (int i = 0; i < gAmplitude.height; i++) {
      for (int j = 0; j < gAmplitude.weight; j++) {
        etfPoint = edgeTangentFlow(gradient, etf, gAmplitude, iAmplitude, i, j, radius);
        temp.at<float>(i, j * 2 + 0) = etfPoint.x;
        temp.at<float>(i, j * 2 + 1) = etfPoint.y;
      }
    }
    temp.convertTo(etf, CV_32FC2);
    //normalize etf
    for (int i = 0; i < gAmplitude.height; i++) {
      for (int j = 0; j < gAmplitude.weight; j++) {
        double vx = etf.at<float>(i, j*2 + 0);
        double vy = etf.at<float>(i, j*2 + 1);
        double mag = sqrt(vx*vx + vy*vy);

        etf.at<float>(i, j*2 + 0) /= mag;
        etf.at<float>(i, j*2 + 1) /= mag;
      }
    }
  }

  return etf;
}

#endif
