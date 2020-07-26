//
// Created by alexma on 14/11/2016.
//
#ifndef ROTATE_HPP
#define ROTATE_HPP
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Point2f RotatePoint1(const Point2f &p, float rad);

Point2f RotatePoint(const Point2f &cen_pt, const Point2f &p, float rad);

#endif