#define PY_ARRAY_UNIQUE_SYMBOL pbcvt_ARRAY_API

#include <boost/python.hpp>
#include <string>
#include <pyboostcvconverter/pyboostcvconverter.hpp>
#include <boost/python/numeric.hpp>
#include <boost/python/extract.hpp>
#include <time.h>
#include <omp.h>
    

//*********MINIOBJECT**********project dependecies
#include "MathUtils.hpp"
#include "ConvTri.h"
#include "gradient.h"
#include "bwlabel.h"
#include "convConst.h"
#include "wrappers.hpp"
#include "sse.hpp"
#include "rotate.h"
#include "rgbConvertMex.hpp"
#include "param.h"
#include "model.h"
#include "gradientMex.h"
#include "convConst.h"
#include "bwlabel.h"
#include "edgesDetect.hpp"
#include "SpDetect.hpp"
#include "spAffinities.hpp"
#include "edgeBoxes.hpp"

//*********MINIOBJECT**********project dependecies


namespace pbcvt {

    using namespace boost::python;
    using namespace std;

/**
 * @brief Example function. Basic inner matrix product using explicit matrix conversion.
 * @param left left-hand matrix operand (NdArray required)
 * @param right right-hand matrix operand (NdArray required)
 * @return an NdArray representing the dot-product of the left and right operands
 */

    boost::python::tuple edges_detect(
        PyObject *py_img, PyObject *py_uintParams, double fracFtrs, char* str_param,
        PyObject *py_thrs, PyObject *py_fids, PyObject *py_child, PyObject *py_count, PyObject *py_depth, 
        PyObject *py_segs, PyObject *py_nSegs, PyObject *py_eBins, PyObject *py_eBnds)
    {
        //receive ndarray as cv::Mat
        cv::Mat img, uintParams, thrs, fids, fids_temp, child, count, depth, segs, nSegs, eBins, eBnds;
        img        = pbcvt::fromNDArrayToMat(py_img       );
        uintParams = pbcvt::fromNDArrayToMat(py_uintParams);
        thrs       = pbcvt::fromNDArrayToMat(py_thrs      ); 
        fids       = pbcvt::fromNDArrayToMat(py_fids      );
        child      = pbcvt::fromNDArrayToMat(py_child     );
        count      = pbcvt::fromNDArrayToMat(py_count     );
        depth      = pbcvt::fromNDArrayToMat(py_depth     );
        segs       = pbcvt::fromNDArrayToMat(py_segs      );
        nSegs      = pbcvt::fromNDArrayToMat(py_nSegs     );
        eBins      = pbcvt::fromNDArrayToMat(py_eBins     );
        eBnds      = pbcvt::fromNDArrayToMat(py_eBnds     );

        //*******************test*******************
        // printf("you have successfully recevied all the numpy ndarray\n");
        //*******************test*******************


        //parse input string params
        char *discretize = new char[50];
        char *split = new char[50];
        char *modelDir = new char[50];
        char *modelFnm = new char[50];
        char *bsdsDir = new char[50];

        const char *delim = ";";
        char *p;
        p = strtok(str_param, delim);
        strcpy(discretize, p);
        p = strtok(NULL, delim);
        strcpy(split, p);
        p = strtok(NULL, delim);
        strcpy(modelDir, p);
        p = strtok(NULL, delim);
        strcpy(modelFnm, p);
        p = strtok(NULL, delim);
        strcpy(bsdsDir, p);

        //constructing models
        _model model = _model();
        bool showpic = false;
        model.init(uintParams, fracFtrs, showpic, discretize, split, modelDir, modelFnm, bsdsDir
        , thrs, fids, child, count, depth, segs, nSegs, eBins, eBnds); 

        //definde edgesDetect output
        cv::Mat E, O;
        uint32 *ind;
        uint8* edgesDetect_segs;
        uint32* edgsDetect_segDims;

        std::tie(E, O) = edgesDetect(img, model, 3, ind, edgesDetect_segs, edgsDetect_segDims);


        PyObject *nda_E = pbcvt::fromMatToNDArray(E);
        PyObject *nda_O = pbcvt::fromMatToNDArray(O);
        // PyObject *nda_inds = pbcvt::fromMatToNDArray(inds);

        //construct seg info
        _opt opts = model.opts;
        const int imWidth = (const int)opts.imWidth;
        const int gtWidth = (const int)opts.gtWidth;  
        const int stride = (const int)opts.stride;
        const int h = img.rows;
        const int w = img.cols;

        int r = (int)opts.imWidth / 2;
	    int pad[4];

	    for (int i = 0; i < 4; i++)
	        pad[i] = r;
	    pad[1] = pad[1] + (4 - (h + 2 * r) % 4) % 4;
	    pad[3] = pad[3] + (4 - (w + 2 * r) % 4) % 4;

        const int h1 = (int)ceil(double(h + pad[0] + pad[1] - imWidth) / stride);
        const int w1 = (int)ceil(double(w + pad[2] + pad[3] - imWidth) / stride);
        const int nTreesEval = opts.nTreesEval;
        const int indDims[3] = {h1, w1, nTreesEval};

        int segDim_size[] = {5};
        int segs_size[] = {gtWidth * gtWidth * h1 * w1 * nTreesEval};


        timeval start_t1, start_t2;
        gettimeofday(&start_t1, 0);
        cv::Mat mat_inds = cv::Mat(3, indDims, CV_32SC1, ind);
        cv::Mat mat_segs = cv::Mat(1, segs_size, CV_8U, edgesDetect_segs);
        cv::Mat mat_segsDim = cv::Mat(1, segDim_size, CV_32S, edgsDetect_segDims);
        
        
        gettimeofday(&start_t2, 0);

        PyObject *nda_inds = pbcvt::fromMatToNDArray(mat_inds);
        PyObject* nda_segs = pbcvt::fromMatToNDArray(mat_segs);
        PyObject* nda_segsDim = pbcvt::fromMatToNDArray(mat_segsDim);

        delete [] discretize;
        delete [] split;
        delete [] modelDir;
        delete [] modelFnm;
        delete [] bsdsDir;

        delete [] ind;
        delete [] edgesDetect_segs;
        delete [] edgsDetect_segDims;
        
        return boost::python::make_tuple(boost::python::handle<>(nda_E), 
                                         boost::python::handle<>(nda_O),
                                         boost::python::handle<>(nda_inds),
                                         boost::python::handle<>(nda_segs),
                                         boost::python::handle<>(nda_segsDim));
    };

