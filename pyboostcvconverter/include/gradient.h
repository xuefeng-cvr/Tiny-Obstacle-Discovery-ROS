#ifndef GRADIENT_HPP
#define GRADIENT_HPP

#include <iostream>
#include <opencv2/opencv.hpp>
#include "MathUtils.hpp"
#include "gradientMex.h"
#include "ConvTri.h"

using namespace std;
using namespace cv;

tuple<Mat, Mat> gradientMag(Mat I, int channel, bool full = 0);

Mat gradientHist(Mat M, Mat O, int bin, int nOrients, bool softbin);

void gradientMagNorm(Mat M, double normRad, float normConst);
#endif