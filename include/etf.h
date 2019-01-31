#ifndef ETF_H
#define ETF_H

#include <cmath>

float magnitudeWeight(float originGradMagn, float currentGradMagn){
  return (originGradMagn-currentGradMagn + 1.0) / 2.0;
}

Point2d edgeTangentFlow(const Mat & isophote, const Mat & gHat, const int oRow, const int oColumn, const int radius = 5) {
  int cRow, cColumn;
  float oGradMag, cGradMag, wMagnitude, wDirectional, vectorNormalizingTerm;
  Point2d oTan(isophote.at<float>(oRow, oColumn * 2 + 0), isophote.at<float>(oRow, oColumn * 2 + 1));
  Point2d etf(0.0, 0.0);

  oGradMag = gHat.at<float>(oRow, oColumn);
  cGradMag = 0.0;
  vectorNormalizingTerm = 0.0;

  for (int y = -radius; y <= radius; y++) {
    for (int x = -radius; x <= radius; x++) {
      cRow = oRow + y;
      cColumn = oColumn + x;
      //border verification
      if (cRow > -1 && cRow < gHat.rows && cColumn > -1 && cColumn < gHat.cols) {
        //if we respect the radius (pythagore) a²+b² = c²
        if ( (x*x+y*y) <= radius*radius ) {
          cGradMag = gHat.at<float>(cRow, cColumn);
          Point2d cTan(isophote.at<float>(cRow, cColumn * 2 + 0), isophote.at<float>(cRow, cColumn * 2 + 1));

          wMagnitude = magnitudeWeight(oGradMag, cGradMag);
          wDirectional = oTan.dot(cTan);

          etf += cTan * wMagnitude * wDirectional;
          vectorNormalizingTerm += wMagnitude * wDirectional;
        }
      }
    }
  }
  //normalize etf
  etf = (vectorNormalizingTerm != 0.0) ? etf / vectorNormalizingTerm : Point2d(0.0, 0.0);

  return etf;
}

Mat computeETF(const Mat & gVectMap, const Mat & iVectMap, const Mat & gHat, const int radius = 5, int iteration = 3) {
  Mat temp(gHat.rows, gHat.cols, CV_32FC2); //picture of vectors
  Point2d etfPoint;
  Mat etf = iVectMap.clone();

  while (iteration--) {
    for (int y = 0; y < gHat.rows; y++) {
      for (int x = 0; x < gHat.cols; x++) {
        etfPoint = edgeTangentFlow(etf, gHat, y, x, radius);
        temp.at<float>(y, x * 2 + 0) = etfPoint.x;
        temp.at<float>(y, x * 2 + 1) = etfPoint.y;
      }
    }
    temp.convertTo(etf, CV_32FC2);
    //normalize etf
    for (int y = 0; y < gHat.rows; y++) {
      for (int x = 0; x < gHat.cols; x++) {
        double vx = etf.at<float>(y, x*2 + 0);
        double vy = etf.at<float>(y, x*2 + 1);
        double mag = sqrt(vx*vx + vy*vy) + 1.0e-8;

        etf.at<float>(y, x * 2 + 0) = static_cast<float>(vx / mag);
        etf.at<float>(y, x * 2 + 1) = static_cast<float>(vy / mag);;
      }
    }
  }

  return etf;
}

#endif