    boost::python::tuple sp_detect(
        PyObject* py_I, PyObject* py_E,
        char* type, PyObject* py_param_ints, double alpha, double beta, double merge, PyObject* py_seed)
    {   
        cv::Mat I, E, param_ints, param_floats, seed;
        I = pbcvt::fromNDArrayToMat(py_I       );
        E = pbcvt::fromNDArrayToMat(py_E       );
        param_ints = pbcvt::fromNDArrayToMat(py_param_ints   );
        seed = pbcvt::fromNDArrayToMat(py_seed );

        _para_sp opt;
        opt.init(type, param_ints, alpha, beta, merge, &seed);

        //define output
        cv::Mat S, V;
        std::tie(S, V) = spDetect(I, E, opt);
        PyObject* nda_S = pbcvt::fromMatToNDArray(S);
        PyObject* nda_V = pbcvt::fromMatToNDArray(V);
        
        return boost::python::make_tuple(boost::python::handle<>(nda_S),
                                         boost::python::handle<>(nda_V));
    };

    boost::python::tuple sp_affinities(
        PyObject* py_S, PyObject* py_E, PyObject* py_segs, PyObject* py_segDims, int nThreads = 4
        )
    {
        cv::Mat S, E, mat_segs, mat_segDims;
        S = pbcvt::fromNDArrayToMat(py_S);
        E = pbcvt::fromNDArrayToMat(py_E);
        mat_segs = pbcvt::fromNDArrayToMat(py_segs);
        mat_segDims = pbcvt::fromNDArrayToMat(py_segDims);

        S.convertTo(S,CV_32S);
        E.convertTo(E,CV_32F);
        mat_segs.convertTo(mat_segs,CV_8U);
        mat_segDims.convertTo(mat_segDims,CV_32S);

        uint8* segs = mat_segs.data;
        uint32* segDims = (uint32*)mat_segDims.data;

        //construct output
        cv::Mat A, E0, U;
        std::tie(A, E0, U) = spAffinities(S, E, segs, segDims);

        PyObject *nda_A = pbcvt::fromMatToNDArray(A);
        PyObject *nda_E0 = pbcvt::fromMatToNDArray(E0);
        PyObject *nda_U = pbcvt::fromMatToNDArray(U);

        return boost::python::make_tuple(boost::python::handle<>(nda_A),
                                         boost::python::handle<>(nda_E0),
                                         boost::python::handle<>(nda_U));
    };

    boost::python::tuple combined(
        PyObject *py_img, PyObject *py_uintParams, PyObject* py_floatParams, char* str_param,
        PyObject *py_thrs, PyObject *py_fids, PyObject *py_child, PyObject *py_count, PyObject *py_depth, 
        PyObject *py_segs, PyObject *py_nSegs, PyObject *py_eBins, PyObject *py_eBnds, PyObject* py_seed)
    {
        //receive ndarray as cv::Mat
        cv::Mat img, uintParams, floatParams, thrs, fids, fids_temp, child, count, depth, segs, nSegs, eBins, eBnds, seed;
        img        = pbcvt::fromNDArrayToMat(py_img       );
        uintParams = pbcvt::fromNDArrayToMat(py_uintParams);
        thrs       = pbcvt::fromNDArrayToMat(py_thrs      ); 
        fids       = pbcvt::fromNDArrayToMat(py_fids      );
        child      = pbcvt::fromNDArrayToMat(py_child     );
        count      = pbcvt::fromNDArrayToMat(py_count     );
        depth      = pbcvt::fromNDArrayToMat(py_depth     );
        segs       = pbcvt::fromNDArrayToMat(py_segs      );
        nSegs      = pbcvt::fromNDArrayToMat(py_nSegs     );
        eBins      = pbcvt::fromNDArrayToMat(py_eBins     );
        eBnds      = pbcvt::fromNDArrayToMat(py_eBnds     );
        floatParams =pbcvt::fromNDArrayToMat(py_floatParams);
        seed       = pbcvt::fromNDArrayToMat(py_seed );

        //parse input float params
        double fracFtrs, alpha, beta, merge;
        fracFtrs = floatParams.at<int>(0);
        alpha = floatParams.at<int>(0);
        beta = floatParams.at<int>(0);
        merge = floatParams.at<int>(0);


        //parse input string params
        char *discretize = new char[50];
        char *split = new char[50];
        char *modelDir = new char[50];
        char *modelFnm = new char[50];
        char *bsdsDir = new char[50];
        char *type = new char[50];


        const char *delim = ";";
        char *p;
        p = strtok(str_param, delim);
        strcpy(discretize, p);
        p = strtok(NULL, delim);
        strcpy(split, p);
        p = strtok(NULL, delim);
        strcpy(modelDir, p);
        p = strtok(NULL, delim);
        strcpy(modelFnm, p);
        p = strtok(NULL, delim);
        strcpy(bsdsDir, p);
        p = strtok(NULL, delim);
        strcpy(type, p);

        //constructing models
        _model model = _model();
        _para_sp opt;

        bool showpic = false;
        model.init(uintParams, fracFtrs, showpic, discretize, split, modelDir, modelFnm, bsdsDir
        , thrs, fids, child, count, depth, segs, nSegs, eBins, eBnds);
        opt.init1(type, uintParams, alpha, beta, merge, &seed);
        
        timeval t1_ed,t2_ed,t1_sp,t2_sp,t1_aff,t2_aff;
        long long tlength_ed,tlength_sp,tlength_aff;
        //definde edgesDetect output
        cv::Mat E, O;
        uint32 *ind;
        uint8* edgesDetect_segs;
        uint32* edgsDetect_segDims;
        // printf("output created\n");
        
        std::tie(E, O) = edgesDetect(img, model, 3, ind, edgesDetect_segs, edgsDetect_segDims);
        std::tuple<Mat, Mat> spResult = spDetect(img, E, opt);
        Mat S, V;
        tie(S,V) = spResult;

        std::tuple<Mat, Mat, Mat> affinitiesResult =  spAffinities(S, E, edgesDetect_segs, edgsDetect_segDims);
        Mat unuse3,E0,U;
        tie(unuse3,E0,U) = affinitiesResult;

        PyObject *nda_A = pbcvt::fromMatToNDArray(unuse3);
        PyObject *nda_E0 = pbcvt::fromMatToNDArray(E0);
        PyObject *nda_U = pbcvt::fromMatToNDArray(U);

        delete [] discretize;
        delete [] split;
        delete [] modelDir;
        delete [] modelFnm;
        delete [] bsdsDir;
        delete [] type;

        delete [] ind;
        delete [] edgesDetect_segs;
        delete [] edgsDetect_segDims;

        return boost::python::make_tuple(boost::python::handle<>(nda_A),
                                         boost::python::handle<>(nda_E0),
                                         boost::python::handle<>(nda_U));
    }

