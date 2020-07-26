#ifndef BWLABEL_HPP
#define BWLABEL_HPP

#include <memory.h>
#include <opencv2/opencv.hpp>
#define IS_LEFT_ADJ(p, w) (p % w == 0 ? true : false)
#define IS_BOTTOM_ADJ(p, h, w) (p / w + 1 >= h ? true : false)
#define IS_RIGHT_ADJ(p, w) ((p + 1) % w == 0 ? true : false)
#define IS_TOP_ADJ(p, w) (p < w ? true : false)
#define IS_NOT_ADJ(p, h, w) ((!IS_LEFT_ADJ(p, w)) && (!IS_BOTTOM_ADJ(p, h, w)) && (!IS_RIGHT_ADJ(p, w)) && (!IS_TOP_ADJ(p, w)))
using namespace std;
using namespace cv;

int bwlabel(int *map, int h, int w, int *&connectMap);

Mat bwlabel(const Mat in, int *num, const int mode);

tuple<Mat,int> bwlabel(Mat map, int n);

#endif
