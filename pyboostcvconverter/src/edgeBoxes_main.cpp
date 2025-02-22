#include "edgeBoxes.hpp"

int clamp(int v, int a, int b) { return v < a ? a : v > b ? b : v; }


void EdgeBoxGenerator::generate(Boxes &boxes, arrayf &E, arrayf &O, arrayf &V, 
                                float ** regionY, int ** minmaxH, int ** minmaxW, 
                                int ** minmaxSZ, int reg_x, int reg_y)
{
    clusterEdges(E, O, V);
    prepDataStructs(E);
    scoreAllBoxes(boxes, regionY, minmaxH, minmaxW, minmaxSZ, reg_x, reg_y);
}

void EdgeBoxGenerator::clusterEdges(arrayf &E, arrayf &O, arrayf &V)
{
    int c, r, cd, rd, i, j;
    h = E._h;
    w = E._w;

    // greedily merge connected edge pixels into clusters (create _segIds)
    _segIds.init(h, w);
    _segCnt = 1;
    for (c = 0; c < w; c++)
        for (r = 0; r < h; r++)
        {
            if (c == 0 || r == 0 || c == w - 1 || r == h - 1 || E.val(c, r) <= _edgeMinMag)
                _segIds.val(c, r) = -1;
            else
                _segIds.val(c, r) = 0;
        }
    for (c = 1; c < w - 1; c++)
        for (r = 1; r < h - 1; r++)
        {
            if (_segIds.val(c, r) != 0)
                continue;
            float sumv = 0;
            int c0 = c, r0 = r;
            vectorf vs;
            vectori cs, rs;
            while (sumv < _edgeMergeThr)
            {
                _segIds.val(c0, r0) = _segCnt;
                float o0 = O.val(c0, r0), o1, v;
                bool found;
                for (cd = -1; cd <= 1; cd++)
                    for (rd = -1; rd <= 1; rd++)
                    {
                        if (_segIds.val(c0 + cd, r0 + rd) != 0)
                            continue;
                        found = false;
                        for (i = 0; i < cs.size(); i++)
                            if (cs[i] == c0 + cd && rs[i] == r0 + rd)
                            {
                                found = true;
                                break;
                            }
                        if (found)
                            continue;
                        o1 = O.val(c0 + cd, r0 + rd);
                        v = fabs(o1 - o0) / PI;
                        if (v > .5)
                            v = 1 - v;
                        vs.push_back(v);
                        cs.push_back(c0 + cd);
                        rs.push_back(r0 + rd);
                    }
                float minv = 1000;
                j = 0;
                for (i = 0; i < vs.size(); i++)
                    if (vs[i] < minv)
                    {
                        minv = vs[i];
                        c0 = cs[i];
                        r0 = rs[i];
                        j = i;
                    }
                sumv += minv;
                if (minv < 1000)
                    vs[j] = 1000;
            }
            _segCnt++;
        }

    // merge or remove small segments
    _segMag.resize(_segCnt, 0);
    for (c = 1; c < w - 1; c++)
        for (r = 1; r < h - 1; r++)
            if ((j = _segIds.val(c, r)) > 0)
                _segMag[j] += E.val(c, r);
    for (c = 1; c < w - 1; c++)
        for (r = 1; r < h - 1; r++)
            if ((j = _segIds.val(c, r)) > 0 && _segMag[j] <= _clusterMinMag)
                _segIds.val(c, r) = 0;
    i = 1;
    while (i > 0)
    {
        i = 0;
        for (c = 1; c < w - 1; c++)
            for (r = 1; r < h - 1; r++)
            {
                if (_segIds.val(c, r) != 0)
                    continue;
                float o0 = O.val(c, r), o1, v, minv = 1000;
                j = 0;
                for (cd = -1; cd <= 1; cd++)
                    for (rd = -1; rd <= 1; rd++)
                    {
                        if (_segIds.val(c + cd, r + rd) <= 0)
                            continue;
                        o1 = O.val(c + cd, r + rd);
                        v = fabs(o1 - o0) / PI;
                        if (v > .5)
                            v = 1 - v;
                        if (v < minv)
                        {
                            minv = v;
                            j = _segIds.val(c + cd, r + rd);
                        }
                    }
                _segIds.val(c, r) = j;
                if (j > 0)
                    i++;
            }
    }

    // compactify representation
    _segMag.assign(_segCnt, 0);
    vectori map(_segCnt, 0);
    _segCnt = 1;
    for (c = 1; c < w - 1; c++)
        for (r = 1; r < h - 1; r++)
            if ((j = _segIds.val(c, r)) > 0)
                _segMag[j] += E.val(c, r);
    for (i = 0; i < _segMag.size(); i++)
        if (_segMag[i] > 0)
            map[i] = _segCnt++;
    for (c = 1; c < w - 1; c++)
        for (r = 1; r < h - 1; r++)
            if ((j = _segIds.val(c, r)) > 0)
                _segIds.val(c, r) = map[j];

    // compute positional means and recompute _segMag
    _segMag.assign(_segCnt, 0);
    vectorf meanX(_segCnt, 0), meanY(_segCnt, 0);
    vectorf meanOx(_segCnt, 0), meanOy(_segCnt, 0), meanO(_segCnt, 0);
    for (c = 1; c < w - 1; c++)
        for (r = 1; r < h - 1; r++)
        {
            j = _segIds.val(c, r);
            if (j <= 0)
                continue;
            float m = E.val(c, r), o = O.val(c, r);
            _segMag[j] += m;
            meanOx[j] += m * cos(2 * o);
            meanOy[j] += m * sin(2 * o);
            meanX[j] += m * c;
            meanY[j] += m * r;
        }
    for (i = 0; i < _segCnt; i++)
        if (_segMag[i] > 0)
        {
            float m = _segMag[i];
            meanX[i] /= m;
            meanY[i] /= m;
            meanO[i] = atan2(meanOy[i] / m, meanOx[i] / m) / 2;
        }

    // compute segment affinities
    _segAff.resize(_segCnt);
    _segAffIdx.resize(_segCnt);
    for (i = 0; i < _segCnt; i++)
        _segAff[i].resize(0);
    for (i = 0; i < _segCnt; i++)
        _segAffIdx[i].resize(0);
    const int rad = 2;
    for (c = rad; c < w - rad; c++)
        for (r = rad; r < h - rad; r++)
        {
            int s0 = _segIds.val(c, r);
            if (s0 <= 0)
                continue;
            for (cd = -rad; cd <= rad; cd++)
                for (rd = -rad; rd <= rad; rd++)
                {
                    int s1 = _segIds.val(c + cd, r + rd);
                    if (s1 <= s0)
                        continue;
                    bool found = false;
                    for (i = 0; i < _segAffIdx[s0].size(); i++)
                        if (_segAffIdx[s0][i] == s1)
                        {
                            found = true;
                            break;
                        }
                    if (found)
                        continue;
                    float o = atan2(meanY[s0] - meanY[s1], meanX[s0] - meanX[s1]) + PI / 2;
                    float a = fabs(cos(meanO[s0] - o) * cos(meanO[s1] - o));
                    a = pow(a, _gamma);
                    _segAff[s0].push_back(a);
                    _segAffIdx[s0].push_back(s1);
                    _segAff[s1].push_back(a);
                    _segAffIdx[s1].push_back(s0);
                }
        }

    // compute _segC and _segR
    _segC.resize(_segCnt);
    _segR.resize(_segCnt);
    for (c = 1; c < w - 1; c++)
        for (r = 1; r < h - 1; r++)
            if ((j = _segIds.val(c, r)) > 0)
            {
                _segC[j] = c;
                _segR[j] = r;
            }

    // optionally create visualization (assume memory initialized is 3*w*h)
    if (V._x)
        for (c = 0; c < w; c++)
            for (r = 0; r < h; r++)
            {
                i = _segIds.val(c, r);
                V.val(c + w * 0, r) = i <= 0 ? 1 : ((123 * i + 128) % 255) / 255.0f;
                V.val(c + w * 1, r) = i <= 0 ? 1 : ((7 * i + 3) % 255) / 255.0f;
                V.val(c + w * 2, r) = i <= 0 ? 1 : ((174 * i + 80) % 255) / 255.0f;
            }
}

