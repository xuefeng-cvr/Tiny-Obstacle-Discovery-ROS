/*******************************************************************************
 * Structured Edge Detection Toolbox      Version 3.01
 * Code written by Piotr Dollar, 2014.
 * Licensed under the MSR-LA Full Rights License [see license.txt]
 *******************************************************************************/
#include <opencv2/opencv.hpp>
#include <math.h>
#include <string.h>
#include "MathUtils.hpp"
#include "tbb/tbb.h"
 

#define USEOMP
#ifdef _OPENMP
#include <omp.h>
#endif
using namespace std;
using namespace cv;
using namespace tbb;


typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;



// refine sticky superpixels given image and edge data
void sticky(uint32 *S, int h, int w, float *I, float *E, double *prm) {
    //*************test*******************
    int* index = (int*)malloc(sizeof(int) * w);
    for(int i = 0; i < w; i++)
        index[i] = i;


    // get additional parameters
    const uint32 maxIter = uint32(prm[0]);
    uint32 nThreads = uint32(prm[1]);
    const float sigs = float(prm[2]), sigx = float(prm[3]);
    const float sige = float(prm[4]), sigc = float(prm[5]);
    
    // initialize mus and ns
    int m = 0;
    for (int x = 0; x < w * h; x++)
        m = S[x] > m ? S[x] : m;
    m++;
    float *ns = (float *) malloc(sizeof(float) * m);
    float *mus = (float *) malloc(sizeof(float) * m * 5);
    memset(ns, 0, sizeof(float) * m);
    memset(mus, 0, sizeof(float) * m * 5);
    for (int x = 0; x < w; x++)
    {
        for (int y = 0; y < h; y++)
        {
            int i = (int)S[x * h + y];
            ns[i]++;
            mus[i * 5] += (float)x;
            mus[i * 5 + 1] += (float)y;
            for (int z = 0; z < 3; z++)
                mus[i * 5 + z + 2] += I[z * h * w + x * h + y];
        }
    }
    for (uint32 i = 0; i < m; i++)
        for (uint32 z = 0; z < 5; z++)
            mus[i * 5 + z] /= ns[i];
    
    // iterate moving boundaries
    uint32 changed = 1, iter = 0;
    while (changed && iter < maxIter) {
        changed = 0;
#ifdef USEOMP
        nThreads = min(nThreads, uint32(omp_get_max_threads()));
#pragma omp parallel for num_threads(nThreads)
#endif
        for (int xi = 0; xi < int(w); xi++)
            for (uint32 y = 0; y < h; y++) {
                // get local region, if all uniform continue
                uint32 x = xi, x0, x1, y0, y1, i, j, s = S[x * h + y], t, sBest, T[4];
                x0 = x > 0 ? x - 1 : x;
                x1 = x < w - 1 ? x + 1 : x;
                y0 = y > 0 ? y - 1 : y;
                y1 = y < h - 1 ? y + 1 : y;
                T[0] = S[x0 * h + y];
                T[1] = S[x1 * h + y];
                T[2] = S[x * h + y0];
                T[3] = S[x * h + y1];
                if (s == T[0] && s == T[1] && s == T[2] && s == T[3])
                    continue;
                // compute error of each sp label, store best
                float d, e, dBest = 1e10f, ds[5], vs[5], es[4];
                vs[0] = float(x);
                vs[1] = float(y);
                sBest = T[0] + 1;
                for (j = 0; j < 3; j++)
                    vs[j + 2] = I[j * h * w + x * h + y];
                es[0] = E[x0 * h + y];
                es[1] = E[x1 * h + y];
                es[2] = E[x * h + y0];
                es[3] = E[x * h + y1];
                for (i = 0; i < 4; i++) {
                    t = T[i];
                    if (t == sBest)
                        continue;
                    e = 1e10f;
                    for (j = 0; j < 4; j++)
                        if (T[j] != t)
                            e = e < es[j] ? e : es[j];
                    for (j = 0; j < 5; j++) {
                        ds[j] = mus[t * 5 + j] - vs[j];
                        ds[j] *= ds[j];
                    }
                    d = (ds[0] + ds[1]) * sigx + (ds[2] + ds[3] + ds[4]) * sigc - e * sige;
                    d += sigs / ns[t];
                    if (d < dBest) {
                        dBest = d;
                        sBest = t;
                    }
                }
                // assign pixel to closest sp, update new and old sp
                if (sBest != s) {
                    t = sBest;
                    changed++;
                    ns[s]--;
                    ns[t]++;
                    S[x * h + y] = t;
                    for (j = 0; j < 5; j++)
                        mus[s * 5 + j] = (mus[s * 5 + j] * (ns[s] + 1) - vs[j]) / ns[s];
                    for (j = 0; j < 5; j++)
                        mus[t * 5 + j] = (mus[t * 5 + j] * (ns[t] - 1) + vs[j]) / ns[t];
                }
            }
        // parallel_for(blocked_range<size_t>(0,w), ApplyFoo(index, h, w, S, &changed, E, I, mus, ns, sigx, sigc, sige, sigs));
        iter++;
    }
    delete[] ns;
    delete[] mus;
    delete[] index;
}

