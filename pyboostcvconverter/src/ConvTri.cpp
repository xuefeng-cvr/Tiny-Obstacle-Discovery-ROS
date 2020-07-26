#include "ConvTri.h"
Mat ConvTri(Mat I, double r, int s)
{
    int height = I.rows;
    int width = I.cols;
    int depth = I.channels();
    Mat J, J1;
    if (r > 0 & r <= 1)
    {
        float p = (float)(12 / r / (r + 2) - 2);
        Mat kernel;
        kernel.push_back<float>(1 / (2 + p));
        kernel.push_back<float>(p / (2 + p));
        kernel.push_back<float>(1 / (2 + p));
        Mat kernel_t(kernel.cols, kernel.rows, kernel.type());
        transpose(kernel, kernel_t);
        r = 1;
        Point anchor = Point(-1, -1);
        filter2D(I, J, CV_32F, kernel_t, anchor, 0, BORDER_REFLECT);
        filter2D(J, J1, CV_32F, kernel, anchor, 0, BORDER_REFLECT);
        kernel.release();
        kernel_t.release();
    }
    else
    {
        Mat kernel;
        float ke = 0;
        for (int i = 0; i < r; i++)
        {
            ke = i + 1;
            kernel.push_back<float>(ke);
        }
        ke = (float)r + 1;
        kernel.push_back<float>(ke);
        for (int i = (int)r + 1; i < 2 * r + 1; i++)
        {
            ke = (float)(2 * r + 1 - i);
            kernel.push_back<float>(ke);
        }
        kernel /= pow((r + 1), 2);
        Mat kernel_t(kernel.cols, kernel.rows, kernel.type());
        transpose(kernel, kernel_t);
        Point anchor = Point(-1, -1);
        filter2D(I, J, CV_32F, kernel_t, anchor, 0, BORDER_REFLECT);
        filter2D(J, J1, CV_32F, kernel, anchor, 0, BORDER_REFLECT);
        kernel.release();
        kernel_t.release();
    }

    J.release();

    //handle the difference between row major and col major layout
    return J1;
}
