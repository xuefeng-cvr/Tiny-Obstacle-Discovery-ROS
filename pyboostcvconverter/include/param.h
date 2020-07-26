#ifndef PARAM_HPP
#define PARAM_HPP

#include "model.h"
#include "matio.h"
#include <opencv2/opencv.hpp>
#include <iostream>

#ifdef _OPENMP
#include <omp.h>
#endif
using namespace std;
using namespace cv;

_para initial_para();

_para_sp initial_para_sp();

_model loadmodel();

#endif