// relabel superpixels (condense and enforce connectivity)
void relabel(uint32 *S, uint32 h, uint32 w) {
    uint32 *T = new uint32[h * w], *map = new uint32[h * w / 2]();
    uint32 x, y, z, t, t1, t2, m = 1;
    T[0] = m++;
    for (y = 1; y < h; y++)
        T[y] = (S[y] == S[y - 1]) ? T[y - 1] : m++;
    for (x = 1; x < w; x++) {
        z = x * h;
        T[z] = (S[z - h] == S[z]) ? T[z - h] : m++;
        for (y = 1; y < h; y++) {
            z = y + x * h;
            T[z] = S[z - 1] == S[z] ? T[z - 1] : S[z - h] == S[z] ? T[z - h] : m++;
            if (T[z - 1] != T[z - h] && S[z - 1] == S[z] && S[z - h] == S[z]) {
                t1 = T[z - 1];
                while (map[t1])
                    t1 = map[t1];
                t2 = T[z - h];
                while (map[t2])
                    t2 = map[t2];
                if (t1 == t2)
                    continue;
                if (t1 < t2) {
                    t = T[z] = t1;
                    t1 = T[z - h];
                } else {
                    t = T[z] = t2;
                    t1 = T[z - 1];
                }
                while (map[t1]) {
                    t2 = map[t1];
                    map[t1] = t;
                    t1 = t2;
                }
                map[t1] = t;
            }
        }
    }
    uint32 m1 = 0;
    for (t = 1; t < m; t++)
        map[t] = map[t] ? map[map[t]] : m1++;
    for (x = 0; x < w * h; x++)
        S[x] = map[T[x]];
    delete[] T;
    delete[] map;
}