    PyObject *convConst(PyObject * E_origin, int r) 
    {

        cv::Mat E;
        E = pbcvt::fromNDArrayToMat(E_origin);
        E.convertTo(E,CV_32F);

        int h = E.rows, w = E.cols, d = 1, s = 1;

        float *E_array = new float [w*h];
        float *J_array = new float [w*h];
        for(int i = 0; i < h; i++)
        {
            for(int j = 0; j < w; j++)
            {
                E_array[i+j*h] = E.at<float>(i,j);
                J_array[i+j*h] = 0;
            }
        }

        convTri(E_array, J_array, h, w, d, r, s);

        cv::Mat J_out(h,w,CV_32F);

        for(int i = 0; i < h; i++)
        {
            for(int j = 0; j < w; j++)
            {
                J_out.at<float>(i,j) = J_array[i+j*h];
            }
        }

        PyObject *J= pbcvt::fromMatToNDArray(J_out);

        delete [] E_array;
        delete [] J_array;

        return J;

    }

    boost::python::tuple gradientMex(PyObject * I_origin) 
    {

        cv::Mat I;
        I = pbcvt::fromNDArrayToMat(I_origin);

        I.convertTo(I,CV_32F);

        int h = I.rows, w = I.cols, d = 1;
        float *I_array = new float [w*h];
        float *Gx_array = new float [w*h];
        float *Gy_array = new float [w*h];
        for(int i = 0; i < h; i++)
        {
            for(int j = 0; j < w; j++)
            {
                I_array[i+j*h] = I.at<float>(i,j);
                Gx_array[i+j*h] = 0;
                Gy_array[i+j*h] = 0;
            }
        }

        grad2(I_array, Gx_array, Gy_array, h, w, d);

        cv::Mat Gx_out(h,w,CV_32F);
        cv::Mat Gy_out(h,w,CV_32F);
        
        for(int i = 0; i < h; i++)
        {
            for(int j = 0; j < w; j++)
            {
                Gx_out.at<float>(i,j) = Gx_array[i+j*h];
                Gy_out.at<float>(i,j) = Gy_array[i+j*h];
            }
        }

        PyObject *Gx = pbcvt::fromMatToNDArray(Gx_out);
        PyObject *Gy = pbcvt::fromMatToNDArray(Gy_out);

        delete [] I_array;
        delete [] Gx_array;
        delete [] Gy_array;

        return boost::python::make_tuple(boost::python::handle<>(Gx), 
                                         boost::python::handle<>(Gy));

    }


