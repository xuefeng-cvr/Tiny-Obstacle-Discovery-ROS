#include "MathUtils.hpp"
#include "tbb/tbb.h"

using namespace tbb;

class ApplyFoo {
    int *const my_index;
    int h, w;
    uint32 *S, *changed;
    float *E, *I, *mus, *ns; 
    float sigx, sigc, sige, sigs;
public:
    void operator()( const blocked_range<size_t>& r) const {
        int *index = my_index;
        for( size_t i=r.begin(); i!=r.end(); ++i ) 
           {
                int xi = index[i];
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
                    changed[0]++;
                    ns[s]--;
                    ns[t]++;
                    S[x * h + y] = t;
                    for (j = 0; j < 5; j++)
                        mus[s * 5 + j] = (mus[s * 5 + j] * (ns[s] + 1) - vs[j]) / ns[s];
                    for (j = 0; j < 5; j++)
                        mus[t * 5 + j] = (mus[t * 5 + j] * (ns[t] - 1) + vs[j]) / ns[t];
                }
            }
           }
    }
    ApplyFoo( int index[], int ph, int pw,
    uint32 *pS, uint32 *pchanged,
    float *pE, float *pI, float *pmus, float *pns,
    float psigx, float psigc, float psige, float psigs) :
        my_index(index), h(ph), w(pw), S(pS), changed(pchanged), 
        E(pE), I(pI), mus(pmus), ns(pns), sigx(psigx), sigc(psigc), sige(psige), sigs(psigs)
    {}
};