void EdgeBoxGenerator::prepDataStructs(arrayf &E)
{
    int c, r, i;

    // initialize step sizes
    _scStep = sqrt(1 / _alpha);
    _arStep = (1 + _alpha) / (2 * _alpha);
    _rcStepRatio = (1 - _alpha) / (1 + _alpha);

    // create _scaleNorm
    _scaleNorm.resize(10000);
    for (i = 0; i < 10000; i++)
        _scaleNorm[i] = pow(1.f / i, _kappa);

    // create _segIImg
    arrayf E1;
    E1.init(h, w);
    for (i = 0; i < _segCnt; i++)
        if (_segMag[i] > 0)
        {
            E1.val(_segC[i], _segR[i]) = _segMag[i];
        }
    _segIImg.init(h + 1, w + 1);
    for (c = 1; c < w; c++)
        for (r = 1; r < h; r++)
        {
            _segIImg.val(c + 1, r + 1) = E1.val(c, r) + _segIImg.val(c, r + 1) +
                                         _segIImg.val(c + 1, r) - _segIImg.val(c, r);
        }

    // create _magIImg
    _magIImg.init(h + 1, w + 1);
    for (c = 1; c < w; c++)
        for (r = 1; r < h; r++)
        {
            float e = E.val(c, r) > _edgeMinMag ? E.val(c, r) : 0;
            _magIImg.val(c + 1, r + 1) = e + _magIImg.val(c, r + 1) +
                                         _magIImg.val(c + 1, r) - _magIImg.val(c, r);
        }

    // create remaining data structures
    _hIdxs.resize(h);
    _hIdxImg.init(h, w);
    for (r = 0; r < h; r++)
    {
        int s = 0, s1;
        _hIdxs[r].push_back(s);
        for (c = 0; c < w; c++)
        {
            s1 = _segIds.val(c, r);
            if (s1 != s)
            {
                s = s1;
                _hIdxs[r].push_back(s);
            }
            _hIdxImg.val(c, r) = int(_hIdxs[r].size()) - 1;
        }
    }
    _vIdxs.resize(w);
    _vIdxImg.init(h, w);
    for (c = 0; c < w; c++)
    {
        int s = 0;
        _vIdxs[c].push_back(s);
        for (r = 0; r < h; r++)
        {
            int s1 = _segIds.val(c, r);
            if (s1 != s)
            {
                s = s1;
                _vIdxs[c].push_back(s);
            }
            _vIdxImg.val(c, r) = int(_vIdxs[c].size()) - 1;
        }
    }

    // initialize scoreBox() data structures
    int n = _segCnt + 1;
    _sWts.init(n, 1);
    _sDone.init(n, 1);
    _sMap.init(n, 1);
    _sIds.init(n, 1);
    for (i = 0; i < n; i++)
        _sDone.val(0, i) = -1;
    _sId = 0;
}

