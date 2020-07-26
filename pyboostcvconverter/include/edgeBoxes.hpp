/*******************************************************************************
* Structured Edge Detection Toolbox      Version 3.01
* Code written by Piotr Dollar and Larry Zitnick, 2014.
* Licensed under the MSR-LA Full Rights License [see license.txt]
*******************************************************************************/
#include <algorithm>
#include <vector>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "MathUtils.hpp"
#include "model.h"
#include "sse.hpp"
#include <omp.h>
#include <sys/time.h>

#ifndef _OPENMP
#define _OPENMP
#endif


using namespace std;
using namespace cv;
#define PI 3.14159265f
// template<typename T>
// void getadd(Mat, T *);



int clamp(int v, int a, int b);

// trivial array class encapsulating pointer arrays
template <class T>
class Array
{
  public:
    Array()
    {
        _h = _w = 0;
        _x = 0;
        _free = 0;
    }

    virtual ~Array() { clear(); }

    void clear()
    {
        if (_free)
            delete[] _x;
        _h = _w = 0;
        _x = 0;
        _free = 0;
    }

    void init(int h, int w)
    {
        clear();
        _h = h;
        _w = w;
        _x = new T[h * w]();
        _free = 1;
    }

    T &val(size_t c, size_t r) { return _x[c * _h + r]; }

    int _h, _w;
    T *_x;
    bool _free;
};

// convenient typedefs
typedef vector<float> vectorf;
typedef vector<int> vectori;
typedef Array<float> arrayf;
typedef Array<int> arrayi;

// bounding box data structures and routines
typedef struct
{
    int c, r, w, h;
    float s;
    float v[3];
} Box;
typedef vector<Box> Boxes;

bool filt_bbox(Box &box, float ** regionY, int ** minmaxH, int ** minmaxW, int ** minmaxSZ, int reg_x, int reg_y);

bool boxesCompare(const Box &a, const Box &b);

float boxesOverlap(Box &a, Box &b);

void boxesNms(Boxes &boxes, float thr, float eta, int maxBoxes);

// main class for generating edge boxes
class EdgeBoxGenerator
{
  public:
    // method parameters (must be manually set)
    float _alpha, _beta, _eta, _minScore;
    int _maxBoxes;
    float _edgeMinMag, _edgeMergeThr, _clusterMinMag;
    float _maxAspectRatio, _minBoxArea, _maxBoxLength, _gamma, _kappa;

    // main external routine (set parameters first)
    void generate(Boxes &boxes, arrayf &E, arrayf &O, arrayf &V, 
                    float ** regionY, int ** minmaxH, int ** minmaxW, 
                    int ** minmaxSZ, int reg_x, int reg_y);

  private:
    // edge segment information (see clusterEdges)
    int h, w;                   // image dimensions
    int _segCnt;                // total segment count
    arrayi _segIds;             // segment ids (-1/0 means no segment)
    vectorf _segMag;            // segment edge magnitude sums
    vectori _segR, _segC;       // segment lower-right pixel
    vector<vectorf> _segAff;    // segment affinities
    vector<vectori> _segAffIdx; // segment neighbors

    // data structures for efficiency (see prepDataStructs)
    arrayf _segIImg, _magIImg;
    arrayi _hIdxImg, _vIdxImg;
    vector<vectori> _hIdxs, _vIdxs;
    vectorf _scaleNorm;
    float _scStep, _arStep, _rcStepRatio;

    // data structures for efficiency (see scoreBox)
    arrayf _sWts;
    arrayi _sDone, _sMap, _sIds;
    int _sId;

    // helper routines
    void clusterEdges(arrayf &E, arrayf &O, arrayf &V);

    void prepDataStructs(arrayf &E);

    void scoreAllBoxes(Boxes &boxes, float ** regionY, int ** minmaxH, int ** minmaxW, int ** minmaxSZ, int reg_x, int reg_y);

    void scoreBox(Box &box);

    void refineBox(Box &box);

    void drawBox(Box &box, arrayf &E, arrayf &V);
};


class EdgeBoxGenerator2
{
  public:
    // method parameters (must be manually set)
    float _alpha, _beta, _minScore;
    int _maxBoxes;
    float _edgeMinMag, _edgeMergeThr, _clusterMinMag;
    float _maxAspectRatio, _minBoxArea, _gamma, _kappa;

    // main external routine (set parameters first)
    void generate(Boxes &boxes, arrayf &E, arrayf &O, arrayf &V);

  private:
    // edge segment information (see clusterEdges)
    int h, w;                   // image dimensions
    int _segCnt;                // total segment count
    arrayi _segIds;             // segment ids (-1/0 means no segment)
    vectorf _segMag;            // segment edge magnitude sums
    vectori _segR, _segC;       // segment lower-right pixel
    vector<vectorf> _segAff;    // segment affinities
    vector<vectori> _segAffIdx; // segment neighbors

    // data structures for efficiency (see prepDataStructs)
    arrayf _segIImg, _magIImg;
    arrayi _hIdxImg, _vIdxImg;
    vector<vectori> _hIdxs, _vIdxs;
    vectorf _scaleNorm;
    float _scStep, _arStep, _rcStepRatio;

    // data structures for efficiency (see scoreBox)
    arrayf _sWts;
    arrayi _sDone, _sMap, _sIds;
    int _sId;

    // helper routines
    void clusterEdges(arrayf &E, arrayf &O, arrayf &V);
    void prepDataStructs(arrayf &E);
    void scoreBox(Box &box);
    void scoreAllBoxes(Boxes &boxes);
};


class EdgeBoxGenerator_hirec
{
  public:
    // method parameters (must be manually set)
    float _alpha, _beta, _eta, _minScore;
    int _maxBoxes;
    float _maxAspectRatio, _minBoxArea, _kappa, _stepCoeff, _old_rcStepRatio;

    // main external routine (set parameters first)
    void generate(Boxes &boxes, arrayf &E, arrayf &O, arrayf &V);

  private:
    // edge segment information (see clusterEdges)
    int h, w;                   // image dimensions
    int _segCnt;                // total segment count
    arrayi _segIds;             // segment ids (-1/0 means no segment)
    vectorf _segMag;            // segment edge magnitude sums
    vectori _segR, _segC;       // segment lower-right pixel
    vector<vectorf> _segAff;    // segment affinities
    vector<vectori> _segAffIdx; // segment neighbors

    // data structures for efficiency (see prepDataStructs)
    arrayf _segIImg, _magIImg;
    arrayi _hIdxImg, _vIdxImg;
    vector<vectori> _hIdxs, _vIdxs;
    vectorf _scaleNorm;
    float _scStep, _arStep, _rcStepRatio;

    // data structures for efficiency (see scoreBox)
    arrayf _sWts;
    arrayi _sDone, _sMap, _sIds;
    int _sId;

    // helper routines
    void clusterEdges(arrayf &E, arrayf &O, arrayf &V);

    void prepDataStructs(arrayf &E);

    void scoreAllBoxes(Boxes &boxes);

    void scoreBox(Box &box);

    void refineBox(Box &box);

    void drawBox(Box &box, arrayf &E, arrayf &V);
};

Mat edgebox_main1(Mat E0, Mat O0, _para o, Mat regionY_origin, Mat minmaxH_origin, 
                    Mat minmaxW_origin, Mat minmaxSZ_origin, int reg_x, int reg_y);

Mat edgebox_main2(Mat E0, Mat O0, _para o, float *bboxes_f, int num_box);
