#ifndef ETF_H
#define ETF_H

#include <cmath>

/**
The magnitude weight function. The result range between 0 and 1.
Bigger weight are given to pixel whose gradient magnitude are higher
than whose of origin pixel.
*/
float magnitudeWeight(float originGradMagn, float currentGradMagn){
  return (currentGradMagn-originGradMagn + 1.0) / 2.0;
}

/**
Compute the edge tangent flow for a given pixel.
the etf for a given pixel respect this formula
for all the pixel is the radius (sum),
compute his spacial weight*directionweight*magnitudeWeight
*/
Point2d edgeTangentFlow(const Mat & isophote, const Mat & gHat, const int oRow, const int oColumn, const int radius = 5) {
  int cRow, cColumn;
  float oGradMag, cGradMag, wMagnitude, wDirectional, vectorNormalizingTerm;
  Point2d oTan(isophote.at<float>(oRow, oColumn * 2 + 0), isophote.at<float>(oRow, oColumn * 2 + 1));
  Point2d etf(0.0, 0.0);

  oGradMag = gHat.at<float>(oRow, oColumn);
  cGradMag = 0.0;
  vectorNormalizingTerm = 0.0;

  //for a given pixel we compute the etf inside a given radius
  for (int y = -radius; y <= radius; y++) {
    for (int x = -radius; x <= radius; x++) {
      cRow = oRow + y;
      cColumn = oColumn + x;
      // if we are not in the given radius, no need for computation
      if (cRow > -1 && cRow < gHat.rows && cColumn > -1 && cColumn < gHat.cols) {
        // if we respect the radius (pythagore) a²+b² = c²
        // spacial weight function, 0 if not inside the circle
        if ( (x*x+y*y) <= radius*radius ) {

          cGradMag = gHat.at<float>(cRow, cColumn);
          Point2d cTan(isophote.at<float>(cRow, cColumn * 2 + 0),
                       isophote.at<float>(cRow, cColumn * 2 + 1));

          wMagnitude = magnitudeWeight(oGradMag, cGradMag);
          // compute directional weight, a bigger weight is given if vector as the same orientation
          wDirectional = oTan.dot(cTan);

          etf += cTan * wMagnitude * wDirectional;
          vectorNormalizingTerm += wMagnitude * wDirectional;
        }
      }
    }
  }
  // normalize etf
  etf = (vectorNormalizingTerm != 0.0) ? etf / vectorNormalizingTerm : Point2d(0.0, 0.0);

  return etf;
}

/**
Compute an ETF (Edge tangent Flow) field of vector.
Computing the ETF is in different step:
- For each pixel compute the edge tangent flow using an given isophote
  This part is detailled above the edgeTangentFlow() function
- Before iterating once again, normalize the finded etf and use it as isophote in the next iteration
This function is as light as possible to provide a freedom of parameters
@param iVectMap, a map of vector (CV_32FC2) containing the isophote (orthogonal to the gradient)
@param gHat, the magnitude of the gradient (CV_32FC1)
@param radius, the radius for the spacial weight
@param iteration, the number of iteration to smooth the etf
*/
Mat computeETF(const Mat & iVectMap, const Mat & gHat, const int radius = 5, int iteration = 3) {
  //temp will contains the value between two iterations
  Mat temp(gHat.rows, gHat.cols, CV_32FC2);
  Point2d etfPoint;
  // we use etf in our computation. Here etf is the isophote (the edges)
  Mat etf = iVectMap.clone();

  // we get a smoother etf after each iteration
  while (iteration--) {
    for (int y = 0; y < gHat.rows; y++) {
      for (int x = 0; x < gHat.cols; x++) {
        // compute the etf for a single pixel
        etfPoint = edgeTangentFlow(etf, gHat, y, x, radius);
        temp.at<float>(y, x * 2 + 0) = etfPoint.x;
        temp.at<float>(y, x * 2 + 1) = etfPoint.y;
      }
    }
    temp.convertTo(etf, CV_32FC2);
    //normalize etf
    for (int y = 0; y < gHat.rows; y++) {
      for (int x = 0; x < gHat.cols; x++) {
        float vx = etf.at<float>(y, x*2 + 0);
        float vy = etf.at<float>(y, x*2 + 1);
        // compute the magnitude to normalize. A 10e-8 is added to avoid div by zero
        float mag = sqrt(vx*vx + vy*vy) + 1.0e-8;

        etf.at<float>(y, x * 2 + 0) = static_cast<float>(vx / mag);
        etf.at<float>(y, x * 2 + 1) = static_cast<float>(vy / mag);;
      }
    }
  }

  return etf;
}

#endif