    PyObject *scorebox(PyObject *E_origin, PyObject *O_origin, float alpha, float kappa, 
                        float minScore, float maxNum, float maxRatio, float minArea,
                        PyObject *regionY_origin, PyObject *minmaxH_origin, 
                        PyObject *minmaxW_origion, PyObject *minmaxSZ_origin, int reg_x, int reg_y) 
    {
        cv::Mat E,O;
        E = pbcvt::fromNDArrayToMat(E_origin);
        O = pbcvt::fromNDArrayToMat(O_origin);

        E.convertTo(E,CV_32F);
        O.convertTo(O,CV_32F);

        cv::Mat regionY,minmaxH,minmaxW,minmaxSZ;
        regionY = pbcvt::fromNDArrayToMat(regionY_origin);
        minmaxH = pbcvt::fromNDArrayToMat(minmaxH_origin);
        minmaxW = pbcvt::fromNDArrayToMat(minmaxW_origion);
        minmaxSZ = pbcvt::fromNDArrayToMat(minmaxSZ_origin);

        regionY.convertTo(regionY,CV_32F);
        minmaxH.convertTo(minmaxH,CV_32S);
        minmaxW.convertTo(minmaxW,CV_32S);
        minmaxSZ.convertTo(minmaxSZ,CV_32S);


        float tol = 0.1;
        _para o;
        o.alpha = alpha;
        o.beta = alpha + tol;
        o.kappa = kappa;
        o.minScore = minScore;
        o.maxBoxes = maxNum;
        o.maxAspectRatio = maxRatio;
        o.minBoxArea = minArea;


        cv::Mat bbs_mat_out;
        bbs_mat_out = edgebox_main1(E, O, o, regionY, minmaxH, minmaxW, minmaxSZ, reg_x, reg_y);
        PyObject *bbs_mat = pbcvt::fromMatToNDArray(bbs_mat_out);

        return bbs_mat;

    }

    PyObject *scoreboxesMex(PyObject *E_origin, PyObject *O_origin, float alpha, float beta,
                            float minScore, float maxNum, float edgeMinMag, float edgeMergeThr, 
                            float clusterMinMag, float maxRatio, float minArea, float gamma, 
                            float kappa, PyObject *bboxes_perato_origin) 
    {
        cv::Mat E, O, bboxes_perato;
        E = pbcvt::fromNDArrayToMat(E_origin);
        O = pbcvt::fromNDArrayToMat(O_origin);
        bboxes_perato = pbcvt::fromNDArrayToMat(bboxes_perato_origin);

        E.convertTo(E,CV_32F);
        O.convertTo(O,CV_32F);
        bboxes_perato.convertTo(bboxes_perato,CV_32F);

        int h = bboxes_perato.rows, w = bboxes_perato.cols;

        float *bboxes_f = new float [w*h];
        for(int i = 0; i < h; i++)
        {
            for(int j = 0; j < w; j++)
            {
                bboxes_f[i+j*h] = bboxes_perato.at<float>(i,j);
            }
        }

        _para o;
        o.alpha = alpha;
        o.beta = beta;
        o.minScore = minScore;
        o.maxBoxes = maxNum;
        o.edgeMinMag = edgeMinMag;
        o.edgeMergeThr = edgeMergeThr;
        o.clusterMinMag = clusterMinMag;
        o.maxAspectRatio = maxRatio;
        o.minBoxArea = minArea;
        o.gamma = gamma;
        o.kappa = kappa;
        

        cv::Mat bbs_mat_out;
        bbs_mat_out = edgebox_main2(E, O, o, bboxes_f, h);
        PyObject *bbs_mat = pbcvt::fromMatToNDArray(bbs_mat_out);

        delete [] bboxes_f;

        return bbs_mat;

    }

    PyObject *bbsnms(PyObject *py_bbs, float py_beta, int py_maxBoxes)
    {
        cv::Mat bbs;
        bbs=pbcvt::fromNDArrayToMat(py_bbs);
        bbs.convertTo(bbs,CV_32F);

        Boxes boxes;
        for(int i = 0; i < bbs.rows;i++)
        {
            	Box b;
            	b.r = bbs.at<float>(i,0);
            	b.c = bbs.at<float>(i,1);
            	b.h = bbs.at<float>(i,2);
            	b.w = bbs.at<float>(i,3);
            	b.s = bbs.at<float>(i,4); 
            	boxes.push_back(b);
        }
        
        
        boxesNms(boxes,py_beta,1,py_maxBoxes);
        
        int n = (int)boxes.size();
        float *bbss = new float[n * 5];
        for (int i = 0; i < n; i++)
        {
            bbss[i + 0 * n] = (float)boxes[i].c;
            bbss[i + 1 * n] = (float)boxes[i].r;
            bbss[i + 2 * n] = (float)boxes[i].w;
            bbss[i + 3 * n] = (float)boxes[i].h;
            bbss[i + 4 * n] = boxes[i].s;
        }
        
        Mat bbs_mat(n, 5, CV_32FC1);
        fillmat(bbss, bbs_mat);
        
        PyObject *py_bbss = pbcvt::fromMatToNDArray(bbs_mat);

        delete [] bbss;

        return py_bbss;
    }


    PyObject *compute_integralMap(PyObject *image) {

        cv::Mat imageMat;
        imageMat = pbcvt::fromNDArrayToMat(image);
		imageMat.convertTo(imageMat, CV_64F);
        auto r =imageMat.rows , c =imageMat.cols;

		cv::Mat sum = cv::Mat::zeros(r +1, c + 1, CV_64F);
		cv::Mat sqsum = cv::Mat::zeros(r + 1, c + 1, CV_64F);
		integral(imageMat, sum, sqsum, CV_64F, CV_64F);
        PyObject *ret = pbcvt::fromMatToNDArray(sum);
        return ret;
    }

    PyObject *use_integralMap(PyObject *bbox,PyObject *map) {

        cv::Mat bboxMat,mapMat;
		int minY,minX,maxX,maxY;
		bboxMat = pbcvt::fromNDArrayToMat(bbox);
		mapMat=pbcvt::fromNDArrayToMat(map);
        auto r =bboxMat.rows , c =bboxMat.cols;
		cv::Mat out = cv::Mat::ones(r,1, CV_64FC1);
		for (int i = 0; i < r; i++){
			int* row= bboxMat.ptr<int>(i);
			minY=row[0];
			minX=row[1];
			maxY=row[0]+row[2];
			maxX=row[1]+row[3];
			out.at<double>(i)=mapMat.at<double>(maxY,maxX)+mapMat.at<double>(minY,minX)-mapMat.at<double>(minY,maxX)-mapMat.at<double>(maxY,minX);
		}
        PyObject *ret = pbcvt::fromMatToNDArray(out);
        return ret;
    }



