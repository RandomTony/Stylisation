#ifdef _MSC_VER
#pragma once
#endif

#ifndef _NPR_NPREDGES_DETAIL_H_
#define _NPR_NPREDGES_DETAIL_H_

#include <ctime>

#include "vector_field.h"
#include "../npr/lic.h"

namespace lime {

inline DoGParams::DoGParams(double _kappa, double _sigma, double _tau, double _phi,
                            NPREdgeType _edgeType)
    : kappa(_kappa)
    , sigma(_sigma)
    , tau(_tau)
    , phi(_phi)
    , edgeType(_edgeType) {
}

namespace {  // NOLINT

void uniformNoise(cv::OutputArray noise, cv::InputArray gray, int nNoise) {
    cv::Mat  img = gray.getMat();
    cv::Mat& out = noise.getMatRef();

    Assertion(img.channels() == 1 && img.depth() == CV_32F,
        "input image must be single channel and floating-point-valued");

    const int width  = img.cols;
    const int height = img.rows;

    Random rand((unsigned int)time(0));

    out = cv::Mat::zeros(height, width, CV_32FC1);
    int count = 0;
    while (count < nNoise) {
        int x = rand.nextInt(width);
        int y = rand.nextInt(height);
        if (rand.nextReal() < img.at<float>(y, x)) continue;
        out.at<float>(y, x) = 1.0f;
        count++;
    }
}

void edgeXDoG(cv::InputArray input, cv::OutputArray output, const DoGParams& param) {
    cv::Mat  gray = input.getMat();
    cv::Mat& edge = output.getMatRef();

    const int width = gray.cols;
    const int height = gray.rows;
    const int dim = gray.channels();

    cv::Mat g1, g2;
    cv::GaussianBlur(gray, g1, cv::Size(0, 0), param.sigma);
    cv::GaussianBlur(gray, g2, cv::Size(0, 0), param.kappa * param.sigma);

    edge = cv::Mat(height, width, CV_32FC1);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double diff = g1.at<float>(y, x) - param.tau * g2.at<float>(y, x);
            if (diff > 0.0) {
                edge.at<float>(y, x) = 1.0f;
            } else {
                edge.at<float>(y, x) = static_cast<float>(1.0 + tanh(param.phi * diff));
            }
        }
    }
}

/*
input is the grayscale picture
vfield is the given vector field, is adjacent and opposite vector
kSize is the kernel size, 10
sigma_s, equal to three
sigma t, variable
*/
void gaussWithFlow(cv::InputArray input, cv::OutputArray output, const cv::Mat& vfield,
                  int ksize, double sigma_s, double sigma_t) {
    cv::Mat  image = input.getMat();
    cv::Mat& out = output.getMatRef();

    const int width = image.cols;
    const int height = image.rows;
    // 1
    const int dim = image.channels();

    //L equal to 15, rapport de 1.5 entre L et T
    const int L = static_cast<int>(ksize * 1.5);
    //1 chanel *h *w
    out = cv::Mat::zeros(height, width, CV_MAKETYPE(CV_32F, dim));

    cv::Mat temp = cv::Mat::zeros(height, width, CV_MAKETYPE(CV_32F, dim));

    // premier parcours de l'image
    ompfor(int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // get adjacent and opposite length
            double ty = vfield.at<float>(y, x * 2 + 0);
            double tx = vfield.at<float>(y, x * 2 + 1);
            double weight = 0.0;
            // sur la taille du noyau
            for (int t = -ksize; t <= ksize; t++) {
                // Go along the width, but why with t*ty and not t+ty
                int xx = static_cast<int>(x - 0.5 * tx * t);
                int yy = static_cast<int>(y + 0.5 * ty * t);
                // border verification
                if (xx >= 0 && yy >= 0 && xx < width && yy < height) {
                    // gaussian weight
                    double w = exp(-t * t / sigma_t);
                    for (int c = 0; c < dim; c++) {
                        temp.at<float>(y, x*dim + c) += static_cast<float>(w * image.at<float>(yy, xx*dim + c));
                    }
                    weight += w;
                }
            }

            // normalize it
            for (int c = 0; c < dim; c++) {
                temp.at<float>(y, x*dim + c) = temp.at<float>(y, x*dim + c) / static_cast<float>(weight);
            }
        }
    }

    ompfor(int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double weight = 0.0;
            // 1 dimention. Initialisation du pix courant à 0
            for (int c = 0; c < dim; c++) {
                out.at<float>(y, x*dim + c) = 0.0f;
            }

            // pour pm = -1 et pm = 1 (dans un sans et dans l'autre?)
            for (int pm = -1; pm <= 1; pm += 2) {
                // Initialisation à 0
                int l = 0;
                // take length of adjacent and opposite lenght of vector at current pixel (alogn isophote)
                double tx = pm * vfield.at<float>(y, x * 2 + 0);
                double ty = pm * vfield.at<float>(y, x * 2 + 1);
                Point2d pt = Point2d(x + 0.5, y + 0.5);
                // < 15
                while (++l < L) {
                    // take upperValue pt.x and pt.y, when l == 0, px and py are upper right values
                    int px = static_cast<int>(ceil(pt.x));
                    int py = static_cast<int>(ceil(pt.y));
                    if (px < 0 || py < 0 || px >= width || py >= height) {
                        break;
                    }
                    // take weight depending on sigma S
                    double w = exp(-l * l / sigma_s);
                    // along different chanels
                    for (int c = 0; c < dim; c++) {
                        // accumulate as we go along l. Farest point have a lighter weight
                        out.at<float>(y, x*dim + c) += static_cast<float>(w * temp.at<float>(py, px*dim + c));
                    }
                    weight += w;
                    // take tangent field for next point
                    double vx = vfield.at<float>(py, px * 2 + 0);
                    double vy = vfield.at<float>(py, px * 2 + 1);
                    if (vx == 0.0f && vy == 0.0f) {
                        break;
                    }

                    // inner product between old tangent and current tangent, and keep the same direction
                    double inner = vx * tx + vy * ty;
                    // aligned tangent
                    double sx = sign(inner) * vx;
                    double sy = sign(inner) * vy;

                    // take next point, just add the tangent
                    px = static_cast<int>(ceil(pt.x + 0.5 * sx));
                    py = static_cast<int>(ceil(pt.y + 0.5 * sy));
                    if (px < 0 || py < 0 || px >= width || py >= height) {
                        break;
                    }

                    // take next point tangent
                    vx = vfield.at<float>(py, px * 2 + 0);
                    vy = vfield.at<float>(py, px * 2 + 1);
                    // take the
                    inner = vx * tx + vy * ty;
                    tx = sign(inner) * vx;
                    ty = sign(inner) * vy;
                    pt.x += tx;
                    pt.y += ty;
                }
            }

            for (int c = 0; c < dim; c++) {
                if (weight != 0.0) {
                    out.at<float>(y, x*dim + c) = out.at<float>(y, x*dim + c) / static_cast<float>(weight);
                } else {
                    out.at<float>(y, x*dim + c) = temp.at<float>(y, x*dim + c);
                }
            }
        }
    }
}

