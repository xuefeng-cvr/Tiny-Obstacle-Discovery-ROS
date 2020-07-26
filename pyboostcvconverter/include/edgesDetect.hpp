#include <iostream>
#include <opencv2/opencv.hpp>
#include <tuple>
#include "model.h"
#include "MathUtils.hpp"
#include "ConvTri.h"

tuple<Mat, Mat> edgesChns(Mat, _opt);
tuple<Mat, Mat> edgesDetect(Mat I, _model model, int nargout, uint32*& ind, uint8*& segs, uint32*& segDims);