	PyObject *hsi_std(PyObject *H,PyObject *S,PyObject *V,PyObject *H2,PyObject *S2,PyObject *V2,PyObject *b,PyObject *area) {
		
		cv::Mat HMat,SMat,VMat,H2Mat,S2Mat,V2Mat,bMat,areaMat;
		HMat = pbcvt::fromNDArrayToMat(H);
		HMat.convertTo(HMat, CV_64F);
		SMat=pbcvt::fromNDArrayToMat(S);
		SMat.convertTo(SMat, CV_64F);
		VMat = pbcvt::fromNDArrayToMat(V);
		VMat.convertTo(VMat, CV_64F);
		H2Mat=pbcvt::fromNDArrayToMat(H2);
		H2Mat.convertTo(H2Mat, CV_64F);
		S2Mat = pbcvt::fromNDArrayToMat(S2);
		S2Mat.convertTo(S2Mat, CV_64F);
		V2Mat=pbcvt::fromNDArrayToMat(V2);
		V2Mat.convertTo(V2Mat, CV_64F);
		bMat = pbcvt::fromNDArrayToMat(b);
		bMat.convertTo(bMat, CV_32S);
		areaMat=pbcvt::fromNDArrayToMat(area);
		areaMat.convertTo(areaMat, CV_64F);
		

		auto r =HMat.rows,c =HMat.cols,r_b=bMat.rows;
		double H_sum,S_sum,V_sum,H2_sum,S2_sum,V2_sum,h_var,s_var,v_var;
		int minY,minX,maxX,maxY;

		cv::Mat sqsum = cv::Mat::zeros(r + 1, c + 1, CV_64F);
		cv::Mat H_inte = cv::Mat::zeros(r + 1, c + 1, CV_64F);
		cv::Mat S_inte = cv::Mat::zeros(r + 1, c + 1, CV_64F);
		cv::Mat V_inte = cv::Mat::zeros(r + 1, c + 1, CV_64F);
		cv::Mat H2_inte = cv::Mat::zeros(r + 1, c + 1, CV_64F);
		cv::Mat S2_inte = cv::Mat::zeros(r + 1, c + 1, CV_64F);
		cv::Mat V2_inte = cv::Mat::zeros(r + 1, c + 1, CV_64F);
		cv::Mat hsv_std = cv::Mat::zeros(r_b, 3, CV_64F);

		integral(HMat, H_inte, sqsum, CV_64F, CV_64F);
		integral(SMat, S_inte, sqsum, CV_64F, CV_64F);
		integral(VMat, V_inte, sqsum, CV_64F, CV_64F);
		integral(H2Mat, H2_inte, sqsum, CV_64F, CV_64F);
		integral(S2Mat, S2_inte, sqsum, CV_64F, CV_64F);
		integral(V2Mat, V2_inte, sqsum, CV_64F, CV_64F);


		for (int i = 0; i < r_b; i++){
			int* row= bMat.ptr<int>(i);
			minX=row[0];
			minY=row[1];
			maxX=row[2]+1;
			maxY=row[3]+1;
			 
			H_sum=H_inte.ptr<double>(maxY)[maxX]+H_inte.ptr<double>(minY)[minX]-H_inte.ptr<double>(minY)[maxX]-H_inte.ptr<double>(maxY)[minX];
			S_sum=S_inte.ptr<double>(maxY)[maxX]+S_inte.ptr<double>(minY)[minX]-S_inte.ptr<double>(minY)[maxX]-S_inte.ptr<double>(maxY)[minX];
			V_sum=V_inte.ptr<double>(maxY)[maxX]+V_inte.ptr<double>(minY)[minX]-V_inte.ptr<double>(minY)[maxX]-V_inte.ptr<double>(maxY)[minX];
			H2_sum=H2_inte.ptr<double>(maxY)[maxX]+H2_inte.ptr<double>(minY)[minX]-H2_inte.ptr<double>(minY)[maxX]-H2_inte.ptr<double>(maxY)[minX];
			S2_sum=S2_inte.ptr<double>(maxY)[maxX]+S2_inte.ptr<double>(minY)[minX]-S2_inte.ptr<double>(minY)[maxX]-S2_inte.ptr<double>(maxY)[minX];
			V2_sum=V2_inte.ptr<double>(maxY)[maxX]+V2_inte.ptr<double>(minY)[minX]-V2_inte.ptr<double>(minY)[maxX]-V2_inte.ptr<double>(maxY)[minX];

			h_var=(H2_sum-H_sum*(H_sum/areaMat.ptr<double>(i)[0]));
			s_var=(S2_sum-S_sum*(S_sum/areaMat.ptr<double>(i)[0]));
			v_var=(V2_sum-V_sum*(V_sum/areaMat.ptr<double>(i)[0]));

			if(h_var<0) h_var = 0;
			if(s_var<0) s_var = 0;
			if(v_var<0) v_var = 0;

			double* p= hsv_std.ptr<double>(i);
			p[0]=h_var;
			p[1]=s_var;
			p[2]=v_var;

		}
		PyObject *ret = pbcvt::fromMatToNDArray(hsv_std);
		return ret;
	}

	bool equal(double num1,double num2)

	{

		if (fabs(num1-num2) < 0.000001)
	    		return true;
	    	else return false;

	}