void EdgeBoxGenerator::scoreBox(Box &box)
{
    int i, j, k, q, bh, bw, r0, c0, r1, c1, r0m, r1m, c0m, c1m;
    float *sWts = _sWts._x;
    int sId = _sId++;
    int *sDone = _sDone._x, *sMap = _sMap._x, *sIds = _sIds._x;

    // add edge count inside box
    r1 = clamp(box.r + box.h, 0, h - 1);
    r0 = box.r = clamp(box.r, 0, h - 1);
    c1 = clamp(box.c + box.w, 0, w - 1);
    c0 = box.c = clamp(box.c, 0, w - 1);
    bh = box.h = r1 - box.r;
    bh /= 2;
    bw = box.w = c1 - box.c;
    bw /= 2;
    float v = _segIImg.val(c0, r0) + _segIImg.val(c1 + 1, r1 + 1) - _segIImg.val(c1 + 1, r0) - _segIImg.val(c0, r1 + 1);
    // subtract middle quarter of edges
    r0m = r0 + bh / 2;
    r1m = r0m + bh;
    c0m = c0 + bw / 2;
    c1m = c0m + bw;
    v -= _magIImg.val(c0m, r0m) + _magIImg.val(c1m + 1, r1m + 1) - _magIImg.val(c1m + 1, r0m) - _magIImg.val(c0m, r1m + 1);
    // short circuit computation if impossible to score highly
    float norm = _scaleNorm[bw + bh];
    box.s = v * norm;
    if (box.s < _minScore)
    {
        box.s = 0;
        return;
    }
    // find interesecting segments along four boundaries
    int cs, ce, rs, re, n = 0;
    cs = _hIdxImg.val(c0, r0);
    ce = _hIdxImg.val(c1, r0); // top
    for (i = cs; i <= ce; i++)
        if ((j = _hIdxs[r0][i]) > 0 && sDone[j] != sId)
        {
            sIds[n] = j;
            sWts[n] = 1;
            sDone[j] = sId;
            sMap[j] = n++;
        }
    cs = _hIdxImg.val(c0, r1);
    ce = _hIdxImg.val(c1, r1); // bottom
    for (i = cs; i <= ce; i++)
        if ((j = _hIdxs[r1][i]) > 0 && sDone[j] != sId)
        {
            sIds[n] = j;
            sWts[n] = 1;
            sDone[j] = sId;
            sMap[j] = n++;
        }
    rs = _vIdxImg.val(c0, r0);
    re = _vIdxImg.val(c0, r1); // left
    for (i = rs; i <= re; i++)
        if ((j = _vIdxs[c0][i]) > 0 && sDone[j] != sId)
        {
            sIds[n] = j;
            sWts[n] = 1;
            sDone[j] = sId;
            sMap[j] = n++;
        }
    rs = _vIdxImg.val(c1, r0);
    re = _vIdxImg.val(c1, r1); // right
    for (i = rs; i <= re; i++)
        if ((j = _vIdxs[c1][i]) > 0 && sDone[j] != sId)
        {
            sIds[n] = j;
            sWts[n] = 1;
            sDone[j] = sId;
            sMap[j] = n++;
        }
    // follow connected paths and set weights accordingly (w=1 means remove)
    for (i = 0; i < n; i++)
    {
        float w = sWts[i];
        j = sIds[i];
        for (k = 0; k < int(_segAffIdx[j].size()); k++)
        {
            q = _segAffIdx[j][k];
            float wq = w * _segAff[j][k];
            if (wq < .05f)
                continue; // short circuit for efficiency
            if (sDone[q] == sId)
            {
                if (wq > sWts[sMap[q]])
                {
                    sWts[sMap[q]] = wq;
                    i = min(i, sMap[q] - 1);
                }
            }
            else if (_segC[q] >= c0 && _segC[q] <= c1 && _segR[q] >= r0 && _segR[q] <= r1)
            {
                sIds[n] = q;
                sWts[n] = wq;
                sDone[q] = sId;
                sMap[q] = n++;
            }
        }
    }
    // finally remove segments connected to boundaries
    for (i = 0; i < n; i++)
    {
        k = sIds[i];
        if (_segC[k] >= c0 && _segC[k] <= c1 && _segR[k] >= r0 && _segR[k] <= r1)
            v -= sWts[i] * _segMag[k];
    }
    v *= norm;
    if (v < _minScore)
        v = 0;
    box.s = v;
}


void EdgeBoxGenerator::refineBox(Box &box)
{
    int rStep = int(box.h * _rcStepRatio);
    int cStep = int(box.w * _rcStepRatio);
    while (1)
    {
        // prepare for iteration
        rStep /= 2;
        cStep /= 2;
        if (rStep <= 2 && cStep <= 2)
            break;
        rStep = max(1, rStep);
        cStep = max(1, cStep);
        Box B;
        // search over r start
        B = box;
        B.r = box.r - rStep;
        B.h = B.h + rStep;
        scoreBox(B);
        if (B.s <= box.s)
        {
            B = box;
            B.r = box.r + rStep;
            B.h = B.h - rStep;
            scoreBox(B);
        }
        if (B.s > box.s)
            box = B;
        // search over r end
        B = box;
        B.h = B.h + rStep;
        scoreBox(B);
        if (B.s <= box.s)
        {
            B = box;
            B.h = B.h - rStep;
            scoreBox(B);
        }
        if (B.s > box.s)
            box = B;
        // search over c start
        B = box;
        B.c = box.c - cStep;
        B.w = B.w + cStep;
        scoreBox(B);
        if (B.s <= box.s)
        {
            B = box;
            B.c = box.c + cStep;
            B.w = B.w - cStep;
            scoreBox(B);
        }
        if (B.s > box.s)
            box = B;
        // search over c end
        B = box;
        B.w = B.w + cStep;
        scoreBox(B);
        if (B.s <= box.s)
        {
            B = box;
            B.w = B.w - cStep;
            scoreBox(B);
        }
        if (B.s > box.s)
            box = B;
    }
}

void EdgeBoxGenerator::drawBox(Box &box, arrayf &E, arrayf &V)
{
    // score box and draw color coded edges (red=out, green=in)
    int i, c, r;
    float e, o;
    if (!V._x)
        return;
    int sId = _sId;
    scoreBox(box);
    int c0, r0, c1, r1;
    r1 = clamp(box.r + box.h, 0, h - 1);
    r0 = box.r = clamp(box.r, 0, h - 1);
    c1 = clamp(box.c + box.w, 0, w - 1);
    c0 = box.c = clamp(box.c, 0, w - 1);
    for (c = 0; c < w; c++)
        for (r = 0; r < h; r++)
            V.val(c + w * 0, r) = V.val(c + w * 1, r) = V.val(c + w * 2, r) = 1;
    for (c = 0; c < w; c++)
        for (r = 0; r < h; r++)
        {
            i = _segIds.val(c, r);
            if (i <= 0)
                continue;
            e = E.val(c, r);
            o = (_sDone._x[i] == sId) ? _sWts._x[_sMap._x[i]] : (_segC[i] >= c0 && _segC[i] <= c1 && _segR[i] >= r0 && _segR[i] <= r1) ? 0 : 1;
            V.val(c + w * 0, r) = 1 - e + e * o;
            V.val(c + w * 1, r) = 1 - e * o;
            V.val(c + w * 2, r) = 1 - e;
        }
    // finally draw bounding box
    r = r0;
    for (c = c0; c <= c1; c++)
        V.val(c + w * 0, r) = V.val(c + w * 1, r) = V.val(c + w * 2, r) = 0;
    r = r1;
    for (c = c0; c <= c1; c++)
        V.val(c + w * 0, r) = V.val(c + w * 1, r) = V.val(c + w * 2, r) = 0;
    c = c0;
    for (r = r0; r <= r1; r++)
        V.val(c + w * 0, r) = V.val(c + w * 1, r) = V.val(c + w * 2, r) = 0;
    c = c1;
    for (r = r0; r <= r1; r++)
        V.val(c + w * 0, r) = V.val(c + w * 1, r) = V.val(c + w * 2, r) = 0;
}

