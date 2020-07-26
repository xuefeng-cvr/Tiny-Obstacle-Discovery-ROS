//
// Created by xfbupt on 18-3-16.
//

#include "MathUtils.hpp"

void getaddu(Mat I, unsigned int *I_data) {
    int rows = I.rows;
    int cols = I.cols;
    int i1, j1, k1;
    
    vector<unsigned int> vdata(rows * cols * I.channels());
    vector<Mat> Ivec;
    split(I, Ivec);
//#ifdef _OPENMP
//    int nThreads = min(8, omp_get_max_threads());
//#pragma omp parallel for num_threads(nThreads)
//#endif
    for (i1 = 0; i1 < I.channels(); i1++) {
        for (j1 = 0; j1 < cols; j1++) {
            for (k1 = 0; k1 < rows; k1++) {
                vdata[i1 * rows * cols + j1 * rows + k1] = (unsigned int) Ivec[i1].at<int>(k1, j1);
            }
        }
    }
    
    for (i1 = 0; i1 < I.channels() * rows * cols; i1++)
        I_data[i1] = vdata[i1];
}

void getadd5d(Mat mat5D,uint8* data)
{
    int a = mat5D.size[0];
    int b = mat5D.size[1];
    int c = mat5D.size[2];
    int d = mat5D.size[3];
    int e = mat5D.size[4];
    int i = 0;
//#ifdef _OPENMP
//    int nThreads = min(8, omp_get_max_threads());
//#pragma omp parallel for num_threads(nThreads)
//#endif
    for (int z = 0; z<a; z++)
    {
        for (int y = 0; y<b; y++)
        {
            for (int x = 0; x<c; x++)
            {
                for (int a1 = 0; a1 < d; a1++)
                {
                    for (int a2 = 0; a2 < e; a2++)
                    {
                        uchar* pa = (mat5D.data + mat5D.step[0] * z + mat5D.step[1] * y + mat5D.step[2] * x + a1*mat5D.step[3] + a2*mat5D.step[4]);
                        uint8* pij = (uint8*)pa;
                        data[i++] = *pij;
                    }
                }
            }
        }
    }
}

bool save_Mat(string svpath,int *data,int h,int w,int c)
{
    mat_t    *matfp;
    matvar_t *matvar;
    size_t    dims[3];
    int dim = 2;
    dims[0] = h;
    dims[1] = w;
    if (c>1) {
        dims[2] = c;
        dim = 3;
    }
    matfp = Mat_CreateVer(svpath.c_str(),NULL,MAT_FT_DEFAULT);
    if ( NULL == matfp ) {
        fprintf(stderr,"Error creating MAT file");
        return false;
    }
    matvar = Mat_VarCreate("temp",MAT_C_INT32,MAT_T_INT32,dim,dims,data,0);
    if ( NULL == matvar ) {
        fprintf(stderr,"Error creating variable for 'x'\n");
        return false;
    } else {
        Mat_VarWrite(matfp,matvar,MAT_COMPRESSION_ZLIB);
        Mat_VarFree(matvar);
    }
    Mat_Close(matfp);
    return true;
}
bool save_Mat(string svpath,double *data,int h,int w,int c)
{
    
    mat_t    *matfp;
    matvar_t *matvar;
    size_t    dims[3];
    int dim = 2;
    dims[0] = h;
    dims[1] = w;
    dims[2] = 0;
    if (c>1) {
        dims[2] = c;
        dim = 3;
    }
    matfp = Mat_CreateVer(svpath.c_str(),NULL,MAT_FT_DEFAULT);
    if ( NULL == matfp ) {
        fprintf(stderr,"Error creating MAT file");
        return false;
    }
    matvar = Mat_VarCreate("temp",MAT_C_DOUBLE,MAT_T_DOUBLE,dim,dims,data,0);
    if ( NULL == matvar ) {
        Mat_Close(matfp);
        fprintf(stderr,"Error creating variable for 'x'\n");
        return false;
    } else {
        Mat_VarWrite(matfp,matvar,MAT_COMPRESSION_ZLIB);
        Mat_VarFree(matvar);
    }
    Mat_Close(matfp);
    return true;
}
bool save_Mat(string svpath,float *data,int h,int w,int c)
{
    mat_t    *matfp;
    matvar_t *matvar;
    size_t    dims[3];
    int dim = 2;
    dims[0] = h;
    dims[1] = w;
    if (c>1) {
        dims[2] = c;
        dim = 3;
    }
    matfp = Mat_CreateVer(svpath.c_str(),NULL,MAT_FT_DEFAULT);
    if ( NULL == matfp ) {
        fprintf(stderr,"Error creating MAT file");
        return false;
    }
    matvar = Mat_VarCreate("temp",MAT_C_SINGLE,MAT_T_SINGLE,dim,dims,data,0);
    if ( NULL == matvar ) {
        fprintf(stderr,"Error creating variable for 'x'\n");
        return false;
    } else {
        Mat_VarWrite(matfp,matvar,MAT_COMPRESSION_ZLIB);
        Mat_VarFree(matvar);
    }
    Mat_Close(matfp);
    return true;
}
bool save_Mat(string svpath,uint8 *data,int h,int w,int c)
{
    mat_t    *matfp;
    matvar_t *matvar;
    size_t    dims[3];
    int dim = 2;
    dims[0] = h;
    dims[1] = w;
    if (c>1) {
        dims[2] = c;
        dim = 3;
    }
    matfp = Mat_CreateVer(svpath.c_str(),NULL,MAT_FT_DEFAULT);
    if ( NULL == matfp ) {
        fprintf(stderr,"Error creating MAT file");
        return false;
    }
    matvar = Mat_VarCreate("temp",MAT_C_UINT8,MAT_T_UINT8,dim,dims,data,0);
    if ( NULL == matvar ) {
        fprintf(stderr,"Error creating variable for 'x'\n");
        return false;
    } else {
        Mat_VarWrite(matfp,matvar,MAT_COMPRESSION_ZLIB);
        Mat_VarFree(matvar);
    }
    Mat_Close(matfp);
    return true;
}

bool save_Mat(string svpath,uint32 *data,int h,int w,int c)
{
    
    mat_t    *matfp;
    matvar_t *matvar;
    size_t    dims[3];
    int dim = 2;
    dims[0] = h;
    dims[1] = w;
    if (c>1) {
        dims[2] = c;
        dim = 3;
    }
    matfp = Mat_CreateVer(svpath.c_str(),NULL,MAT_FT_DEFAULT);
    if ( NULL == matfp ) {
        fprintf(stderr,"Error creating MAT file");
        return false;
    }
    matvar = Mat_VarCreate("temp",MAT_C_UINT32,MAT_T_UINT32,dim,dims,data,0);
    if ( NULL == matvar ) {
        fprintf(stderr,"Error creating variable for 'x'\n");
        return false;
    } else {
        Mat_VarWrite(matfp,matvar,MAT_COMPRESSION_ZLIB);
        Mat_VarFree(matvar);
    }
    Mat_Close(matfp);
    return true;
}

double timeInterval(timeval t1,timeval t2)
{
    t2.tv_sec -= t1.tv_sec;
    t2.tv_usec -= t1.tv_usec;
    return (1000000 * t2.tv_sec + t2.tv_usec)/1000.0;
}