	bool big_than(double num1,double num2)

	{

		if(num1-num2>0.000001)
	    		return true;
	    	else return false;

	}

	void mode(std::vector<double> ucmPatch_no0,double &max,int &flag)
	{	int t=0;
		double now=0;
		for (int i = 0; i <ucmPatch_no0.size(); i++){
			if(equal(ucmPatch_no0[i],now))
			{
				t++;
			}
			else
			{
				now=ucmPatch_no0[i];
				t=1;
			}
			if (t>=flag)
			{
				flag=t;
				max=ucmPatch_no0[i];	
			}
		}

	}

	PyObject *MaxPb(PyObject *ucm,PyObject *ucm_0,PyObject *windows,PyObject *r,PyObject *r_c,PyObject *skip) {
		clock_t start,finish,start1,finish1;
		start=clock();
		cv::Mat ucmMat,ucm_0Mat,bMat,rMat,r_cMat,skipMat,ucmPatch_no0;
		ucmMat = pbcvt::fromNDArrayToMat(ucm);
		ucm_0Mat = pbcvt::fromNDArrayToMat(ucm_0);
		bMat = pbcvt::fromNDArrayToMat(windows);
		rMat = pbcvt::fromNDArrayToMat(r);
		r_cMat = pbcvt::fromNDArrayToMat(r_c);
		skipMat = pbcvt::fromNDArrayToMat(skip);
		
		ucmMat.convertTo(ucmMat, CV_64F);
		ucm_0Mat.convertTo(ucm_0Mat, CV_64F);
		bMat.convertTo(bMat, CV_64F);
		auto r_ucm=ucmMat.rows,c_ucm=ucmMat.cols,r_b=bMat.rows;
		
		
		cv::Mat pb_feat = cv::Mat::zeros(r_b, 7, CV_64F);
		cv::Mat sqsum = cv::Mat::zeros(r_ucm + 1, c_ucm + 1, CV_64F);
		cv::Mat ucm_inte = cv::Mat::zeros(r_ucm +1, c_ucm + 1, CV_64F);
		cv::Mat ucm_inte1 = cv::Mat::zeros(r_ucm +1, c_ucm + 1, CV_64F);
		cv::Mat ucm1 = cv::Mat::zeros(r_ucm, c_ucm, CV_64F);
		cv::Mat edge_map_inte = cv::Mat::zeros(r_ucm +1, c_ucm + 1, CV_64F);
		integral(ucmMat, ucm_inte, sqsum, CV_64F, CV_64F);
		integral(ucm_0Mat, edge_map_inte, sqsum, CV_64F, CV_64F);
		

		double max,inte,flag,flag_sum;
		int minY,minX,maxX,maxY;
		std::vector<cv::Mat>ucm_inte10;
			
		for (int i = 0; i<10; i++){
			for (int j=0; j <r_ucm; j++){
				for (int k =0; k<c_ucm; k++){
					if(big_than(ucmMat.ptr<double>(j)[k],skipMat.ptr<double>(i)[0]) &&(big_than(skipMat.ptr<double>(i+1)[0],ucmMat.ptr<double>(j)[k])||equal(skipMat.ptr<double>(i+1)[0],ucmMat.ptr<double>(j)[k])))
						ucm1.ptr<double>(j)[k]=1;
					else{
						ucm1.ptr<double>(j)[k]=0;
					}
				}
			}
			integral(ucm1, ucm_inte1, sqsum, CV_64F, CV_64F);
			ucm_inte10.push_back(ucm_inte1.clone());
		}

		double minVal; 
		double maxVal; 
		Point minLoc; 
		Point maxLoc;	
		for (int i = 0; i <r_b; i++){
			int index=0;
			int* row_0= rMat.ptr<int>(i);
			minX=row_0[0];
			minY=row_0[1];
			maxX=row_0[2]+1;
			maxY=row_0[3]+1;
			flag=0;
			flag_sum=0;
			for (int j = 0; j <10; j++){
				inte=ucm_inte10[j].ptr<double>(maxY)[maxX]+ucm_inte10[j].ptr<double>(minY)[minX]-ucm_inte10[j].ptr<double>(minY)[maxX]-ucm_inte10[j].ptr<double>(maxY)[minX];
				if(big_than(inte,flag)){
					flag=inte;
					maxVal=skipMat.ptr<double>(j+1)[0];
				}
				flag_sum+=inte;
			}

			pb_feat.ptr<double>(i)[1]=maxVal;

			pb_feat.ptr<double>(i)[2]=flag/flag_sum;

			double s=bMat.ptr<double>(i)[2]*bMat.ptr<double>(i)[3];
			pb_feat.ptr<double>(i)[5]=(ucm_inte.ptr<double>(maxY)[maxX]+ucm_inte.ptr<double>(minY)[minX]-ucm_inte.ptr<double>(minY)[maxX]-ucm_inte.ptr<double>(maxY)[minX])/s;
			pb_feat.ptr<double>(i)[6]=(edge_map_inte.ptr<double>(maxY)[maxX]+edge_map_inte.ptr<double>(minY)[minX]-edge_map_inte.ptr<double>(minY)[maxX]-edge_map_inte.ptr<double>(maxY)[minX])/s;
			

			int* row= r_cMat.ptr<int>(i);
			minX=row[0];
			minY=row[1];
			maxX=row[2]+1;
			maxY=row[3]+1;
			pb_feat.ptr<double>(i)[3]=(ucm_inte.ptr<double>(maxY)[maxX]+ucm_inte.ptr<double>(minY)[minX]-ucm_inte.ptr<double>(minY)[maxX]-ucm_inte.ptr<double>(maxY)[minX])*4/s;
			pb_feat.ptr<double>(i)[4]=(edge_map_inte.ptr<double>(maxY)[maxX]+edge_map_inte.ptr<double>(minY)[minX]-edge_map_inte.ptr<double>(minY)[maxX]-edge_map_inte.ptr<double>(maxY)[minX])*4/s;
					
		}
		PyObject *ret = pbcvt::fromMatToNDArray(pb_feat);
		return ret;
	}