void EdgeBoxGenerator::scoreAllBoxes(Boxes &boxes, float ** regionY, int ** minmaxH, int ** minmaxW, int ** minmaxSZ, int reg_x, int reg_y)
{
    boxes.resize(0);
    int arRad, scNum;
    float minSize = sqrt(_minBoxArea);
    arRad = int(log(_maxAspectRatio) / log(_arStep * _arStep));
    scNum = int(ceil(log(_maxBoxLength / minSize) / log(_scStep)));
    for (int s = 0; s < scNum; s++)
    {
        int a, r, c, bh, bw, kr, kc, bId = -1;
        float ar, sc;
        for (a = 0; a < 2 * arRad + 1; a++)
        {
            ar = pow(_arStep, float(a - arRad));
            sc = minSize * pow(_scStep, float(s));
            bh = int(sc / ar);
            kr = max(2, int(bh * _rcStepRatio));
            bw = int(sc * ar);
            kc = max(2, int(bw * _rcStepRatio));
            for (c = 0; c < w - bw + kc; c += kc)
                for (r = 0; r < h - bh + kr; r += kr)
                {
                    Box b;
                    b.r = r;
                    b.c = c;
                    b.h = bh;
                    b.w = bw;
                    // if(filt_bbox(b, regionY, minmaxH, minmaxW, minmaxSZ, reg_x, reg_y))
                    // {
                    //     boxes.push_back(b);
                    // }
                    boxes.push_back(b);
                }
        }
    }
    // score all boxes, refine top candidates, perform nms
    int i, k = 0, m = int(boxes.size());

    for (i = 0; i < m; i++)
    {
        scoreBox(boxes[i]);
        if (!boxes[i].s)
            continue;
        k++;
        refineBox(boxes[i]);
    }

    sort(boxes.rbegin(), boxes.rend(), boxesCompare);
    boxes.resize(k);
    boxesNms(boxes, _beta, _eta, _maxBoxes);
}

bool filt_bbox(Box &box, float ** regionY, int ** minmaxH, int ** minmaxW, int ** minmaxSZ, int reg_x, int reg_y)
{
    int r = box.r;
    int c = box.c;
    int h = box.h;
    int w = box.w;

    int area_thresh = 20;
    int center_x = (c + reg_x - 1) + w / 2;
    int center_y = (r + reg_y - 1) + h / 2;
    int area_box = h * w;

    for(int i = 0; i < 3; i++)
    {
        if(center_y > regionY[i][0] && center_y < regionY[i][1])
        {
            if(h < (minmaxH[i][1] + area_thresh * (i + 1)) && h > (minmaxH[i][0] - area_thresh * (i + 1)))
            {
                if(w < (minmaxW[i][1] + area_thresh * (i + 1)) && w > (minmaxW[i][0] - area_thresh * (i + 1)))
                {
                    if(area_box < (minmaxSZ[i][1] + area_thresh * (i + 1)) && area_box > minmaxSZ[i][0])
                    {
                        return true;
                    }
                    return false;
                }
                else
                    return false;

            }
            else
                return false;
        }
    }
    return false;
}

bool boxesCompare(const Box &a, const Box &b) { return a.s < b.s; }

float boxesOverlap(Box &a, Box &b)
{
    float areai, areaj, areaij;
    int r0, r1, c0, c1, r1i, c1i, r1j, c1j;
    r1i = a.r + a.h;
    c1i = a.c + a.w;
    if (a.r >= r1i || a.c >= c1i)
        return 0;
    r1j = b.r + b.h;
    c1j = b.c + b.w;
    if (a.r >= r1j || a.c >= c1j)
        return 0;
    areai = (float)a.w * a.h;
    r0 = max(a.r, b.r);
    r1 = min(r1i, r1j);
    areaj = (float)b.w * b.h;
    c0 = max(a.c, b.c);
    c1 = min(c1i, c1j);
    areaij = (float)max(0, r1 - r0) * max(0, c1 - c0);
    return areaij / (areai + areaj - areaij);
}



