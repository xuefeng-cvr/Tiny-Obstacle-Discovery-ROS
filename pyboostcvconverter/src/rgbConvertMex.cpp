#include "rgbConvertMex.hpp"
#include "MathUtils.hpp"

Mat rgbToLuvu(Mat I)
{
    uint8 *I_data = new uint8[I.rows * I.cols * I.channels()];
    getadd<uint8>(I, I_data);
    
    float *J_data;
    float norm = 1.0f / 255;
    J_data = rgbConvert<uint8, float>(I_data, I.rows * I.cols, I.channels(), 2, norm);

    Mat J(I.rows, I.cols, CV_32FC(I.channels()));
    fillmat<float>(J_data, J);

    // Delete parameters
    delete [] I_data;
    delete [] J_data;
    return J;
}