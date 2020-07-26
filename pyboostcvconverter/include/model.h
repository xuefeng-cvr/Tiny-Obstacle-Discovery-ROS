//
// Created by alexma on 21/10/2016.
//
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

#ifndef EDGEBOX_MODEL_H
#define EDGEBOX_MODEL_H

using namespace std;
using namespace cv;

typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

class _opt
{
  public:
    uint32 imWidth, gtWidth;
    uint32 nPos, nNeg, nImgs, nTrees;
    double fracFtrs;
    uint32 minCount, minChild;
    uint32 maxDepth;
    char *discretize;
    uint32 nSamples, nClasses;
    char *split;
    uint32 nOrients;
    uint32 grdSmooth, chnSmooth, simSmooth;
    uint32 normRad;
    uint32 shrink;
    uint32 nCells;
    uint32 rgbd;
    uint32 stride;
    uint32 multiscale;
    uint32 sharpen;
    uint32 nTreesEval;
    uint32 nThreads;
    uint32 nms;
    uint32 seed;
    uint32 useParfor;
    char *modelDir;
    char *modelFnm;
    char *bsdsDir;
    uint32 nChns, nChnFtrs, nSimFtrs, nTotFtrs;
    bool showpic;
    uint32 showtime;
    uint32 showboxnum;

    void init(cv::Mat src, double fracFtrs, bool showpic, char* discretize, 
    char* split, char* modelDir, char* modelFnm, char* bsdsDir)
    {
        this->imWidth    = src.at<int>(0);
        this->gtWidth    = src.at<int>(1);
        this->nPos       = src.at<int>(2);
        this->nNeg       = src.at<int>(3);
        this->nImgs      = src.at<int>(4);
        this->nTrees     = src.at<int>(5);
        this->minCount   = src.at<int>(6);
        this->minChild   = src.at<int>(7);
        this->maxDepth   = src.at<int>(8);
        this->nSamples   = src.at<int>(9);
        this->nClasses   = src.at<int>(10);
        this->nOrients   = src.at<int>(11);
        this->grdSmooth  = src.at<int>(12);
        this->chnSmooth  = src.at<int>(13);
        this->simSmooth  = src.at<int>(14);
        this->normRad    = src.at<int>(15);
        this->shrink     = src.at<int>(16);
        this->nCells     = src.at<int>(17);
        this->rgbd       = src.at<int>(18);
        this->stride     = src.at<int>(19);
        this->multiscale = src.at<int>(20);
        this->sharpen    = src.at<int>(21);
        this->nTreesEval = src.at<int>(22);
        this->nThreads   = src.at<int>(23);
        this->nms        = src.at<int>(24);
        this->seed       = src.at<int>(25);
        this->useParfor  = src.at<int>(26);
        this->nChns      = src.at<int>(27);
        this->nChnFtrs   = src.at<int>(28);
        this->nSimFtrs   = src.at<int>(29);
        this->nTotFtrs   = src.at<int>(30);
        this->showtime   = src.at<int>(31);
        this->showboxnum = src.at<int>(32);
        //double
        this->fracFtrs = fracFtrs;

        //char*
        this->discretize = new char[50];
        this->split = new char[50];
        this->modelDir = new char[50];
        this->modelFnm = new char[50];
        this->bsdsDir = new char[50];

        strcpy(this->discretize, discretize);
        strcpy(this->split, split);
        strcpy(this->modelDir, modelDir);
        strcpy(this->modelFnm, modelFnm);
        strcpy(this->bsdsDir, bsdsDir);
    };
    void display()
    {
      printf("********************opts*****************\n");
      printf("imWidth = %u\n",this->imWidth);
      printf("gtWidth = %u\n",this->gtWidth);
      printf("nPos = %u\n",this->nPos);
      printf("nNeg = %u\n",this->nNeg);
      printf("nImgs = %u\n",this->nImgs);
      printf("nTrees = %u\n",this->nTrees);
      printf("minCount = %u\n",this->minCount);
      printf("minChild = %u\n",this->minChild);   
      printf("maxDepth = %u\n",this->maxDepth);   
      printf("nSamples = %u\n",this->nSamples);   
      printf("nClasses = %u\n",this->nClasses);
      printf("nOrients = %u\n",this->nOrients);
      printf("grdSmooth =%u\n",this->grdSmooth);
      printf("chnSmooth =%u\n",this->chnSmooth);
      printf("simSmooth =%u\n",this->simSmooth);
      printf("normRad = %u\n",this->normRad);
      printf("shrink = %u\n",this->shrink);
      printf("nCells = %u\n",this->nCells);
      printf("rgbd = %u\n",this->rgbd);
      printf("stride = %u\n",this->stride);
      printf("multiscale = %u\n",this->multiscale);
      printf("sharpen = %u\n",this->sharpen);
      printf("nTreesEval = %u\n",this->nTreesEval);
      printf("nThreads = %u\n",this->nThreads);
      printf("nms = %u\n",this->nms);
      printf("seed = %u\n",this->seed);
      printf("useParfor = %u\n",this->useParfor);
      printf("nChns = %u\n",this->nChns);
      printf("nChnFtrs = %u\n",this->nChnFtrs);
      printf("nSimFtrs = %u\n",this->nSimFtrs);
      printf("nTotFtrs = %u\n",this->nTotFtrs);
      printf("showtime = %u\n",this->showtime);
      printf("showboxnum = %u\n",this->showboxnum);

      //double
      printf("fracFtrs = %lf\n", this->fracFtrs);

      //char*
      printf("discretize = %s\n", this->discretize);
      printf("split = %s\n", this->split);
      printf("modelDir = %s\n", this->modelDir);
      printf("modelFnm = %s\n", this->modelFnm);
      printf("bsdsDir = %s\n", this->bsdsDir);
      printf("********************opts*****************\n");
    };
};