// add or remove superpixel boundaries (and make superpixels 1 or 0-indexed)
void boundaries(uint32 *S, uint32 h, uint32 w, float *E, bool add, uint32 nThreads) {
    // helper for getting local neighborhood
#define NEIGHBORS8(S)          \
uint32 *L = S + x * h + y;   \
int x0, y0, x1, y1;        \
x0 = (x == 0) ? 0 : -1;    \
x1 = (x == w - 1) ? 0 : 1; \
x0 *= h;                   \
x1 *= h;                   \
y0 = (y == 0) ? 0 : -1;    \
y1 = (y == h - 1) ? 0 : 1; \
uint32 N[8] = {L[x0], L[x1], L[y0], L[y1], L[x0 + y0], L[x0 + y1], L[x1 + y0], L[x1 + y1]};
    if (add) {
        // make S 1-indexed
        for (uint32 x = 0; x < w * h; x++)
            S[x]++;
        // add 4-connectivity boundary greedily
#ifdef USEOMP
        nThreads = min(nThreads, uint32(omp_get_max_threads()));
#pragma omp parallel for num_threads(nThreads)
#endif
        for (int xi = 0; xi < int(w); xi++)
            for (uint32 y = 0; y < h; y++) {
                uint32 x = xi, a = x * h + y, b = (x + 1) * h + y, c = x * h + (y + 1), s = S[a];
                if (s == 0)
                    continue;
                if (x < w - 1 && s != S[b] && S[b] > 0) {
                    if (E[a] > E[b])
                        S[a] = 0;
                    else
                        S[b] = 0;
                }
                if (y < h - 1 && s != S[c] && S[c] > 0) {
                    if (E[a] > E[c])
                        S[a] = 0;
                    else
                        S[c] = 0;
                }
            }
        // add 8-connectivity boundary
#ifdef USEOMP
#pragma omp parallel for num_threads(nThreads)
#endif
        for (int xi = 0; xi < int(w); xi++)
            for (uint32 y = 0; y < h; y++) {
                uint32 x = xi;
                NEIGHBORS8(S);
                uint32 i, s = L[0];
                if (s == 0)
                    continue;
                if (N[0] && N[1] && N[2] && N[3])
                    continue;
                for (i = 0; i < 8; i++)
                    if (N[i] && N[i] != s) {
                        L[0] = 0;
                        break;
                    }
            }
        // remove excess boundary pixels
#ifdef USEOMP
#pragma omp parallel for num_threads(nThreads)
#endif
        for (int xi = 0; xi < int(w); xi++)
            for (uint32 y = 0; y < h; y++) {
                uint32 x = xi;
                NEIGHBORS8(S);
                uint32 i, s = L[0];
                if (s != 0)
                    continue;
                for (i = 0; i < 8; i++)
                    if (N[i]) {
                        s = L[0] = N[i];
                        break;
                    }
                for (i = 0; i < 8; i++)
                    if (N[i] && N[i] != s) {
                        L[0] = 0;
                        break;
                    }
            }
    } else {
        // make S 0-indexed
        uint32 *T = new uint32[h * w];
        memcpy(T, S, h * w * sizeof(uint32));
#ifdef USEOMP
#pragma omp parallel for num_threads(nThreads)
#endif
        for (int xi = 0; xi < int(w); xi++)
            for (uint32 y = 0; y < h; y++) {
                uint32 x = xi;
                NEIGHBORS8(T);
                uint32 i, *s = &S[x * h + y];
                if (*s != 0)
                    continue;
                float *F = E + x * h + y;
                float e = 10;
                float Es[8] = {F[x0], F[x1], F[y0], F[y1], F[x0 + y0], F[x0 + y1], F[x1 + y0], F[x1 + y1]};
                for (i = 0; i < 8; i += 2) {
                    if (N[i + 1] && Es[i + 1] < e) {
                        *s = N[i + 1];
                        e = Es[i + 1];
                    }
                    if (N[i + 0] && Es[i + 0] < e) {
                        *s = N[i + 0];
                        e = Es[i + 0];
                    }
                }
            }
        for (uint32 x = 0; x < w * h; x++)
            if (S[x])
                S[x]--;
        delete[] T;
    }
}

