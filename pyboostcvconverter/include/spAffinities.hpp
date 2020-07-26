#include <iostream>
#include <opencv2/opencv.hpp>
#include "model.h"
#include <vector>
#include "MathUtils.hpp"
#include "param.h"
#include "bwlabel.h"
#include "ConvTri.h"
#include "rgbConvertMex.hpp"

Mat spDetectmain(string mode, Mat Smat, Mat Emat, bool add, uint nThreads); //boundaries
//Mat spDetectmain(string mode, Mat Smat, Mat Emat, Mat segs, int nThreads);  //affinities
Mat spDetectmain(string mode, Mat Smat, Mat Emat, uint8* Segs, uint32* segDims, int nThreads);  //affinities
Mat spDetectmain(string mode, Mat Smat, Mat Emat, float thr);               //merge
Mat spDetectmain(string mode, Mat Smat, Mat Imat, Mat Emat, double *prm);   //sticky
Mat spDetectmain(string mode, Mat Smat, Mat Imat, bool hasBnds);            //visualize
Mat spDetectmain(string mode, Mat Smat, Mat Amat);                          //edges
Mat ucm_mean_pb(Mat E, Mat S);
tuple<Mat, Mat, Mat> spAffinities(Mat S, Mat E, uint8* segs, uint32* segDims, int nThreads = 4);