class _model
{
  public:
    _opt opts;
    Mat thrs;
    Mat fids;
    Mat child;
    Mat count;
    Mat depth;
    Mat segs;
    Mat nSegs;
    Mat eBins;
    Mat eBnds;

    void init(cv::Mat src, double fracFtrs, bool showpic, char* discretize, char* split, char* modelDir, char* modelFnm, char* bsdsDir, Mat thrs, Mat fids, Mat child, Mat count, Mat depth, Mat segs, Mat nSegs, Mat eBins, Mat eBnds)
    {

        this->opts.init(src, fracFtrs, showpic, discretize, split, modelDir, modelFnm, bsdsDir);
        this->thrs  = thrs ;
        this->fids  = fids ;
        this->child = child;
        this->count = count;
        this->depth = depth;
        this->segs  = segs ;
        this->nSegs = nSegs;
        this->eBins = eBins;
        this->eBnds = eBnds;
    };
    ~_model()
    {
        this->opts.~_opt();
    };
    void display()
    {
        printf("********************MODEL***********************\n");
        printf("thrs %d : %d, %d, %d    type : %d\n", this->thrs.dims, this->thrs.rows, this->thrs.cols, this->thrs.channels(), this->thrs.type());
        printf("fids %d : %d, %d, %d    type : %d\n", this->fids.dims, this->fids.rows, this->fids.cols, this->fids.channels(), this->fids.type());
        printf("child %d : %d, %d, %d   type : %d\n", this->child.dims, this->child.rows, this->child.cols, this->child.channels(), this->child.type());
        printf("count %d : %d, %d, %d   type : %d\n", this->count.dims, this->count.rows, this->count.cols, this->count.channels(), this->count.type());
        printf("depth %d : %d, %d, %d   type : %d\n", this->depth.dims, this->depth.rows, this->depth.cols, this->depth.channels(), this->depth.type());
        printf("segs %d : %d, %d, %d    type : %d\n", this->segs.dims, this->segs.rows, this->segs.cols, this->segs.channels(), this->segs.type());
        cout << this->segs.size << endl;
        printf("nSegs %d : %d, %d, %d   type : %d\n", this->nSegs.dims, this->nSegs.rows, this->nSegs.cols, this->nSegs.channels(), this->nSegs.type());
        printf("eBins %d : %d, %d, %d   type : %d\n", this->eBins.dims, this->eBins.rows, this->eBins.cols, this->eBins.channels(), this->eBins.type());
        printf("eBnds %d : %d, %d, %d   type : %d\n", this->eBnds.dims, this->eBnds.rows, this->eBnds.cols, this->eBnds.channels(), this->eBnds.type());
        printf("********************MODEL***********************\n");
    };
};

class _para
{
  public:
    string name;
    float alpha;
    float beta;
    float eta;
    float minScore;
    int maxBoxes;
    float edgeMinMag;
    float edgeMergeThr;
    float clusterMinMag;
    float maxAspectRatio;
    float minBoxArea;
    float maxBoxLength;
    float gamma;
    float kappa;
};

class _para_sp
{
public:
  string type;
  int nIter;
  int nThreads;
  int k;
  float alpha;
  float beta;
  float merge;
  int bounds;
  Mat seed;

  void init(char* type, cv::Mat param_ints, float alpha, float beta, float merge , cv::Mat *seed)
  {
      this->type = type;
      
      this->nIter = param_ints.at<int>(0);
      this->nThreads = param_ints.at<int>(1);
      this->k = param_ints.at<int>(2);
      this->bounds = param_ints.at<int>(3);

      this->alpha = alpha;
      this->beta = beta;
      this->merge = merge;

      seed->copyTo(this->seed);
  };

  void init1(char* type, cv::Mat param_ints, float alpha, float beta, float merge , cv::Mat *seed)
  {
      this->type = type;
      
      this->nIter = param_ints.at<int>(33);
      this->nThreads = param_ints.at<int>(34);
      this->k = param_ints.at<int>(35);
      this->bounds = param_ints.at<int>(36);

      this->alpha = alpha;
      this->beta = beta;
      this->merge = merge;

      seed->copyTo(this->seed);
  };
  
  void display()
  {
      printf("********************para_sp***********************\n");
      cout << "type: " << this->type << endl;
      cout << "nIter: " << this->nIter << endl;
      cout << "nThreads: " << this->nThreads << endl;
      cout << "k: " << this->k << endl;
      cout << "bounds: " << this->bounds << endl;
      float a = 3.0;
      cout << "a: " << a << endl;

      cout << "alpha: " << this->alpha << endl;
      cout << "beta: " << this->beta << endl;
      cout << "merge: " << this->merge << endl;
      printf("********************para_sp***********************\n");
  };
};

#endif