// merge superpixels that are separated by a weak boundary
void merge(uint32 *S, uint32 h, uint32 w, float *E, float thr) {
    // compute m and min for each region
    uint32 x;
    uint32 m = 0;
    for (x = 0; x < w * h; x++)
        m = S[x] > m ? S[x] : m;
    m++;
    float *es = new float[m];
    for (x = 0; x < m; x++)
        es[x] = 1000;
    for (x = 0; x < w * h; x++)
        es[S[x]] = min(E[x], es[S[x]]);
    // check for regions to merge and compute label mapping
    uint32 *map = new uint32[m]();
    for (x = 0; x < w; x++)
        for (uint32 y = 0; y < h; y++)
            if (S[x * h + y] == 0) {
                uint32 i, j, s, s1, s2, k = 0, U[8];
                NEIGHBORS8(S);
                for (i = 0; i < 8; i++)
                    if ((s = N[i]) != 0) {
                        for (j = 0; j < k; j++)
                            if (s == U[j])
                                break;
                        if (j == k)
                            U[k++] = s;
                    }
                for (i = 0; i < k - 1; i++)
                    for (j = i + 1; j < k; j++) {
                        s1 = U[i];
                        while (map[s1])
                            s1 = map[s1];
                        s2 = U[j];
                        while (map[s2])
                            s2 = map[s2];
                        if (s1 != s2 && E[x * h + y] - min(es[s1], es[s2]) < thr) {
                            es[s1] = es[s2] = min(es[s1], es[s2]);
                            if (s1 < s2) {
                                s = s1;
                                s1 = U[j];
                            } else {
                                s = s2;
                                s1 = U[i];
                            }
                            while (map[s1]) {
                                s2 = map[s1];
                                map[s1] = s;
                                s1 = s2;
                            }
                            map[s1] = s;
                        }
                    }
            }
    // perform mapping and remove obsolete boundaries
    uint32 m1 = 1;
    for (uint32 s = 1; s < m; s++)
        map[s] = map[s] ? map[map[s]] : m1++;
    for (x = 0; x < w * h; x++)
        if (S[x])
            S[x] = map[S[x]];
    for (x = 0; x < w; x++)
        for (uint32 y = 0; y < h; y++)
            if (S[x * h + y] == 0) {
                uint32 i, s = 0;
                NEIGHBORS8(S);
                for (i = 0; i < 8; i++)
                    if (N[i]) {
                        s = N[i];
                        break;
                    }
                for (; i < 8; i++)
                    if (N[i] && N[i] != s)
                        break;
                if (i == 8)
                    S[x * h + y] = s;
            }
    delete[] es;
    delete[] map;
}

// compute visualization of superpixels
void visualize(float *V, uint32 *S, uint32 h, uint32 w, float *I, bool hasBnds) {
    uint32 i, z, n = h * w;
    uint32 m = 0;
    for (uint32 x = 0; x < w * h; x++)
        m = S[x] > m ? S[x] : m;
    m++;
    float *clrs = new float[m];
    uint32 *cnts = new uint32[n];
    for (i = 0; i < m; i++)
        cnts[i] = 0;
    for (i = 0; i < n; i++)
        cnts[S[i]]++;
    for (z = 0; z < 3; z++) {
        for (i = 0; i < m; i++)
            clrs[i] = 0;
        for (i = 0; i < n; i++)
            clrs[S[i]] += I[z * n + i];
        for (i = 0; i < m; i++)
            clrs[i] /= cnts[i];
        if (hasBnds)
            clrs[0] = 0;
        for (i = 0; i < n; i++)
            V[z * n + i] = clrs[S[i]];
    }
    delete[] clrs;
    delete[] cnts;
}

