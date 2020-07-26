#include <iostream>
#include <opencv2/opencv.hpp>
#include "model.h"
#include <vector>
#include "MathUtils.hpp"
#include "param.h"
#include "bwlabel.h"
#include "ConvTri.h"
#include "rgbConvertMex.hpp"
#include "spAffinities.hpp"

// using namespace std;
using namespace cv;


Mat spDetectmain(string mode, Mat Smat, Mat Emat, bool add, uint nThreads); //boundaries
Mat spDetectmain(string mode, Mat Smat, Mat Emat, uint8* Segs, uint32* segDims, int nThreads);  //affinities
Mat spDetectmain(string mode, Mat Smat, Mat Emat, float thr);               //merge
Mat spDetectmain(string mode, Mat Smat, Mat Imat, Mat Emat, double *prm);   //sticky
Mat spDetectmain(string mode, Mat Smat, Mat Imat, bool hasBnds);            //visualize
Mat spDetectmain(string mode, Mat Smat, Mat Amat);                          //edges


Mat ucm_mean_pb(Mat E, Mat S);

enum ConvolutionType
{
    /* Return the full convolution, including border */
    CONVOLUTION_FULL,

    /* Return only the part that corresponds to the original image */
    CONVOLUTION_SAME,

    /* Return only the submatrix containing elements that were not influenced by the border */
    CONVOLUTION_VALID
};

Mat conv2(const Mat &img, const Mat &ikernel, ConvolutionType type)
{
    Mat dest;
    Mat kernel;
    flip(ikernel, kernel, -1);

    Mat source = img;
    if (CONVOLUTION_FULL == type)
    {
        source = Mat();
        const int additionalRows = kernel.rows - 1, additionalCols = kernel.cols - 1;
        copyMakeBorder(img, source, (additionalRows + 1) / 2, additionalRows / 2, (additionalCols + 1) / 2, additionalCols / 2, BORDER_CONSTANT, Scalar(0));
    }
    Point anchor(kernel.cols - kernel.cols / 2 - 1, kernel.rows - kernel.rows / 2 - 1);
    int borderMode = BORDER_CONSTANT;
    filter2D(source, dest, img.depth(), kernel, anchor, 0, borderMode);

    if (CONVOLUTION_VALID == type)
    {
        dest = dest.colRange((kernel.cols - 1) / 2, dest.cols - kernel.cols / 2).rowRange((kernel.rows - 1) / 2, dest.rows - kernel.rows / 2);
    }
    return dest;
}

Mat upsampleEdges(Mat E0)
{
    int i, j;
    int h = E0.rows;
    int w = E0.cols;
    int h2 = h * 2;
    int w2 = w * 2;
    
    Mat E(h2, w2, CV_64FC1, Scalar(0));
    E0.convertTo(E0,CV_64FC1);
    // upsample the contour map
    for (i = 0; i < h; i = i + 1)
        for (j = 0; j < w; j = j + 1)
            E.at<double>(i * 2, j * 2) = E0.at<double>(i, j);
    
    for (i = 0; i <= h2 - 2; i = i + 2)
        for (j = 1; j <= w2 - 3; j = j + 2)
            E.at<double>(i, j) = min<double>(E.at<double>(i, j - 1), E.at<double>(i, j + 1));
    E.row(h2-2).copyTo(E.row(h2-1));
    
    for (i = 1; i <= h2 - 3; i = i + 2)
        for (j = 0; j <= w2 - 2; j = j + 2)
            E.at<double>(i, j) = min<double>(E.at<double>(i - 1, j), E.at<double>(i + 1, j));
    E.col(w2-2).copyTo(E.col(w2-1));
    
    // remove single pixel segments created by thick edges in E0 (2x2 blocks)
    Mat E0_binary;
    threshold(E0, E0_binary, 0, 1, THRESH_BINARY); //E0 > 0
    double A[] = {0.25, 0.25, 0.25, 0.25};
    Mat conv_mat(2, 2, CV_64FC1, A);
    Mat A_conv = conv2(E0_binary, conv_mat, CONVOLUTION_SAME);
    vector<Point> ps;

    for (i = 0; i < h; i++)
    {
        for (j = 0; j < w; j++)
        {
            if (A_conv.at<double>(i,j) == 1)
            {
                Point p(i,j);
                ps.push_back(p);
            }
        }
    }
    
    int x, y;
    for (i = 0; i < ps.size(); i++)
    {
        x = (ps[i].x) * 2 + 1;
        y = (ps[i].y) * 2 + 1;
        double es[2][4] = {2, 2, 2, 2, 2, 2, 2, 2};
        if (x > 1 && y > 1)
        {
            es[0][0] = E.at<double>(x - 2, y - 1);
            es[1][0] = E.at<double>(x - 1, y - 2);
        }
        if (x < h2 - 3 && y > 1)
        {
            es[0][1] = E.at<double>(x + 2, y - 1);
            es[1][1] = E.at<double>(x + 1, y - 2);
        }
        if (x < h2 - 3 && y < w2 - 3)
        {
            es[0][2] = E.at<double>(x + 2, y + 1);
            es[1][2] = E.at<double>(x + 1, y + 2);
        }
        if (x > 1 && y < w2 - 3)
        {
            es[0][3] = E.at<double>(x - 2, y + 1);
            es[1][3] = E.at<double>(x - 1, y + 2);
        }
        double e = 100000;
        int jj = 0;
        for (j = 0;j < 4; j++)
        {
            double tmp = max(es[0][j],es[1][j]);
            if(e > tmp && tmp > 0)
            {
                e = tmp;
                jj = j;
            }
        }
        
        int x1, y1;
        if (jj == 0 || jj == 3)
            x1 = x - 1;
        else
            x1 = x + 1;
        if (jj == 0 || jj == 1)
            y1 = y - 1;
        else
            y1 = y + 1;
        E.at<double>(x, y1) = e;
        E.at<double>(x1, y) = e;
        E.at<double>(x1, y1) = e;
        if (es[0][jj] < es[1][jj])
            E.at<double>(x, y1) = 0;
        else
            E.at<double>(x1, y) = 0;
    }
    return E;
}