// from demo_npr_filterscpp:
//  lime::edgeDoG(gray, fdog, lime::DoGParams(2.5, 0.5, 0.95, 10.0, lime::NPR_EDGE_FDOG));
// kappa = 2.5 sigma = 0.5, tau (rho) = 0.95, phi = 10
void edgeFDoG(cv::InputArray input, cv::OutputArray output, const cv::Mat& vfield, const DoGParams& param) {
  cv::Mat  gray = input.getMat();
  cv::Mat& edge = output.getMatRef();

  const int width = gray.cols;
  const int height = gray.rows;
  const int dim = gray.channels();

  if (vfield.empty()) {
    cv::Mat angles;
    // compute ETF with a K size of 11 and return angles between ty and tx
    calcVectorField(gray, angles, 11);
    // return a vfield with adjacent and opposite side *2
    angle2vector(angles, vfield, 2.0);
  }

  // kernel size of 10 px
  const int ksize = 10;

  const double alpha = 2.0;
  // param.sigma = 0.5
  const double sigma2 = 2.0 * param.sigma * param.sigma;
  // sigma2 = 2.0 * 0.5 * 0.5 = 2/4 = 1/2 = 0.5 ... useless
  cv::Mat g1;
  // sigma2 = 0.5
  gaussWithFlow(gray, g1, vfield, ksize, 3.0, sigma2);

  // ksigma = sigma * 2.5 = 1/2 * 5/2 = 5/4 = 1.25
  const double ksigma = param.kappa * param.sigma;

  // ksigma 2 = 2.0 * 5/4 * 5/4 = 25/8 = 3.125
  const double ksigma2 = 2.0 * ksigma * ksigma;
  cv::Mat g2;
  // ksigma equal 3.125
  gaussWithFlow(gray, g2, vfield, ksize, 3.0, ksigma2);

  edge = cv::Mat(height, width, CV_32FC1);
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      double diff = 0.0;
      for (int c = 0; c < dim; c++) {
        // DoG part
        diff += (g1.at<float>(y, x*dim + c) - param.tau * g2.at<float>(y, x*dim + c));
      }
      // maybe gaussian part
      diff /= 3.0;

      // threshold part, strange.
      if (diff > 0.0) {
        edge.at<float>(y, x) = 1.0f;
      } else {
        // here phi is
        edge.at<float>(y, x) = 1.0f + static_cast<float>(tanh(param.phi * diff));
      }
    }
  }
}
}  // unnamed namespace

void edgeDoG(cv::InputArray image, cv::OutputArray edge, const DoGParams& param) {
    cv::Mat input = image.getMat();
    Assertion(input.depth() == CV_32F && input.channels() == 1,
        "Input image must be single channel and floating-point-valued.");

    cv::Mat& outRef = edge.getMatRef();

    switch (param.edgeType) {
    case NPR_EDGE_XDOG:
        edgeXDoG(input, outRef, param);
        break;

    case NPR_EDGE_FDOG:
        edgeFDoG(input, outRef, cv::Mat(), param);
        break;

    default:
        Assertion(false, "Unknown DoG type is specified.");
    }
}

}  // namespace lime

#endif  // _NPR_NPREDGES_DETAIL_H_
