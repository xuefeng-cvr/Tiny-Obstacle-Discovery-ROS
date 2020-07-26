//
// Created by alexma on 23/10/2016.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include "model.h"
#include "MathUtils.hpp"
#include "ConvTri.h"
#include "edgesDetect.hpp"
#include <unistd.h>

using namespace std;
using namespace cv;
    
tuple<Mat, Mat> edgesChns(Mat, _opt);

Mat edgesDetectmain(_model, Mat, Mat, Mat, uint32*&, uint8*&, uint32*&);

Mat edgesNms(Mat, Mat, int, int, float, int);


void writeCSV(string, Mat);

tuple<Mat, Mat> edgesDetect(Mat I, _model model, int nargout, uint32*& ind, uint8*& segs, uint32*& segDims)
{
    //**********************test*************************
    // cout << "you got in the fucntion" << endl;
    //**********************test*************************

    // get parameters
    _opt opts = model.opts;
    const int imWidth = (const int)opts.imWidth;
    const int gtWidth = (const int)opts.gtWidth;
    const int stride = (const int)opts.stride;
    const int nTreesEval = opts.nTreesEval;
    opts.nTreesEval = min(opts.nTreesEval, opts.nTrees);
    opts.multiscale = 0;
    // need to add something here
    opts.stride = max(opts.stride, opts.shrink);
    model.opts = opts;

    //get image size
    int rowsiz = I.rows;
    int colsiz = I.cols;
    auto siz = I.size();
    const int h1 = (int)ceil(double(rowsiz - imWidth) / stride);
    const int w1 = (int)ceil(double(colsiz - imWidth) / stride);
    const int indDims[3] = {h1, w1, nTreesEval};
//    const int segDims[5] = {gtWidth, gtWidth, h1, w1, nTreesEval};
    
    //define outputs
    Mat E(rowsiz, colsiz, CV_32FC1);
    //Mat O(rowsiz, colsiz, CV_32FC1);
//    Mat segs(5, segDims, CV_8UC1);
    // Mat inds(3, indDims, CV_32SC1);
//    tuple<Mat, Mat, Mat, Mat> output;
    tuple<Mat, Mat> output;
    
    // pad image, making divisible by 4
    int r = (int)opts.imWidth / 2;
    int p[4];
    for (int i = 0; i < 4; i++)
        p[i] = r;
    p[1] = p[1] + (4 - (rowsiz + 2 * r) % 4) % 4;
    p[3] = p[3] + (4 - (colsiz + 2 * r) % 4) % 4;
    Mat I_padding;
    copyMakeBorder(I, I_padding, p[0], p[1], p[2], p[3], BORDER_REFLECT);
    I_padding.copyTo(I);
    I_padding.release();
    
     //**********************test*************************
    // cout << "you pad the img" << endl;
    //**********************test*************************
    
    //compute features and apply forest to image
    tuple<Mat, Mat> chns = edgesChns(I, opts);
    
    Mat chnsReg;
    Mat chnsSim;
    tie(chnsReg, chnsSim) = chns;
    
    double s = opts.sharpen;
    if (s)
    {
        Mat I_norm, I_conv;
        normalize(I, I_norm, 0, 1, NORM_MINMAX, CV_32F);
        I_conv = ConvTri(I_norm, 1);
        I_conv.copyTo(I);
        I_norm.release();
        I_conv.release();
    }

    timeval t1_edm,t2_edm;
    gettimeofday(&t1_edm, 0);
    //        tuple<Mat, Mat, Mat> mainoutput = edgesDetectmain(model, I, chnsReg, chnsSim);
    // tuple<Mat, Mat> mainoutput = edgesDetectmain(model, I, inds, chnsReg, chnsSim, segs, segDims);

    E = edgesDetectmain(model, I, chnsReg, chnsSim, ind, segs, segDims);

     //**********************test*************************
    // cout << "you have detected the edges" << endl;
    //**********************test*************************
    
//    tie(E, inds, segs) = mainoutput;
    // tie(E, inds) = mainoutput;
    //normalize and finalize edge maps
    double t = pow(opts.stride, 2) / pow(opts.gtWidth, 2) / opts.nTreesEval;
    r = (int)opts.gtWidth / 2;
    if (s == 0)
        t = t * 2;
    else if (s == 1)
        t = t * 1.8;
    else
        t = t * 1.66;
    Mat new_E;
    E.rowRange(r, rowsiz + r).colRange(r, colsiz + r).copyTo(new_E);
    new_E *= t;
    E = ConvTri(new_E, 1);
    new_E.release();
    
    
    //compute approximate orientation O from edges E
    Mat Oxx, Oxy, Oyy;
    Mat E_conv = ConvTri(E, 4);
    Sobel(E_conv, Oxx, -1, 2, 0);
    Sobel(E_conv, Oxy, -1, 1, 1);
    Sobel(E_conv, Oyy, -1, 0, 2);
    E_conv.release();
    
    float *o = new float[rowsiz * colsiz];
    float *oxx = (float *)Oxx.data;
    float *oxy = (float *)Oxy.data;
    float *oyy = (float *)Oyy.data;
    for (int i = 0; i < rowsiz * colsiz; i++)
    {
        int xysign = -((oxy[i] > 0) - (oxy[i] < 0));
        o[i] = (atan((oyy[i] * xysign / (oxx[i] + 1e-5))) > 0) ? (float)fmod(
                                                                             atan((oyy[i] * xysign / (oxx[i] + 1e-5))), M_PI)
        : (float)fmod(
                      atan((oyy[i] * xysign / (oxx[i] + 1e-5))) + M_PI, M_PI);
    }
    Oxx.release();
    Oxy.release();
    Oyy.release();
    // Mat O(rowsiz, colsiz, CV_32FC1, o);
    Mat O(rowsiz, colsiz, CV_32FC1);
    fillmat<float>(o, O);
    //perform nms
    // if (opts.nms > 0)
    // {
    //     E = edgesNms(E, O, 1, 5, 1.01, opts.nThreads);
    //     // cout << "running NMS" << endl;
    // }
    
    
//    output = make_tuple(E, O, inds, segs);
    // output = make_tuple(E, O, inds);
    output = make_tuple(E, O);

    // Delete parameters
    delete [] o;

    E.release();
    O.release();

    chnsReg.release();
    chnsSim.release();

    return output;
}