	double compute_cosine(cv::Mat leftMat, cv::Mat rightMat) {
		cv::Mat a,b,ab;
		double sum_a=0,sum_b=0,sum_ab=0,result;
		a=leftMat*leftMat.t();
		b=rightMat*rightMat.t();
		ab=leftMat*rightMat.t();

		sum_a=a.ptr<double>(0)[0];
		sum_b=b.ptr<double>(0)[0];
		sum_ab=ab.ptr<double>(0)[0];
		result=1-(sum_ab/(sqrt(sum_a)*sqrt(sum_b)));
		return result;
    }


    PyObject *HSVdist(PyObject *f_bbox,PyObject *b_bbox,PyObject *integralhsv) {

		cv::Mat f_bboxMat,b_bboxMat,intehsvMat;
		int minY,minX,maxX,maxY;
		f_bboxMat = pbcvt::fromNDArrayToMat(f_bbox);
		b_bboxMat = pbcvt::fromNDArrayToMat(b_bbox);	
		intehsvMat = pbcvt::fromNDArrayToMat(integralhsv);
		
		auto r =intehsvMat.rows , c =intehsvMat.cols,r_b=f_bboxMat.rows;
		typedef cv::Vec<int,49> Vec49d;
		
		cv::Mat hsiFeat_back = cv::Mat::zeros(r_b,49, CV_64F);
		cv::Mat hsiFeat_fore = cv::Mat::zeros(r_b,49, CV_64F);
		cv::Mat hsv_dist=cv::Mat::zeros(r_b,3,CV_64F);

		for (int i = 0; i < r_b; i++){
			int* row_b= b_bboxMat.ptr<int>(i);
			minX=row_b[0];
			minY=row_b[1];
			maxX=row_b[2]+1;
			maxY=row_b[3]+1;
			for (int j = 0; j < 49; j++){
				hsiFeat_back.ptr<double>(i)[j]=double(intehsvMat.at<Vec49d>(maxY,maxX)[j]+intehsvMat.at<Vec49d>(minY,minX)[j]-intehsvMat.at<Vec49d>(minY,maxX)[j]-intehsvMat.at<Vec49d>(maxY,minX)[j]);
			}

			int* row_f= f_bboxMat.ptr<int>(i);
			minX=row_f[0];
			minY=row_f[1];
			maxX=row_f[2]+1;
			maxY=row_f[3]+1;
			for (int j = 0; j < 49; j++){
				hsiFeat_fore.ptr<double>(i)[j]=double(intehsvMat.at<Vec49d>(maxY,maxX)[j]+intehsvMat.at<Vec49d>(minY,minX)[j]-intehsvMat.at<Vec49d>(minY,maxX)[j]-intehsvMat.at<Vec49d>(maxY,minX)[j]);
			}
			hsiFeat_back.ptr<double>(i)[0]=hsiFeat_back.ptr<double>(i)[0]+hsiFeat_back.ptr<double>(i)[16];
			hsiFeat_fore.ptr<double>(i)[0]=hsiFeat_fore.ptr<double>(i)[0]+hsiFeat_fore.ptr<double>(i)[16];
			hsiFeat_back.ptr<double>(i)[16]=0;
			hsiFeat_fore.ptr<double>(i)[16]=0;
		}

		hsiFeat_back=hsiFeat_back-hsiFeat_fore;

		for (int i = 0; i < r_b; i++){
			hsv_dist.ptr<double>(i)[0]=compute_cosine(hsiFeat_back(Rect(Point(0, i),Point(16,i+1))),hsiFeat_fore(Rect(Point(0, i),Point(16,i+1))));
			hsv_dist.ptr<double>(i)[1]=compute_cosine(hsiFeat_back(Rect(Point(16,i),Point(33,i+1))),hsiFeat_fore(Rect(Point(16,i),Point(33,i+1))));
			hsv_dist.ptr<double>(i)[2]=compute_cosine(hsiFeat_back(Rect(Point(33,i),Point(49,i+1))),hsiFeat_fore(Rect(Point(33,i),Point(49,i+1))));
		}

		PyObject *ret = pbcvt::fromMatToNDArray(hsv_dist);
		return ret;
	}


