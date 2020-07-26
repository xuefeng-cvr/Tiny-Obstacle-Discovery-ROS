#include <iostream>
#include <opencv2/opencv.hpp>
#include "model.h"
#include "MathUtils.hpp"

using namespace std;
using namespace cv;

_para initial_para();

tuple<Mat, Mat> edgesDetect(Mat I, _model model, int nargout, uint32*& ind, uint8*& segs, uint32*& segDims);
tuple<Mat, Mat> spDetect(Mat I, Mat E, _para_sp opt);
tuple<Mat, Mat, Mat> spAffinities(Mat S, Mat E, uint8* segs, uint32* segDims, int nThreads = 4);

Mat edgesNms(Mat, Mat, int, int, float, int);


Mat structure_edge(string picname, _model model, _para opt, _para_sp opt_sp)
{
    timeval t1_ed,t2_ed,t1_sp,t2_sp,t1_aff,t2_aff;
    long long tlength_ed,tlength_sp,tlength_aff;
    
    Mat I = imread(picname);
    assert(I.rows != 0 && I.cols != 0);
    model.opts.nms = 0;
    model.opts.nThreads = 8;
    Mat I_resize;
    float shrink = 1;
    resize(I, I_resize, Size(), 1 / shrink, 1 / shrink);
    uint32* ind;
    uint8* segs;
    uint32* segDims;

    tuple<Mat, Mat> edgeResult = edgesDetect(I_resize, model, 4, ind, segs, segDims);
        Mat E, O;
        tie(E, O) = edgeResult;

    tuple<Mat, Mat> spResult = spDetect(I_resize, E, opt_sp);
    gettimeofday(&t2_sp, 0);
    Mat S, V;
    tie(S,V) = spResult;

    tuple<Mat, Mat, Mat> affinitiesResult =  spAffinities(S, E, segs, segDims);
    Mat unuse3,E0,U;
    tie(unuse3,E0,U) = affinitiesResult;

    return U;
}