void boxesNms(Boxes &boxes, float thr, float eta, int maxBoxes)
{
    sort(boxes.rbegin(), boxes.rend(), boxesCompare);
    if (thr > .99)
        return;
    const int nBin = 10000;
    const float step = 1 / thr, lstep = log(step);
    vector<Boxes> kept;
    kept.resize(nBin + 1);
    int i = 0, j, k, n = (int)boxes.size(), m = 0, b, d = 1;

    int compare = 0;

    int * minr = new int [nBin + 1];
    int * minc = new int [nBin + 1];
    int * maxr = new int [nBin + 1];
    int * maxc = new int [nBin + 1];
    for(int i = 0; i < nBin + 1; i++)
    {
        minr[i] = 10000;
        minc[i] = 10000;
        maxr[i] = -1;
        maxc[i] = -1;
    }

    int nThreads = 6;
#ifdef _OPENMP
    nThreads = min(nThreads, omp_get_max_threads());
#endif

    while (i < n && m < maxBoxes)
    {
        b = boxes[i].w * boxes[i].h;
        bool keep = 1;
        b = clamp(int(ceil(log(float(b)) / lstep)), d, nBin - d);
        
        for (int j = b - d; j <= b + d; j++)
        {
            if (!keep)
                break;
            if (minr[j] >= (boxes[i].r + boxes[i].h) || minc[j] >= (boxes[i].c + boxes[i].w))
                continue;
            if (maxr[j] <= boxes[i].r|| maxc[j] <= boxes[i].c)
                continue;
            if (kept[j].size() <= 0)
                continue;

            bool *a = new bool[kept[j].size()]();

            #pragma omp parallel for num_threads(nThreads)
            for (int k = 0; k < kept[j].size(); k++)
            {
                a[k] = boxesOverlap(boxes[i], kept[j][k]) <= thr;
            }
            compare += kept[j].size();

            for (int k = 0; k < kept[j].size(); k++)
                if (!a[k])
                {
                    keep = false;
                    break;
                }

            delete [] a;
        }
        if (keep)
        {
            kept[b].push_back(boxes[i]);
            m++;
            minr[b] = min(minr[b], boxes[i].r);
            minc[b] = min(minc[b], boxes[i].c);
            maxr[b] = max(maxr[b], (boxes[i].r + boxes[i].h));
            maxc[b] = max(maxc[b], (boxes[i].c + boxes[i].w));
        }
        i++;
        if (keep && eta < 1 && thr > .5)
        {
            thr *= eta;
            d = ceil(log(1 / thr) / lstep);
        }
    }

    boxes.resize(m);
    i = 0;
    for (j = 0; j < nBin; j++)
        for (k = 0; k < kept[j].size(); k++)
            boxes[i++] = kept[j][k];
    sort(boxes.rbegin(), boxes.rend(), boxesCompare);

    delete [] minr;
    delete [] minc;
    delete [] maxr;
    delete [] maxc;
}

////////////////////////////////////////////////////////////////////////////////

void EdgeBoxGenerator2::generate(Boxes &boxes, arrayf &E, arrayf &O, arrayf &V)
{
    clusterEdges(E, O, V);
    prepDataStructs(E);
    scoreAllBoxes(boxes);
}

void EdgeBoxGenerator2::scoreAllBoxes(Boxes &boxes)
{
    // score all boxes, refine top candidates, perform nms
    int i, k = 0, m = int(boxes.size());
    for (i = 0; i < m; i++)
    {
        scoreBox(boxes[i]);
    }
}

void EdgeBoxGenerator2::clusterEdges(arrayf &E, arrayf &O, arrayf &V)
{
    int c, r, cd, rd, i, j;
    h = E._h;
    w = E._w;

    // greedily merge connected edge pixels into clusters (create _segIds)
    _segIds.init(h, w);
    _segCnt = 1;

    for (c = 0; c < w; c++)
        for (r = 0; r < h; r++)
        {
            if (c == 0 || r == 0 || c == w - 1 || r == h - 1 || E.val(c, r) <= _edgeMinMag)
                _segIds.val(c, r) = -1;
            else
                _segIds.val(c, r) = 0;
        }
    for (c = 1; c < w - 1; c++)
        for (r = 1; r < h - 1; r++)
        {
            if (_segIds.val(c, r) != 0)
                continue;

            float sumv = 0;
            int c0 = c, r0 = r;
            vectorf vs;
            vectori cs, rs;

            while (sumv < _edgeMergeThr)
            {
                _segIds.val(c0, r0) = _segCnt;
                float o0 = O.val(c0, r0), o1, v;
                bool found;
                for (cd = -1; cd <= 1; cd++)
                    for (rd = -1; rd <= 1; rd++)
                    {
                        if (_segIds.val(c0 + cd, r0 + rd) != 0)
                            continue;
                        found = false;
                        for (i = 0; i < cs.size(); i++)
                            if (cs[i] == c0 + cd && rs[i] == r0 + rd)
                            {
                                found = true;
                                break;
                            }
                        if (found)
                            continue;
                        o1 = O.val(c0 + cd, r0 + rd);
                        v = fabs(o1 - o0) / PI;
                        if (v > .5)
                            v = 1 - v;
                        vs.push_back(v);
                        cs.push_back(c0 + cd);
                        rs.push_back(r0 + rd);
                    }
                float minv = 1000;
                j = 0;
                for (i = 0; i < vs.size(); i++)
                    if (vs[i] < minv)
                    {
                        minv = vs[i];
                        c0 = cs[i];
                        r0 = rs[i];
                        j = i;
                    }
                sumv += minv;
                if (minv < 1000)
                    vs[j] = 1000;
            }
            _segCnt++;
        }

    // merge or remove small segments
    _segMag.resize(_segCnt, 0);
    for (c = 1; c < w - 1; c++)
        for (r = 1; r < h - 1; r++)
            if ((j = _segIds.val(c, r)) > 0)
                _segMag[j] += E.val(c, r);
    for (c = 1; c < w - 1; c++)
        for (r = 1; r < h - 1; r++)
            if ((j = _segIds.val(c, r)) > 0 && _segMag[j] <= _clusterMinMag)
                _segIds.val(c, r) = 0;
    i = 1;
    while (i > 0)
    {
        i = 0;
        for (c = 1; c < w - 1; c++)
            for (r = 1; r < h - 1; r++)
            {
                if (_segIds.val(c, r) != 0)
                    continue;
                float o0 = O.val(c, r), o1, v, minv = 1000;
                j = 0;
                for (cd = -1; cd <= 1; cd++)
                    for (rd = -1; rd <= 1; rd++)
                    {
                        if (_segIds.val(c + cd, r + rd) <= 0)
                            continue;
                        o1 = O.val(c + cd, r + rd);
                        v = fabs(o1 - o0) / PI;
                        if (v > .5)
                            v = 1 - v;
                        if (v < minv)
                        {
                            minv = v;
                            j = _segIds.val(c + cd, r + rd);
                        }
                    }
                _segIds.val(c, r) = j;
                if (j > 0)
                    i++;
            }
    }

    // compactify representation
    _segMag.assign(_segCnt, 0);
    vectori map(_segCnt, 0);
    _segCnt = 1;
    for (c = 1; c < w - 1; c++)
        for (r = 1; r < h - 1; r++)
            if ((j = _segIds.val(c, r)) > 0)
                _segMag[j] += E.val(c, r);
    for (i = 0; i < _segMag.size(); i++)
        if (_segMag[i] > 0)
            map[i] = _segCnt++;
    for (c = 1; c < w - 1; c++)
        for (r = 1; r < h - 1; r++)
            if ((j = _segIds.val(c, r)) > 0)
                _segIds.val(c, r) = map[j];

    // compute positional means and recompute _segMag
    _segMag.assign(_segCnt, 0);
    vectorf meanX(_segCnt, 0), meanY(_segCnt, 0);
    vectorf meanOx(_segCnt, 0), meanOy(_segCnt, 0), meanO(_segCnt, 0);
    for (c = 1; c < w - 1; c++)
        for (r = 1; r < h - 1; r++)
        {
            j = _segIds.val(c, r);
            if (j <= 0)
                continue;
            float m = E.val(c, r), o = O.val(c, r);
            _segMag[j] += m;
            meanOx[j] += m * cos(2 * o);
            meanOy[j] += m * sin(2 * o);
            meanX[j] += m * c;
            meanY[j] += m * r;
        }
    for (i = 0; i < _segCnt; i++)
        if (_segMag[i] > 0)
        {
            float m = _segMag[i];
            meanX[i] /= m;
            meanY[i] /= m;
            meanO[i] = atan2(meanOy[i] / m, meanOx[i] / m) / 2;
        }

    // compute segment affinities
    _segAff.resize(_segCnt);
    _segAffIdx.resize(_segCnt);
    for (i = 0; i < _segCnt; i++)
        _segAff[i].resize(0);
    for (i = 0; i < _segCnt; i++)
        _segAffIdx[i].resize(0);
    const int rad = 2;
    for (c = rad; c < w - rad; c++)
        for (r = rad; r < h - rad; r++)
        {
            int s0 = _segIds.val(c, r);
            if (s0 <= 0)
                continue;
            for (cd = -rad; cd <= rad; cd++)
                for (rd = -rad; rd <= rad; rd++)
                {
                    int s1 = _segIds.val(c + cd, r + rd);
                    if (s1 <= s0)
                        continue;
                    bool found = false;
                    for (i = 0; i < _segAffIdx[s0].size(); i++)
                        if (_segAffIdx[s0][i] == s1)
                        {
                            found = true;
                            break;
                        }
                    if (found)
                        continue;
                    float o = atan2(meanY[s0] - meanY[s1], meanX[s0] - meanX[s1]) + PI / 2;
                    float a = fabs(cos(meanO[s0] - o) * cos(meanO[s1] - o));
                    a = pow(a, _gamma);
                    _segAff[s0].push_back(a);
                    _segAffIdx[s0].push_back(s1);
                    _segAff[s1].push_back(a);
                    _segAffIdx[s1].push_back(s0);
                }
        }

    // compute _segC and _segR
    _segC.resize(_segCnt);
    _segR.resize(_segCnt);
    for (c = 1; c < w - 1; c++)
        for (r = 1; r < h - 1; r++)
            if ((j = _segIds.val(c, r)) > 0)
            {
                _segC[j] = c;
                _segR[j] = r;
            }

    // optionally create visualization (assume memory initialized is 3*w*h)
    if (V._x)
        for (c = 0; c < w; c++)
            for (r = 0; r < h; r++)
            {
                i = _segIds.val(c, r);
                V.val(c + w * 0, r) = i <= 0 ? 1 : ((123 * i + 128) % 255) / 255.0f;
                V.val(c + w * 1, r) = i <= 0 ? 1 : ((7 * i + 3) % 255) / 255.0f;
                V.val(c + w * 2, r) = i <= 0 ? 1 : ((174 * i + 80) % 255) / 255.0f;
            }
}