// compute affinities between all nearby superpixels
void affinities(float *A, uint32 *S, uint32 h, uint32 w, float *E, uint8 *segs, uint32 *dims, uint32 nThreads) {
    const uint32 g = dims[0], h1 = dims[2], w1 = dims[3], nTreesEval = dims[4];
    const uint32 stride = uint32(float(h) / float(h1) + .5f); // 步长
    uint32 m = 0;
    for (uint32 x = 0; x < w * h; x++)
        m = S[x] > m ? S[x] : m;
    m++;

    float *wts = new float[w * h];
    float *Sn = new float[m * m]();
    float *Sd = new float[m * m]();
    
    for (uint32 i = 0; i < w * h; i++)
        wts[i] = 1 / (1 + exp((E[i] - .05f) * 50.f));

#ifdef USEOMP
    nThreads = min(nThreads, uint32(omp_get_max_threads()));
#pragma omp parallel for num_threads(nThreads)
#endif
    
    for (int x = 0; x < int(w); x += stride)
    {
        int xi, x0, x1, y, yi, y0, y1, r = g / 2;
        uint32 i, j, s, m1, s1, t, lbl, ind, nTreesConst;
        uint32 *lookup, *lbls1;
        float *wts1;
        lookup = new uint32[m];
        lbls1 = new uint32[g * g];
        wts1 = new float[m * 25];
        for (y = 0; y < int(h); y += stride)
        {
            // create consecutively labeled copy of local label mask
            x0 = max(-r, -x);
            x1 = min(r, int(w) - x);
            y0 = max(-r, -y);
            y1 = min(r, int(h) - y);
            lookup[0] = S[(x + x0) * h + y + y0];
            wts1[0] = 0;
            j = 0;
            m1 = 1;
            nTreesConst = 0;
            for (xi = x0; xi < x1; xi++)
                for (yi = y0; yi < y1; yi++) {
                    lbl = S[(x + xi) * h + y + yi];
                    if (lbl == lookup[j]) {
                        i = j;
                    } else {
                        for (i = 0; i < m1; i++)
                            if (lookup[i] == lbl)
                                break;
                        if (i == m1) {
                            lookup[m1] = lbl;
                            wts1[m1] = 0;
                            m1++;
                        }
                    }
                    lbls1[(xi + r) * g + yi + r] = i;
                    j = i;
                    wts1[i] += wts[(x + xi) * h + y + yi];
                }
            
            // loop over nTreesEval segmentation masks
            for (t = 0; t < nTreesEval; t++) {
                ind = y / stride + x / stride * h1 + t * h1 * w1;
                const uint8 *seg = segs + ind * g * g;
                s1 = 0;

                // compute number of segments s1 in seg
                for (xi = x0; xi < x1; xi++)
                    for (yi = y0; yi < y1; yi++) {
                        i = (xi + r) * g + yi + r;
                        if (seg[i] > s1)
                            s1 = seg[i];
                    }
                s1++;
                if (s1 == 1)
                {
                    nTreesConst++;
                    continue;
                }
                for (i = m1; i < m1 * (s1 + 1); i++)
                    wts1[i] = 0;
                
                for (xi = x0; xi < x1; xi++)
                    for (yi = y0; yi < y1; yi++) {
                        i = (xi + r) * g + yi + r;
                        wts1[lbls1[i] + (seg[i] + 1) * m1] += wts[(x + xi) * h + y + yi];
                    }
                // update numerator of similarity matrix Sn
                for (s = 1; s <= s1; s++)
                    for (i = 0; i < m1; i++)
                        for (j = 0; j < m1; j++)
                            Sn[lookup[i] * m + lookup[j]] +=
                            wts1[i + s * m1] * wts1[j + s * m1];
            }
            // update Sn same way as Sd for skipped uniform patches
            if (nTreesConst)
                for (i = 0; i < m1; i++)
                    for (j = 0; j < m1; j++)
                        Sn[lookup[i] * m + lookup[j]] += wts1[i] * wts1[j] * nTreesConst;
            // update denominator of similarity matrix Sd
            for (i = 0; i < m1; i++)
                for (j = 0; j < m1; j++)
                    Sd[lookup[i] * m + lookup[j]] += wts1[i] * wts1[j] * nTreesEval;
        }
        delete[] lookup;
        delete[] lbls1;
        delete[] wts1;
    }
    // compute affinities matrix A
    for (uint32 s = 1; s < m; s++)
        for (uint32 t = 1; t < m; t++)
            if (Sd[s * m + t]) {
                float d = Sn[s * m + s] / Sd[s * m + s] / 2 + Sn[t * m + t] / Sd[t * m + t] / 2 -
                Sn[s * m + t] / Sd[s * m + t];
                A[(s - 1) * (m - 1) + (t - 1)] = 1 - max(0.f, d);
            }
    delete[] wts;
    delete[] Sn;
    delete[] Sd;
}

