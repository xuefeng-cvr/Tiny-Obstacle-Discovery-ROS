#ifndef MATHUTILS_HPP
#define MATHUTILS_HPP

#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>
#include "string.h"
#include "matio.h"
#include <sys/time.h>

#ifdef _OPENMP
#include <omp.h>
#endif

using namespace std;
using namespace cv;

typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

bool save_Mat(string svpath,int *data,int h,int w,int c = 1);
bool save_Mat(string svpath,double *data,int h,int w,int c = 1);
bool save_Mat(string svpath,float *data,int h,int w,int c = 1);
bool save_Mat(string svpath,uint8 *data,int h,int w,int c = 1);
bool save_Mat(string svpath,uint32 *data,int h,int w,int c = 1);
void getadd5d(Mat mat5D,uint8* data);
void getaddu(Mat I, unsigned int *I_data);
double timeInterval(timeval,timeval);

//change Mat to Muiti-array
template <typename T>
T** init_array(Mat s, size_t sx, size_t sy)
{
    T **tmp = new T *[sx];
    for(size_t i = 0; i < sx; i++)
    {
        tmp[i] = new T[sy];
        for(size_t j = 0; j < sy; j++)
        {
            tmp[i][j] = s.at<T>(i,j);
        }
    }
    return tmp;
}

// delete array
template <typename T>
void delete_array(T** tmp, size_t sx)
{
    for(size_t i = 0; i < sx; i++)
    {
        delete [] tmp[i];
    }
    delete tmp;
}

// initialize Mat
template <typename T>
void InitMat(Mat tmp)
{
    for(size_t i = 0; i < tmp.rows; i++)
    {
        for(size_t j = 0; j < tmp.cols; j++)
        {
            tmp.at<T>(i,j) = 0;
        }
    }
}

// 2018.3.22 getadd：44.6548ms
// 2018.3.22 2 getadd：36.819ms
template<typename T>
void getadd(const Mat& I, T *dst)
{
    T* pdst = dst;
//    int nr = I.rows;
//    int nc = I.cols;
    int nr,nc;
    int c = I.channels();
    vector<Mat> Ivec;
    split(I, Ivec);
    for(int k = 0;k < c;k ++)
    {
        Mat It;
        transpose(Ivec[k], It);
        nr = It.rows;
        nc = It.cols;
        if(It.isContinuous())
        {
            nr=1;
            nc=nc * It.rows;
        }
        for(int i=0;i < nr;i ++)
        {
            T* psrc=(T*)It.ptr<T>(i);
            memcpy(pdst,psrc,nc*sizeof(T));
            pdst = pdst + nc;
        }
    }
}

template <typename T>
void fillmat(T *I_data, Mat& I)
{
    assert(I.channels() == 1 || I.channels() == 2 || I.channels() == 3 || I.channels() == 4);
    int type;
    if(I.channels() > 1)
    {
        switch (I.type()) {
            case CV_8UC2:
            case CV_8UC3:
            case CV_8UC4:
                type = CV_8UC1;
                break;
            case CV_8SC2:
            case CV_8SC3:
            case CV_8SC4:
                type = CV_8SC1;
                break;
            case CV_16UC2:
            case CV_16UC3:
            case CV_16UC4:
                type = CV_16UC1;
                break;
            case CV_32SC2:
            case CV_32SC3:
            case CV_32SC4:
                type = CV_32SC1;
                break;
            case CV_32FC2:
            case CV_32FC3:
            case CV_32FC4:
                type = CV_32FC1;
                break;
            case CV_64FC2:
            case CV_64FC3:
            case CV_64FC4:
                type = CV_64FC1;
                break;
            default:
                perror("NOT SUPPORT THIS CV_DATATYPE!!!");
                break;
        }
    }
    else
    {
        type = I.type();
    }
    Mat result;
    int nr = I.cols,nc = I.rows;
    vector<Mat> Ivec;
    T* psrc = I_data;
    for(int k = 0;k<I.channels();k++)
    {
        Mat dst(nr,nc,type,psrc);
        psrc = psrc + nr*nc;
        Ivec.push_back(dst);
    }
    if(I.channels() > 1)
        merge(Ivec,result);
    else
        Ivec[0].copyTo(result);
    transpose(result,I);

    return;
}


#endif