void EdgeBoxGenerator2::prepDataStructs(arrayf &E)
{
    int c, r, i;

    // initialize step sizes
    _scStep = sqrt(1 / _alpha);
    _arStep = (1 + _alpha) / (2 * _alpha);
    _rcStepRatio = (1 - _alpha) / (1 + _alpha);
    _scaleNorm.resize(10000);
    for (i = 0; i < 10000; i++)
        _scaleNorm[i] = pow(1.f / i, _kappa);

    // create _segIImg
    arrayf E1;
    E1.init(h, w);
    for (i = 0; i < _segCnt; i++)
        if (_segMag[i] > 0)
        {
            E1.val(_segC[i], _segR[i]) = _segMag[i];
        }
    _segIImg.init(h + 1, w + 1);
    for (c = 1; c < w; c++)
        for (r = 1; r < h; r++)
        {
            _segIImg.val(c + 1, r + 1) = E1.val(c, r) + _segIImg.val(c, r + 1) +
                                         _segIImg.val(c + 1, r) - _segIImg.val(c, r);
        }

    // create _magIImg
    _magIImg.init(h + 1, w + 1);
    for (c = 1; c < w; c++)
        for (r = 1; r < h; r++)
        {
            float e = E.val(c, r) > _edgeMinMag ? E.val(c, r) : 0;
            _magIImg.val(c + 1, r + 1) = e + _magIImg.val(c, r + 1) +
                                         _magIImg.val(c + 1, r) - _magIImg.val(c, r);
        }

    // create remaining data structures
    _hIdxs.resize(h);
    _hIdxImg.init(h, w);
    for (r = 0; r < h; r++)
    {
        int s = 0, s1;
        _hIdxs[r].push_back(s);
        for (c = 0; c < w; c++)
        {
            s1 = _segIds.val(c, r);
            if (s1 != s)
            {
                s = s1;
                _hIdxs[r].push_back(s);
            }
            _hIdxImg.val(c, r) = int(_hIdxs[r].size()) - 1;
        }
    }
    _vIdxs.resize(w);
    _vIdxImg.init(h, w);
    for (c = 0; c < w; c++)
    {
        int s = 0;
        _vIdxs[c].push_back(s);
        for (r = 0; r < h; r++)
        {
            int s1 = _segIds.val(c, r);
            if (s1 != s)
            {
                s = s1;
                _vIdxs[c].push_back(s);
            }
            _vIdxImg.val(c, r) = int(_vIdxs[c].size()) - 1;
        }
    }

    // initialize scoreBox() data structures
    int n = _segCnt + 1;
    _sWts.init(n, 1);
    _sDone.init(n, 1);
    _sMap.init(n, 1);
    _sIds.init(n, 1);
    for (i = 0; i < n; i++)
        _sDone.val(0, i) = -1;
    _sId = 0;
}