	PyObject *compute_feathsv_intgeral(PyObject *e1,PyObject *e2,PyObject *H,PyObject *S,PyObject *V) {
		cv::Mat mask,merge_HSV,e1Mat,e2Mat,HMat,SMat,VMat;

		e1Mat = pbcvt::fromNDArrayToMat(e1);
		e2Mat = pbcvt::fromNDArrayToMat(e2);
		HMat = pbcvt::fromNDArrayToMat(H);
		SMat = pbcvt::fromNDArrayToMat(S);
		VMat = pbcvt::fromNDArrayToMat(V);

		auto r =HMat.rows , c =HMat.cols,e1_num=e1Mat.rows-1,e2_num=e2Mat.rows-1;
		std::vector<cv::Mat> channels_all;

		cv::Mat mask_inte=cv::Mat::zeros(r + 1, c + 1, CV_32F);
		cv::Mat sqsum=cv::Mat::zeros(r + 1, c + 1, CV_32F);

		for (int i = 0; i < e1_num; i++){
			mask=((HMat >=e1Mat.ptr<double>(i)[0])&(HMat < e1Mat.ptr<double>(i+1)[0]))/255;
			mask.convertTo(mask,CV_32F);
			integral(mask, mask_inte, sqsum, CV_32F, CV_32F);
			channels_all.push_back(mask_inte.clone());
		}
		for (int i = 0; i < e2_num; i++){
			mask=((SMat >=e2Mat.ptr<double>(i)[0])&(SMat < e2Mat.ptr<double>(i+1)[0]))/255;
			mask.convertTo(mask,CV_32F);
			integral(mask, mask_inte, sqsum, CV_32F, CV_32F);
			channels_all.push_back(mask_inte.clone());
		}
		for (int i = 0; i < e2_num; i++){
			mask=((VMat >=e2Mat.ptr<double>(i)[0])&(VMat < e2Mat.ptr<double>(i+1)[0]))/255;
			mask.convertTo(mask,CV_32F);
			integral(mask, mask_inte, sqsum, CV_32F, CV_32F);
			channels_all.push_back(mask_inte.clone());
		}
		cv::merge(channels_all, merge_HSV);
		PyObject *ret = pbcvt::fromMatToNDArray(merge_HSV);
		return ret;
	}


    PyObject *dot(PyObject *left, PyObject *right) {

        cv::Mat leftMat, rightMat;
        leftMat = pbcvt::fromNDArrayToMat(left);
        rightMat = pbcvt::fromNDArrayToMat(right);
        auto c1 = leftMat.cols, r2 = rightMat.rows;
        printf("shape : %d, %d\n", leftMat.rows, leftMat.cols);
        printf("shape : %d, %d\n", rightMat.rows, rightMat.cols);
        printf("%d\n", leftMat.at<int>(0));
        printf("%d\n", leftMat.at<int>(1));
        printf("%d\n", leftMat.at<int>(2));
        printf("%d\n", leftMat.at<int>(3));
        if (c1 != r2) {
            PyErr_SetString(PyExc_TypeError,
                            "Incompatible sizes for matrix multiplication.");
            throw_error_already_set();
        }
        cv::Mat result = leftMat * rightMat;
        PyObject *ret = pbcvt::fromMatToNDArray(result);
        return ret;
    }
/**
 * @brief Example function. Simply makes a new CV_16UC3 matrix and returns it as a numpy array.
 * @return The resulting numpy array.
 */

	PyObject* makeCV_16UC3Matrix(){
		cv::Mat image = cv::Mat::zeros(240,320, CV_16UC3);
		PyObject* py_image = pbcvt::fromMatToNDArray(image);
		return py_image;
	}

//
/**
 * @brief Example function. Basic inner matrix product using implicit matrix conversion.
 * @details This example uses Mat directly, but we won't need to worry about the conversion in the body of the function.
 * @param leftMat left-hand matrix operand
 * @param rightMat right-hand matrix operand
 * @return an NdArray representing the dot-product of the left and right operands
 */
    cv::Mat dot2(cv::Mat leftMat, cv::Mat rightMat) {
        auto c1 = leftMat.cols, r2 = rightMat.rows;
        if (c1 != r2) {
            PyErr_SetString(PyExc_TypeError,
                            "Incompatible sizes for matrix multiplication.");
            throw_error_already_set();
        }
        cv::Mat result = leftMat * rightMat;

        return result;
    }

    /**
     * \brief Example function. Increments all elements of the given matrix by one.
     * @details This example uses Mat directly, but we won't need to worry about the conversion anywhere at all,
     * it is handled automatically by boost.
     * \param matrix (numpy array) to increment
     * \return
     */
    cv::Mat increment_elements_by_one(cv::Mat matrix){
        matrix += 1.0;
        return matrix;
    }


#if (PY_VERSION_HEX >= 0x03000000)

    static void *init_ar() {
#else
        static void init_ar(){
#endif
        Py_Initialize();

        import_array();
        return NUMPY_IMPORT_ARRAY_RETVAL;
    }

    BOOST_PYTHON_MODULE (pbcvt) {

        numeric::array::set_module_and_type("numpy", "ndarray");
        //using namespace XM;
        init_ar();

        //initialize converters
        to_python_converter<cv::Mat,pbcvt::matToNDArrayBoostConverter>();
        matFromNDArrayBoostConverter();

        numeric::array::set_module_and_type("numpy","ndarray");

        //expose module-level functions
        def("dot", dot);
        def("dot2", dot2);
		def("makeCV_16UC3Matrix", makeCV_16UC3Matrix);
        def("edges_detect", edges_detect);
        def("sp_detect", sp_detect);
        def("sp_affinities", sp_affinities);
        def("combined", combined);
        def("convConst", convConst);
        def("gradientMex", gradientMex);
        def("scorebox", scorebox);
        def("scoreboxesMex", scoreboxesMex);
        def("bbsnms", bbsnms);
        def("compute_integralMap",compute_integralMap);
		def("use_integralMap",use_integralMap);
		def("hsi_std",hsi_std);
		def("MaxPb", MaxPb);
		def("HSVdist", HSVdist);
		def("compute_feathsv_intgeral", compute_feathsv_intgeral);

		//from PEP8 (https://www.python.org/dev/peps/pep-0008/?#prescriptive-naming-conventions)
        //"Function names should be lowercase, with words separated by underscores as necessary to improve readability."
        def("increment_elements_by_one", increment_elements_by_one);
    }

} //end namespace pbcvt
