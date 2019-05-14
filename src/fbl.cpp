#include "fbl.hpp"
#include <opencv2/core/mat.hpp>
#include <cmath> //for math functions
#include "utils.hpp"

float similarityWeight(const Vec3f& oValue, const Vec3f& cValue, float sigma) {
    float norm = sqrt(pow((oValue[0] - cValue[0]), 2) +
                      pow((oValue[1] - cValue[1]), 2) +
                      pow((oValue[2] - cValue[2]), 2));
    return gaussianValue(norm, sigma);
}

Mat computeFBL(const Mat & img, const Mat & isophote, const float sigmaE,
               const float sigmaG, const float sigmaRE, const float sigmaRG,
               const int maxIter)
{
    int iteration = maxIter;
    int sRadius = floor(kernelFromStandardDev(sigmaE));
    int tRadius = floor(kernelFromStandardDev(sigmaG));
    int tx, ty, cx, cy;
    float weight, tempVal;
    Mat rgb;
    Mat temp = img.clone();
    Mat sKernelValues(1, sRadius*2 + 1, CV_32FC1);
    Mat tKernelValues(1, tRadius*2 + 1, CV_32FC1);
    Point2f gradient, tangent;
    Vec3f cVal, oVal, cG, cE;
    int stepSize = 1;

    for(int i = -sRadius; i <= sRadius; i++)
        sKernelValues.at<float>(0, i+sRadius) = gaussianValue(i, sigmaE);
    for(int i = -tRadius; i <= tRadius; i++)
        tKernelValues.at<float>(0, i+tRadius) = gaussianValue(i, sigmaG);
    //TODO vérifier que ç fonctionne pour les images converties en float

    while (iteration) {
        // clone to compute based o
        rgb = temp.clone();
        // apply bilateral filters iterativel
        std::cout << "FBL pass n°" << maxIter-iteration+1 << "/" <<
                     maxIter << std::endl;
        // first isophote way
        std::cout << "Computing bilateral in isophote way..." << std::flush;
        // #pragma omp parallel for
        for (int y = 0; y < img.rows; y++) {
            for (int x = 0; x < img.cols; x++) {
                /* compute value at origin
                * here we do not need to use similarityWeight because
                * the norm equal 0*/
                weight = sKernelValues.at<float>(0,sRadius)*
                         gaussianValue(0,sigmaRE);
                //take origin color values
                oVal = rgb.at<Vec3f>(y, x);
                cE = weight * oVal;
                // sign is used to go in one way and in the other way
                for (int sign = -1; sign <= 1; sign += 2) {
                    //take tangent form origin
                    tangent = Point2f(isophote.at<float>(y, x * 2 + 0),
                                      isophote.at<float>(y, x * 2 + 1));
                    // take next pointn round the next step
                    cy = y + floor(sign * stepSize * tangent.y + 0.5);
                    cx = x + floor(sign * stepSize * tangent.x + 0.5);

                    for (int c = 1; c <= sRadius; c++) {
                        // border check
                        if (cy > -1 && cy < isophote.rows && cx > -1 &&
                            cx < isophote.cols)
                        {
                            cVal = rgb.at<Vec3f>(cy, cx);
                            tempVal = similarityWeight(oVal, cVal, sigmaRE) *
                                      sKernelValues.at<float>(0, c+sRadius);
                            weight += tempVal;

                            cE += rgb.at<Vec3f>(y,x) * tempVal;
                            // find next point to visit
                            tangent = Point2f(isophote.at<float>(cy, cx*2 + 0),
                                              isophote.at<float>(cy, cx*2 + 1));
                            cy = cy+floor(sign * stepSize * tangent.y + 0.5);
                            cx = cx+floor(sign * stepSize * tangent.x + 0.5);
                        }
                        else
                            break;
                    }
                }
                // normalize value
                temp.at<Vec3f>(y,x) = cE / weight;
            }
        }
        std::cout << "Done" << std::endl;
        // clone to use smoothed value for next compute
        rgb = temp.clone();

        std::cout << "Computing bilateral in gradient way..." << std::flush;
        // #pragma omp parallel for
        for (int y = 0; y < img.rows; y++) {
            for (int x = 0; x < img.cols; x++) {
                // Init weight used in normalisation and cG
                weight = 0.0;
                cG = Vec3f(0.0, 0.0, 0.0);
                // get orthogonal to isophote == gradient
                gradient = Point2f(-isophote.at<float>(y, x * 2 + 1),
                                   isophote.at<float>(y, x * 2 + 0));
                oVal = Vec3f(rgb.at<Vec3f>(y, x));
                for (int t = -tRadius; t <= tRadius; t++){
                    ty = floor(y + t * gradient.y + 0.5);
                    tx = floor(x + t * gradient.x + 0.5);

                    if( ty > -1 && ty < rgb.rows && tx > -1 && tx < rgb.cols){
                        cVal = Vec3f(rgb.at<Vec3f>(y, x));
                        tempVal = tKernelValues.at<float>(0, t + tRadius) *
                                  similarityWeight(oVal, cVal, sigmaRG);
                        weight += tempVal;
                        cG += rgb.at<Vec3f>(ty, tx) * tempVal;
                    }
                }
                // Normaliser les valeurs pour chaque pixle
                temp.at<Vec3f>(y, x) = cG / weight;
            }
        }
        std::cout << "Done" << std::endl;

        iteration--;
    }
    return temp;
}