Mat computeUcm(Mat E)
{
    int i, j;
    Mat E_extend(E.rows * 2 + 1, E.cols * 2 + 1, CV_64FC1);
    Mat E_ROI = E_extend(Rect(0, 0, E.cols * 2, E.rows * 2));
    
    Mat E_upsample = upsampleEdges(E);
    
    E_upsample.copyTo(E_ROI);
    E_upsample.release();
    E_extend.row(E_extend.rows-2).copyTo(E_extend.row(E_extend.rows-1));
    E_extend.col(E_extend.cols-2).copyTo(E_extend.col(E_extend.cols-1));
    
    Mat E_0;
    threshold(E_ROI, E_0, 0, 1, THRESH_BINARY);

    E_0 = Mat::ones(E_0.rows, E_0.cols, CV_64FC1) - E_0;
    
    int region_num;
    Mat S;
    tuple<Mat,int> result = bwlabel(E_0, 8);
    
    tie(S,region_num) = result;
    
    Mat S_d(S.rows/2,S.cols/2,CV_64FC1);
    for (i = 1; i < S.rows; i = i + 2)
        for (j = 1; j < S.cols; j = j + 2)
            S_d.at<double>(i/2,j/2) = (double)S.at<int>(i,j) - 1 ;
    S_d.row(S_d.rows-2).copyTo(S_d.row(S_d.rows-1));
    S_d.col(S_d.cols-2).copyTo(S_d.col(S_d.cols-1));
    S_d.copyTo(S);
    S_d.release();
    
    Mat U = ucm_mean_pb(E_extend, S);

    int h = U.size().height;
    int w = U.size().width;
    Mat U_downsample(h / 2, w / 2, CV_64FC1, Scalar(0));
    for (i = 0; i < h - 2; i = i + 2)
        for (j = 0; j < w - 2; j = j + 2)
            U_downsample.at<double>(i / 2, j / 2) = U.at<double>(i, j);
    U_downsample.copyTo(U);
    U_downsample.release();

    // Delete parameters
    E_extend.release();
    E_0.release();
    S.release();

    return U;
}

tuple<Mat, Mat, Mat> spAffinities(Mat S, Mat E, uint8* segs, uint32* segDims, int nThreads)
{
    Mat A = spDetectmain("affinities", S, E, segs, segDims, nThreads);
    
    Mat E0 = spDetectmain("edges", S, A);
    
    Mat U = computeUcm(E0);
    
    tuple<Mat, Mat, Mat> output = make_tuple(A, E0, U);

    return output;
}
