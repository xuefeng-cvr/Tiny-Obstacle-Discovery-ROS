#include <iostream>
#include <opencv2/opencv.hpp>
#include <memory.h>
#include "model.h"
#include <vector>
#include "MathUtils.hpp"
#include "param.h"
#include "ConvTri.h"
#include "rgbConvertMex.hpp"


tuple<Mat, Mat> spDetect(Mat I, Mat E, _para_sp opt);