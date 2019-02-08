#ifndef FBL_H
#define FBL_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <cmath> //for math functions
#include <utils.h>

float similarityWeight(Point3f oValue, Point3f cValue, float sigma) {
  float norm = sqrt(pow((oValue.x-cValue.x),2)+pow((oValue.y-cValue.y),2)+pow((oValue.z-cValue.z),2));
  return gaussianValue(norm, sigma);
}

Mat Quantification(const Mat & src, int k ){
  Mat quantif(src.rows,src.cols,src.type());
  float pas(255./k);
  for (int i= 0; i < (src.rows); i++) {
    for (int j = 0; j < (src.cols); j++) {
      quantif.at<Vec3b>(i,j)[0] = round((float)src.at<Vec3b>(i,j)[0]/pas)*pas;
      quantif.at<Vec3b>(i,j)[1] = round((float)src.at<Vec3b>(i,j)[1]/pas)*pas;
      quantif.at<Vec3b>(i,j)[2] = round((float)src.at<Vec3b>(i,j)[2]/pas)*pas;
      //std::cout<<quantif.at<uchar>(i,j)<<"\t";
    }
  }
  return quantif;
}

//img need to be double
Mat computeFBL(const Mat & img, const Mat & isophote, const float sigmaE, const float sigmaG, const float sigmaRE, const float sigmaRG, const int maxIter = 4) {
  int nbChannels = 1;
  int iteration = maxIter;
  int sRadius = floor(kernelFromStandardDev(sigmaE));
  int tRadius = floor(kernelFromStandardDev(sigmaG));
  int tx, ty, cx, cy;
  float weight, tempVal;
  Mat lab, temp;
  Mat sKernelValues(1, sRadius*2+1, CV_32FC1);
  Mat tKernelValues(1, tRadius*2+1, CV_32FC1);
  Point2f gradient, tangent, lxct;
  Point3f cVal, oVal;
  Point3f cG, cE;
  int stepSize = 1;

  for(int i = -sRadius; i <= sRadius; i++){
    sKernelValues.at<float>(0, i+sRadius) = gaussianValue(i, sigmaE);
  }
  for(int i = -tRadius; i <= tRadius; i++){
    tKernelValues.at<float>(0, i+tRadius) = gaussianValue(i, sigmaG);
  }
  //TODO vérifier que ç fonctionne pour les images converties en float
  cvtColor(img, lab, COLOR_RGB2Lab);
  nbChannels = lab.channels();
  //prendre les canaux A et B
  temp = lab.clone();

  while (iteration) {
    // apply bilateral filters iterativel

    std::cout << "FBL pass n°" << maxIter-iteration+1 << '\n';

    printf("Computing bilateral in gradient way\n");
    for (int y = 0; y < img.rows; y++) {
      for (int x = 0; x < img.cols; x++) {
        //Calculer pour chaque pixel dans le sens de l'isophote, la seconde partie du filtre bilatéral
        weight = 0.0;
        cG = Point3f(0.0, 0.0, 0.0);
        // get orthogonal to isophote == gradient
        gradient = Point2f(-isophote.at<float>(y, x * 2 + 1), isophote.at<float>(y, x * 2 + 0));
        oVal = Point3f(lab.at<float>(y, x * nbChannels + 0), lab.at<float>(y, x * nbChannels + 1), lab.at<float>(y, x * nbChannels + 2));
        for (int t = -tRadius; t <= tRadius; t++){
          ty = floor(y + t * gradient.y + 0.5);
          tx = floor(x + t * gradient.x + 0.5);

          if( ty > -1 && ty < lab.rows && tx > -1 && tx < lab.cols){
            cVal = Point3f(lab.at<float>(y, x * nbChannels + 0), lab.at<float>(ty, tx * nbChannels + 1), lab.at<float>(ty, tx * nbChannels + 2));

            tempVal = tKernelValues.at<float>(0, t + tRadius) * similarityWeight(oVal, cVal, sigmaRG);
            weight += tempVal;
            cG.x += lab.at<float>(ty, tx * nbChannels + 0) * tempVal; // a Channel
            cG.y += lab.at<float>(ty, tx * nbChannels + 1) * tempVal; // b Channel
            cG.z += lab.at<float>(ty, tx * nbChannels + 2) * tempVal; // b Channel
          }
        }
        // Normaliser les valeurs pour chaque pixle
        temp.at<float>(y,x * nbChannels + 0) = cG.x / weight;
        temp.at<float>(y,x * nbChannels + 1) = cG.y / weight;
        temp.at<float>(y,x * nbChannels + 2) = cG.z / weight;
      }
    }
    for (int y = 0; y < temp.rows; y++) {
      for (int x = 0; x < temp.cols; x++) {
        for (int i = 0; i < nbChannels; i++) {
          lab.at<float>(y, x * nbChannels + i) = temp.at<float>(y, x * nbChannels + i);
        }
      }
    }
    printf("Done\n");
    // first isophote way
    printf("Computing bilateral in isophote way\n");
    for (int y = 0; y < img.rows; y++) {
      for (int x = 0; x < img.cols; x++) {
        // compute value at origin
        // here we do not need to use similarityWeight because the norm equal 0
        weight = sKernelValues.at<float>(0,sRadius)*gaussianValue(0,sigmaRE);
        cE.x = weight * lab.at<float>(y, x * nbChannels + 0);
        cE.y = weight * lab.at<float>(y, x * nbChannels + 1);
        cE.z = weight * lab.at<float>(y, x * nbChannels + 2);
        //take origin color values
        oVal = Point3f(lab.at<float>(y, x * nbChannels + 0), lab.at<float>(y, x * nbChannels + 1), lab.at<float>(y,x * nbChannels + 2));

        for (int sign = -1; sign <= 1; sign += 2) {
          //take tangent form origin
          tangent = Point2f(isophote.at<float>(y, x * 2 + 0), isophote.at<float>(y, x * 2 + 1));
          // take next point
          cy = y + floor(sign * stepSize * tangent.y + 0.5);
          cx = x + floor(sign * stepSize * tangent.x + 0.5);

          for (int c = 1; c <= sRadius; c++){
            if (cy > -1 && cy < isophote.rows && cx > -1 && cx < isophote.cols) {
              cVal = Point3f(lab.at<float>(cy, cx * nbChannels + 0), lab.at<float>(cy, cx * nbChannels + 1), lab.at<float>(cy, cx * nbChannels + 2));
              tempVal = similarityWeight(oVal, cVal, sigmaRE) * sKernelValues.at<float>(0, c+sRadius);
              weight += tempVal;
              cE.x += lab.at<float>(y,x * nbChannels + 0) * tempVal;
              cE.y += lab.at<float>(y,x * nbChannels + 1) * tempVal;
              cE.z += lab.at<float>(y,x * nbChannels + 2) * tempVal;
              // find next point to visit
              tangent = Point2f(isophote.at<float>(cy, cx * 2 + 0), isophote.at<float>(cy, cx * 2 + 1));
              cy = cy+floor(sign*stepSize*tangent.y + 0.5);
              cx = cx+floor(sign*stepSize*tangent.x + 0.5);
            }
            else
              break;
          }
        }
        temp.at<float>(y,x * nbChannels + 0) = cE.x / weight;
        temp.at<float>(y,x * nbChannels + 1) = cE.y / weight;
        temp.at<float>(y,x * nbChannels + 2) = cE.z / weight;
      }
    }
    printf("Done\n");
    for (int y = 0; y < temp.rows; y++) {
      for (int x = 0; x < temp.cols; x++) {
        for (int i = 0; i < nbChannels; i++) {
          lab.at<float>(y, x * nbChannels + i) = temp.at<float>(y, x * nbChannels + i);
        }
      }
    }

    iteration--;
  }
  Mat color;
  cvtColor(lab, color, COLOR_Lab2RGB);
  double min, max;
  minMaxLoc(color, &min, &max);
  color.convertTo(color, CV_8UC3, 255/max);
  color = Quantification(color, 20);
  imshow("abstraction", color);
  waitKey();

  // quantif de l'image pour avoir des "région flattening" avec quantif de la luminance
  return color;
}

#endif
