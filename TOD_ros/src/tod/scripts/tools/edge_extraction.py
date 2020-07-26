#!/usr/bin/env python3
#-*-coding:utf-8-*-
import tools.pbcvt as pbcvt
import numpy as np
import scipy.io as sio
import time
import psutil
import os

def edgesDetect(img, model):
    # packing params into uintparams
    uintparams = []

    opts = model['opts']

    uintparams.append(opts['imWidth'])
    uintparams.append(opts['gtWidth'])
    uintparams.append(opts['nPos'])
    uintparams.append(opts['nNeg'])
    uintparams.append(2147483647)  # uintparams.append(opts['nImgs']) this is inf
    uintparams.append(opts['nTrees'])
    uintparams.append(opts['minCount'])
    uintparams.append(opts['minChild'])
    uintparams.append(opts['maxDepth'])
    uintparams.append(opts['nSamples'])
    uintparams.append(opts['nClasses'])
    uintparams.append(opts['nOrients'])
    uintparams.append(opts['grdSmooth'])
    uintparams.append(opts['chnSmooth'])
    uintparams.append(opts['simSmooth'])
    uintparams.append(opts['normRad'])
    uintparams.append(opts['shrink'])
    uintparams.append(opts['nCells'])
    uintparams.append(opts['rgbd'])
    uintparams.append(opts['stride'])
    uintparams.append(opts['multiscale'])
    uintparams.append(opts['sharpen'])
    uintparams.append(opts['nTreesEval'])
    uintparams.append(opts['nThreads'])
    uintparams.append(opts['nms'])
    uintparams.append(opts['seed'])
    uintparams.append(opts['useParfor'])
    uintparams.append(opts['nChns'])
    uintparams.append(opts['nChnFtrs'])
    uintparams.append(opts['nSimFtrs'])
    uintparams.append(opts['nTotFtrs'])
    uintparams.append(0)  # uintparams.append(opts['showtime'])
    uintparams.append(0)  # uintparams.append(opts['showboxnum'])

    uintparams = np.array(uintparams, dtype=np.int)
    str_param = ";".join((opts['discretize'], opts['split'], opts['modelDir'], opts['modelFnm'], opts['bsdsDir']))

    # converting dtype to match opencv mat
    model['fids'] = model['fids'].astype(np.int32)
    model['depth'] = model['depth'].astype(np.int32)
    model['child'] = model['child'].astype(np.int32)
    model['count'] = model['count'].astype(np.int32)
    model['eBnds'] = model['eBnds'].astype(np.int32)

    result = pbcvt.edges_detect(img, uintparams, opts['fracFtrs'], str_param,
        model['thrs'], model['fids'], model['child'], model['count'], model['depth'],
        model['segs'], model['nSegs'], model['eBins'], model['eBnds'])

    return result[0], result[1], result[2], result[3], result[4]


def spDetect(img, E, opts):
    Type = opts['type']
    param_ints = []
    param_floats = []

    param_ints.append(opts['nIter'])
    param_ints.append(opts['nThreads'])
    param_ints.append(opts['k'])
    param_ints.append(opts['bounds'])

    param_floats.append(opts['alpha'])
    param_floats.append(opts['beta'])
    param_floats.append(opts['merge'])

    param_ints = np.array(param_ints)
    result = pbcvt.sp_detect(img, E, Type, param_ints, opts['alpha'], opts['beta'], opts['merge'], opts['seed'])

    return result[0], result[1]


def spAffinities(S, E, segs, segDims, nThread):
    result = pbcvt.sp_affinities(S, E, segs, segDims, nThread)
    return result[0], result[1], result[2]


def combinedProcess(img, model, opts):
    # packing params into uintparams
    uintparams = []

    uintparams.append(model['opts']['imWidth'])
    uintparams.append(model['opts']['gtWidth'])
    uintparams.append(model['opts']['nPos'])
    uintparams.append(model['opts']['nNeg'])
    uintparams.append(2147483647)  # uintparams.append(model['opts']['nImgs']) this is inf
    uintparams.append(model['opts']['nTrees'])
    uintparams.append(model['opts']['minCount'])
    uintparams.append(model['opts']['minChild'])
    uintparams.append(model['opts']['maxDepth'])
    uintparams.append(model['opts']['nSamples'])
    uintparams.append(model['opts']['nClasses'])
    uintparams.append(model['opts']['nOrients'])
    uintparams.append(model['opts']['grdSmooth'])
    uintparams.append(model['opts']['chnSmooth'])
    uintparams.append(model['opts']['simSmooth'])
    uintparams.append(model['opts']['normRad'])
    uintparams.append(model['opts']['shrink'])
    uintparams.append(model['opts']['nCells'])
    uintparams.append(model['opts']['rgbd'])
    uintparams.append(model['opts']['stride'])
    uintparams.append(model['opts']['multiscale'])
    uintparams.append(model['opts']['sharpen'])
    uintparams.append(model['opts']['nTreesEval'])
    uintparams.append(model['opts']['nThreads'])
    uintparams.append(model['opts']['nms'])
    uintparams.append(model['opts']['seed'])
    uintparams.append(model['opts']['useParfor'])
    uintparams.append(model['opts']['nChns'])
    uintparams.append(model['opts']['nChnFtrs'])
    uintparams.append(model['opts']['nSimFtrs'])
    uintparams.append(model['opts']['nTotFtrs'])
    uintparams.append(0)  # uintparams.append(model['opts']['showtime'])
    uintparams.append(0)  # uintparams.append(model['opts']['showboxnum'])

    # opts
    uintparams.append(opts['nIter'])
    uintparams.append(opts['nThreads'])
    uintparams.append(opts['k'])
    uintparams.append(opts['bounds'])

    uintparams = np.array(uintparams, dtype=np.int)
    str_param = ";".join((model['opts']['discretize'], model['opts']['split'], model['opts']['modelDir'],
                          model['opts']['modelFnm'], model['opts']['bsdsDir'], opts['type']))

    # converting dtype to match opencv mat
    model['fids'] = model['fids'].astype(np.int32)
    model['depth'] = model['depth'].astype(np.int32)
    model['child'] = model['child'].astype(np.int32)
    model['count'] = model['count'].astype(np.int32)
    model['eBnds'] = model['eBnds'].astype(np.int32)

    param_floats = []
    param_floats.append(model['opts']['fracFtrs'])
    param_floats.append(opts['alpha'])
    param_floats.append(opts['beta'])
    param_floats.append(opts['merge'])
    param_floats = np.array(param_floats, dtype=np.double)

    result = pbcvt.combined(img, uintparams, param_floats, str_param,
                                model['thrs'], model['fids'], model['child'], model['count'], model['depth'],
                                model['segs'], model['nSegs'], model['eBins'], model['eBnds'], opts['seed'])
    return result[0], result[1], result[2]