void EdgeBoxGenerator2::scoreBox(Box &box)
{
    int i, j, k, q, bh, bw, r0, c0, r1, c1, r0m, r1m, c0m, c1m;
    float *sWts = _sWts._x;
    int sId = _sId++;
    int *sDone = _sDone._x, *sMap = _sMap._x, *sIds = _sIds._x;
    r1 = clamp(box.r + box.h, 0, h - 1); //(r0,c0),(r1,c1)
    r0 = box.r = clamp(box.r, 0, h - 1);
    c1 = clamp(box.c + box.w, 0, w - 1);
    c0 = box.c = clamp(box.c, 0, w - 1);
    bh = box.h = r1 - box.r;
    bh /= 2;
    bw = box.w = c1 - box.c;
    bw /= 2;
    float v1 = _segIImg.val(c0, r0) + _segIImg.val(c1 + 1, r1 + 1) - _segIImg.val(c1 + 1, r0) - _segIImg.val(c0, r1 + 1);
    // subtract middle quarter of edges
    float v2 = 0;
    r0m = r0 + bh / 2;
    r1m = r0m + bh;
    c0m = c0 + bw / 2;
    c1m = c0m + bw;
    v2 = _magIImg.val(c0m, r0m) + _magIImg.val(c1m + 1, r1m + 1) - _magIImg.val(c1m + 1, r0m) - _magIImg.val(c0m, r1m + 1);
    // short circuit computation if impossible to score highly
    float norm = _scaleNorm[bw + bh];
    box.s = (v1 - v2) * norm;
    if (box.s < _minScore)
    {
        box.s = 0;
        box.v[0] = 0;
        box.v[1] = 0;
        box.v[2] = 0;
        return;
    }
    int cs, ce, rs, re, n = 0;
    cs = _hIdxImg.val(c0, r0);
    ce = _hIdxImg.val(c1, r0);
    for (i = cs; i <= ce; i++)
        if ((j = _hIdxs[r0][i]) > 0 && sDone[j] != sId)
        {
            sIds[n] = j;
            sWts[n] = 1;
            sDone[j] = sId;
            sMap[j] = n++;
        }
    cs = _hIdxImg.val(c0, r1);
    ce = _hIdxImg.val(c1, r1);
    for (i = cs; i <= ce; i++)
        if ((j = _hIdxs[r1][i]) > 0 && sDone[j] != sId)
        {
            sIds[n] = j;
            sWts[n] = 1;
            sDone[j] = sId;
            sMap[j] = n++;
        }
    rs = _vIdxImg.val(c0, r0);
    re = _vIdxImg.val(c0, r1);
    for (i = rs; i <= re; i++)
        if ((j = _vIdxs[c0][i]) > 0 && sDone[j] != sId)
        {
            sIds[n] = j;
            sWts[n] = 1;
            sDone[j] = sId;
            sMap[j] = n++;
        }
    rs = _vIdxImg.val(c1, r0);
    re = _vIdxImg.val(c1, r1);
    for (i = rs; i <= re; i++)
        if ((j = _vIdxs[c1][i]) > 0 && sDone[j] != sId)
        {
            sIds[n] = j;
            sWts[n] = 1;
            sDone[j] = sId;
            sMap[j] = n++;
        }
    for (i = 0; i < n; i++)
    {
        float w = sWts[i];
        j = sIds[i];
        for (k = 0; k < int(_segAffIdx[j].size()); k++)
        {
            q = _segAffIdx[j][k];
            float wq = w * _segAff[j][k];
            if (wq < .05f)
                continue;
            if (sDone[q] == sId)
            {
                if (wq > sWts[sMap[q]])
                {
                    sWts[sMap[q]] = wq;
                    i = min(i, sMap[q] - 1);
                }
            }
            else if (_segC[q] >= c0 && _segC[q] <= c1 && _segR[q] >= r0 && _segR[q] <= r1)
            {
                sIds[n] = q;
                sWts[n] = wq;
                sDone[q] = sId;
                sMap[q] = n++;
            }
        }
    }
    float v3 = 0;
    for (i = 0; i < n; i++)
    {
        k = sIds[i];
        if (_segC[k] >= c0 && _segC[k] <= c1 && _segR[k] >= r0 && _segR[k] <= r1)
            v3 += sWts[i] * _segMag[k];
    }
    float vs = (v1 - v2 - v3) * norm;
    if (vs < _minScore)
        vs = 0;
    box.s = vs;
    box.v[0] = v1;
    box.v[1] = v2;
    box.v[2] = v3;
}


////////////////////////////////////////////////////////////////////////////////

