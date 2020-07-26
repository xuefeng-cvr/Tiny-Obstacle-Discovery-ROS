#include <iostream>
#include <opencv2/opencv.hpp>
#include <memory.h>
#include "model.h"
#include <vector>
#include "MathUtils.hpp"
#include "param.h"
#include "ConvTri.h"
#include "rgbConvertMex.hpp"
#include "SpDetect.hpp"

// using namespace std;
using namespace cv;

Mat spDetectmain(string mode, Mat Smat, Mat Emat, bool add, uint32 nThreads); //boundaries
Mat spDetectmain(string mode, Mat Smat, Mat Emat, Mat segs, int nThreads);  //affinities
Mat spDetectmain(string mode, Mat Smat, Mat Emat, float thr);               //merge
Mat spDetectmain(string mode, Mat Smat, Mat Imat, Mat Emat, double *prm);   //sticky
Mat spDetectmain(string mode, Mat Smat, Mat Imat, bool hasBnds);            //visualize

// Mat ConvTri(Mat I, double r, int downSample = 1);

tuple<Mat, Mat> spDetect(Mat I, Mat E, _para_sp opt)
{
    //**********total_time*********
    timeval t1_total, t2_total;
    gettimeofday(&t1_total, 0);

    const string type = opt.type;
    const int nIter = opt.nIter;
    const int nThreads = opt.nThreads;
    const int k = opt.k;
    const int bounds = opt.bounds;
    const float alpha = opt.alpha;
    const float beta = opt.beta;
    const float merge = opt.merge;
    Mat seed;
    opt.seed.copyTo(seed);

    float sigs[] = {k * alpha / (float)1e4, alpha / (float)1e4, (1 - alpha) * beta, (1 - alpha) * (1 - beta)};
    int h = I.rows;
    int w = I.cols;
    double *p;
    Mat S;

    I.convertTo(I, CV_8UC3);
    uint8 *I_data = new uint8[I.rows * I.cols * I.channels()];
    getadd<uint8>(I, I_data);
    
    timeval t1_rgbconvert,t2_rgbconvert;
    gettimeofday(&t1_rgbconvert, 0);
    float *J = rgbConvert<uint8, float>(I_data, h * w, 3, 1, 1.0f / 255);

    delete [] I_data;
    Mat I_single(I.size(), CV_32FC3);
    fillmat<float>(J, I_single);

    if (!seed.empty())
    {
        if (bounds)
        {
            S = spDetectmain("boundaries", seed, E, false, nThreads);
        }
    }
    else
    {
        int downSample = 2;
        int h1 = h - (h % downSample);
        int w1 = w - (w % downSample);
        int h2 = h1 / downSample;
        int w2 = w1 / downSample;
        Size size(w2, h2);

        Mat I_ROI = I_single(Range(0, h1), cv::Range(0, w1));
        Mat E_ROI = E(Range(0, h1), cv::Range(0, w1));
        Mat I0(h2, w2, CV_32FC3);
        Mat E0(size, CV_32FC1);
        
        timeval t1_resize,t2_resize;
        gettimeofday(&t1_resize, 0);
        resize(I_ROI, I0, I0.size(), 0, 0, INTER_LINEAR);
        resize(E_ROI, E0, Size(), 1.0/downSample, 1.0/downSample, INTER_LINEAR);
        
        I_ROI.release();
        E_ROI.release();

        uint32 *vec = new uint32[h2 * w2];
        for (uint32 i = 0; i < h2 * w2; i++)
            vec[i] = i;
        Mat S_tmp(h2,w2, CV_32SC1);
        fillmat<uint32>(vec, S_tmp);
        S_tmp.copyTo(S);
        S_tmp.release();
        
        double p1[] = {(double)nIter * 2, (double)nThreads, sigs[0] / downSample / downSample, sigs[1] * downSample * downSample, sigs[2], sigs[3]};
        p = p1;
        Mat S_sticky;
        Mat S_resize;
        Mat S_pad;
        Mat convI = ConvTri(I0, 1);
        
        timeval t1_sticky1,t2_sticky1;
        gettimeofday(&t1_sticky1, 0);
        S_sticky = spDetectmain("sticky", S, convI, E0, p);;
        
        resize(S_sticky, S_resize, Size(), downSample, downSample, INTER_NEAREST);
        copyMakeBorder(S_resize, S_pad, 0, h - h1, 0, w - w1, BORDER_REPLICATE);
        S_pad.copyTo(S);
        
        S_pad.release();
        S_sticky.release();
        S_resize.release();

        delete [] vec;

        // Delete parameters
        I0.release();
        E0.release();
        convI.release();

    }
    double p2[] = {(double)nIter, (double)nThreads, sigs[0], sigs[1], sigs[2], sigs[3]};
    p = p2;
    int b = 0;
    
    timeval t1_sticky2,t2_sticky2;
    gettimeofday(&t1_sticky2, 0);
    S = spDetectmain("sticky", S, ConvTri(I_single, 1), E, p);

    if (bounds != b)
    {
        Mat S_bound;
        timeval t1_bound,t2_bound;
        gettimeofday(&t1_bound, 0);
        S_bound = spDetectmain("boundaries", S, E, bounds, nThreads);
        S_bound.copyTo(S);
        S_bound.release();
    }
    if (merge > 0 && bounds)
    {
        Mat S_merge;
        timeval t1_merge,t2_merge;
        gettimeofday(&t1_merge, 0);
        S_merge = spDetectmain("merge", S, E, merge);
        S_merge.copyTo(S);
        S_merge.release();
    }
    timeval t1_vis,t2_vis;
    gettimeofday(&t1_vis, 0);
    Mat V = spDetectmain("visualize", S, I_single, bounds > 0);

    tuple<Mat, Mat> output = make_tuple(S, V);

    // Delete parameters
    delete [] J;
    I_single.release();

    S.release();
    V.release();

    return output;
}
