#include "gradient.h"
tuple<Mat, Mat> gradientMag(Mat I, int channel, bool full )
{
    float *I_data = new float[I.rows * I.cols * I.channels()];
    getadd<float>(I, I_data);
    float *M_data = new float[I.rows * I.cols];
    float *O_data = new float[I.rows * I.cols];
    for (int i = 0; i < I.rows * I.cols; i++)
        M_data[i] = 0.f;
    for (int i = 0; i < I.rows * I.cols; i++)
        O_data[i] = 0.f;
    gradMag(I_data, M_data, O_data, I.rows, I.cols, I.channels(), full);
    Mat M(I.rows, I.cols, CV_32F);
    Mat O(I.rows, I.cols, CV_32F);
    fillmat<float>(M_data, M);
    fillmat<float>(O_data, O);
    tuple<Mat, Mat> output = make_tuple(M, O);
    delete[] I_data;
    delete[] M_data;
    delete[] O_data;
    return output;
};

Mat gradientHist(Mat M, Mat O, int bin, int nOrients, bool softbin)
{
    float *M_data = new float[M.rows * M.cols * M.channels()];
    getadd<float>(M, M_data);
    float *O_data = new float[O.rows * O.cols * O.channels()];
    getadd<float>(O, O_data);
    int hb = M.rows / bin;
    int wb = M.cols / bin;
    float *H_data = new float[hb * wb * nOrients];
    for (int i = 0; i < hb * wb * nOrients; i++)
        H_data[i] = 0.f;
    gradHist(M_data, O_data, H_data, M.rows, M.cols, bin, nOrients, softbin);
    Mat H(hb, wb, CV_32FC(nOrients));
    fillmat<float>(H_data, H);

    // Delete parameters
    delete [] M_data;
    delete [] O_data;
    delete [] H_data;

    return H;
};

void gradientMagNorm(Mat M, double normRad, float normConst)
{
    Mat S = ConvTri(M, normRad);
    int h = M.rows;
    int w = M.cols;
    float *M_data = new float[h * w];
    float *S_data = new float[h * w];
    getadd<float>(M, M_data);
    getadd<float>(S, S_data);
    gradMagNorm(M_data, S_data, h, w, normConst);
    fillmat<float>(M_data, M);

    // Delete parameters
    delete [] M_data;
    delete [] S_data;
}