Mat edgebox_main1(Mat E0, Mat O0, _para o, Mat regionY_origin, Mat minmaxH_origin, 
                    Mat minmaxW_origin, Mat minmaxSZ_origin, int reg_x, int reg_y)
{
    // check and get inputs
    arrayf E;
    arrayf O;
    int h = E0.rows;
    O._h = E._h = h;
    int w = E0.cols;
    O._w = E._w = w;
    E._x = new float[h * w];
    O._x = new float[h * w];
    getadd<float>(E0, E._x);
    getadd<float>(O0, O._x);

    //optionally create memory for visualization
    arrayf V;

    // setup and run EdgeBoxGenerator
    EdgeBoxGenerator edgeBoxGen;
    Boxes boxes;
    edgeBoxGen._alpha = o.alpha;
    edgeBoxGen._beta = o.beta;
    edgeBoxGen._minScore = o.minScore;
    edgeBoxGen._maxBoxes = o.maxBoxes;
    edgeBoxGen._maxAspectRatio = o.maxAspectRatio;
    edgeBoxGen._minBoxArea = o.minBoxArea;
    edgeBoxGen._kappa = o.kappa;
    // edgeBoxGen._eta = o.eta;
    // edgeBoxGen._edgeMinMag = o.edgeMinMag;
    // edgeBoxGen._edgeMergeThr = o.edgeMergeThr;
    // edgeBoxGen._clusterMinMag = o.clusterMinMag;
    // edgeBoxGen._maxBoxLength = std::min(std::min((int)o.maxBoxLength, h), w);
    // edgeBoxGen._gamma = o.gamma;

    edgeBoxGen._eta = 1;
    edgeBoxGen._edgeMinMag = 0.1;
    edgeBoxGen._edgeMergeThr = 0.5;
    edgeBoxGen._clusterMinMag = 0.5;
    edgeBoxGen._maxBoxLength = max(w,h);
    edgeBoxGen._gamma = 2;

    size_t sx = regionY_origin.rows;
    size_t sy = regionY_origin.cols; 
    float **regionY = init_array<float>(regionY_origin, sx, sy);
    
    sx = minmaxH_origin.rows;
    sy = minmaxH_origin.cols; 
    int **minmaxH = init_array<int>(minmaxH_origin, sx, sy);

    sx = minmaxW_origin.rows;
    sy = minmaxW_origin.cols; 
    int **minmaxW = init_array<int>(minmaxW_origin, sx, sy);

    sx = minmaxSZ_origin.rows;
    sy = minmaxSZ_origin.cols; 
    int **minmaxSZ = init_array<int>(minmaxSZ_origin, sx, sy);

    edgeBoxGen.generate(boxes, E, O, V, regionY, minmaxH, minmaxW, minmaxSZ, reg_x, reg_y);
    // boxesNms(boxes, edgeBoxGen._beta, edgeBoxGen._eta, edgeBoxGen._maxBoxes);

    // create output bbs and output to Matlab
    int n = (int)boxes.size();
    float *bbs = new float[n * 5];
    for (int i = 0; i < n; i++)
    {
        bbs[i + 0 * n] = (float)boxes[i].c + 1;
        bbs[i + 1 * n] = (float)boxes[i].r + 1;
        bbs[i + 2 * n] = (float)boxes[i].w;
        bbs[i + 3 * n] = (float)boxes[i].h;
        bbs[i + 4 * n] = boxes[i].s;
    }

    Mat bbs_mat(n, 5, CV_32FC1);
    fillmat(bbs, bbs_mat);

    delete [] E._x;
    delete [] O._x;
    delete [] bbs;

    return bbs_mat;
}

Mat edgebox_main2(Mat E0, Mat O0, _para o, float *bboxes_f, int num_box)
{
    // check and get inputs
    arrayf E;
    arrayf O;
    int h = E0.rows;
    O._h = E._h = h;
    int w = E0.cols;
    O._w = E._w = w;
    E._x = new float[h * w];
    O._x = new float[h * w];
    getadd<float>(E0, E._x);
    getadd<float>(O0, O._x);

    //optionally create memory for visualization
    arrayf V;

    // setup and run EdgeBoxGenerator
    EdgeBoxGenerator2 edgeBoxGen;
    Boxes boxes;
    edgeBoxGen._alpha = o.alpha;
    edgeBoxGen._beta = o.beta;
    edgeBoxGen._minScore = o.minScore;
    edgeBoxGen._maxBoxes = o.maxBoxes;
    edgeBoxGen._maxAspectRatio = o.maxAspectRatio;
    edgeBoxGen._minBoxArea = o.minBoxArea;
    edgeBoxGen._kappa = o.kappa;
    // edgeBoxGen._eta = o.eta;
    edgeBoxGen._edgeMinMag = o.edgeMinMag;
    edgeBoxGen._edgeMergeThr = o.edgeMergeThr;
    edgeBoxGen._clusterMinMag = o.clusterMinMag;
    // edgeBoxGen._maxBoxLength = std::min(std::min((int)o.maxBoxLength, h), w);
    edgeBoxGen._gamma = o.gamma;

    // edgeBoxGen._eta = 1;
    // edgeBoxGen._edgeMinMag = 0.1;
    // edgeBoxGen._edgeMergeThr = 0.5;
    // edgeBoxGen._clusterMinMag = 0.5;
    // edgeBoxGen._maxBoxLength = max(w,h);
    // edgeBoxGen._gamma = 2;
    

    // float *bboxes_f = (float *)mxGetData(pr[13]);
    // int num_box = mxGetM(pr[13]);

    // pl[0] = mxCreateNumericMatrix(num_box, 8, mxSINGLE_CLASS, mxREAL);
    // float *bbs = (float *)mxGetData(pl[0]);


    float *bbs = new float [num_box*8];

    for (int i = 0; i < num_box; i++)
    {
        //         std::cout<<bboxes[i]<<" "<<bboxes[i+num_box]<<" "<<bboxes[i+num_box*2]<<" "<<bboxes[i+num_box*3]<< " "<<num_box<<std::endl;
        Box b;
        b.c = int(bboxes_f[i]) - 1;
        b.r = int(bboxes_f[i + num_box]) - 1;
        b.w = int(bboxes_f[i + num_box * 2]);
        b.h = int(bboxes_f[i + num_box * 3]);
        boxes.push_back(b);
    }
    edgeBoxGen.generate(boxes, E, O, V);

    for (int i = 0; i <num_box; i++)
    {
        bbs[i + 0 * num_box] = (float)boxes[i].c + 1;
        bbs[i + 1 * num_box] = (float)boxes[i].r + 1;
        bbs[i + 2 * num_box] = (float)boxes[i].w;
        bbs[i + 3 * num_box] = (float)boxes[i].h;
        bbs[i + 4 * num_box] = boxes[i].s;
        bbs[i + num_box * 5] = boxes[i].v[0];
        bbs[i + num_box * 6] = boxes[i].v[1];
        bbs[i + num_box * 7] = boxes[i].v[2];
    }

    Mat bbs_mat(num_box, 8, CV_32FC1);
    fillmat(bbs, bbs_mat);

    delete [] E._x;
    delete [] O._x;
    delete [] bbs;
    
    return bbs_mat;
}