// compute superpixel edge strength given affinities matrix
void edges(float *E, uint32 *S, uint32 h, uint32 w, float *A) {
    uint32 x, y, x0, x1, xi, y0, y1, yi, i, j, s, n, ss[9];
    uint32 m = 0;
    
    for (x = 0; x < w * h; x++)
        m = S[x] > m ? S[x] : m;
    
    //
    for (x = 0; x < w; x++)
        for (y = 0; y < h; y++) {
            if (S[x * h + y] != 0)
                continue;
            n = 0;
            E[x * h + y] = .01f;
            x0 = (x == 0) ? 0 : x - 1;
            x1 = (x == w - 1) ? w - 1 : x + 1;
            y0 = (y == 0) ? 0 : y - 1;
            y1 = (y == h - 1) ? h - 1 : y + 1;
            for (xi = x0; xi <= x1; xi++)
                for (yi = y0; yi <= y1; yi++) {
                    s = S[xi * h + yi];
                    if (!s)
                        continue;
                    bool isnew = 1;
                    s--;
                    for (i = 0; i < n; i++)
                        if (s == ss[i]) {
                            isnew = 0;
                            break;
                        }
                    if (isnew)
                        ss[n++] = s;
                }
            for (i = 0; i < n; i++)
                for (j = i + 1; j < n; j++)
                    E[x * h + y] = max(1 - A[ss[i] * m + ss[j]], E[x * h + y]);
        }
}

Mat spDetectmain(string mode, Mat Smat, Mat Imat, Mat Emat, double *prm) {
    assert(mode == "sticky");
    assert(Imat.type() == CV_32FC3);
    assert(Emat.type() == CV_32FC1);
    assert(Smat.type() == CV_32SC1);
    
    int h = Smat.rows;
    int w = Smat.cols;
    Mat out(Smat.size(), CV_32SC1, Scalar(0));
    
    uint32 *S = new uint32[h * w * Smat.channels()]();
    float *I = new float[Imat.rows * Imat.cols * Imat.channels()]();
    float *E = new float[Emat.rows * Emat.cols * Emat.channels()]();
    
    getaddu(Smat, S);
    getadd<float>(Imat, I);
    getadd<float>(Emat, E);
    
    uint32 *T = new uint32[Smat.rows * Smat.cols];
    memcpy(T, S, h * w * sizeof(uint32));
    sticky(T, h, w, I, E, prm);
    relabel(T, h, w);
    fillmat<uint32>(T, out);

    // Delete parameters
    delete [] S;
    delete [] I;
    delete [] E;
    delete [] T;

    return out;
}

Mat spDetectmain(string mode, Mat Smat, Mat Emat, bool add, uint32 nThreads) {
    assert(mode == "boundaries");
    assert(Emat.empty() == 0 && Emat.type() == CV_32FC1);
    assert(Smat.empty() == 0 && Smat.type() == CV_32SC1);
    
    int h = Smat.rows;
    int w = Smat.cols;
    Mat out(Smat.size(), CV_32SC1, Scalar(0));
    
    uint32 *S = new uint32[h * w * Smat.channels()]();
    float *E = new float[Emat.rows * Emat.cols * Emat.channels()]();
    getadd<uint32>(Smat, S);
    getadd<float>(Emat, E);
    
    uint32 *T = new uint32[h * w]();
    memcpy(T, S, h * w * sizeof(uint32));
    
    boundaries(T, h, w, E, add, nThreads);
    fillmat<uint32>(T, out);

    // Delete parameters
    delete [] S;
    delete [] E;
    delete [] T;

    return out;
}

Mat spDetectmain(string mode, Mat Smat, Mat Emat, float thr) {
    assert(mode == "merge");
    assert(Emat.empty() == 0 && Emat.type() == CV_32FC1);
    assert(Smat.empty() == 0 && Smat.type() == CV_32SC1);
    
    uint32 h = Smat.rows;
    uint32 w = Smat.cols;
    Mat out(Smat.size(), CV_32SC1, Scalar(0));
    
    uint32 *S = new uint32[h * w * Smat.channels()]();
    float *E = new float[Emat.rows * Emat.cols * Emat.channels()]();
    getadd<uint32>(Smat, S);
    getadd<float>(Emat, E);
    
    uint32 *T = new uint32[h * w]();
    memcpy(T, S, h * w * sizeof(uint32));
    
    merge(T, h, w, E, thr);
    fillmat<uint32>(T, out);

    // Delete parameters
    delete [] S;
    delete [] E;
    delete [] T;

    return out;
}

Mat spDetectmain(string mode, Mat Smat, Mat Imat, bool hasBnds) {
    assert(mode == "visualize");
    assert(Imat.empty() == 0 && Imat.type() == CV_32FC3);
    assert(Smat.empty() == 0 && Smat.type() == CV_32SC1);
    
    uint32 h = Smat.rows;
    uint32 w = Smat.cols;
    Mat out(Smat.size(), CV_32SC3, Scalar(0));
    
    uint32 *S = new uint32[h * w * Smat.channels()]();
    float *I = new float[Imat.rows * Imat.cols * Imat.channels()]();
    getadd<uint32>(Smat, S);
    getadd<float>(Imat, I);
    
    float *V = new float[h * w * 3];
    visualize(V, S, h, w, I, hasBnds);
    fillmat<float>(V, out);

    // Delete parameters
    delete [] S;
    delete [] I;
    delete [] V;

    return out;
}

Mat spDetectmain(string mode, Mat Smat, Mat Emat, uint8* Segs, uint32* segDims, int nThreads) {
    
    assert(mode == "affinities");
    assert(Emat.empty() == 0 && Emat.type() == CV_32FC1);
    assert(Smat.empty() == 0 && Smat.type() == CV_32SC1);
    timeval t1_aff,t2_aff,t1_affmain,t2_affmain;
    uint32 h = Smat.rows;
    uint32 w = Smat.cols;
    
    uint32 *S = new uint32[h * w * Smat.channels()]();
    float *E = new float[Emat.rows * Emat.cols * Emat.channels()]();
    gettimeofday(&t1_aff, 0);
    getadd<uint32>(Smat, S);
    getadd<float>(Emat, E);
    int m = 0;
    for (int x = 0; x < w * h; x++)
        m = S[x] > m ? S[x] : m;
    Mat out(m, m, CV_32FC1, Scalar(0));
    
    float *A = new float[m * m]();
    
    gettimeofday(&t1_affmain, 0);
    affinities(A, S, h, w, E, Segs, segDims, nThreads);
    fillmat<float>(A, out);

    // Delete parameters
    delete [] S;
    delete [] E;
    delete [] A;

    return out;
}

Mat spDetectmain(string mode, Mat Smat, Mat Amat) {
    assert(mode == "edges");
    assert(Smat.empty() == 0 && Smat.type() == CV_32SC1);
    assert(Amat.empty() == 0 && Amat.type() == CV_32FC1);
    
    uint32 h = Smat.rows;
    uint32 w = Smat.cols;
    Mat out(Smat.size(), CV_32FC1, Scalar(0));
    
    uint32 *S = new uint32[h * w * Smat.channels()]();
    float *A = new float[Amat.rows * Amat.cols * Amat.channels()]();
    getadd<uint32>(Smat, S);
    getadd<float>(Amat, A);
    
    float *E = new float[h * w];
    for(int i = 0; i < h * w; i++)
    {
        E[i] = 0;
    }
    edges(E, S, h, w, A);
    fillmat<float>(E, out);

    // Delete parameters
    delete [] S;
    delete [] A;
    delete [] E;

    return out;
}
