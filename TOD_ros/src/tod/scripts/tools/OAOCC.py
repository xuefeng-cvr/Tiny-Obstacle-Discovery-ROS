from tools.edge_extraction import edgesDetect, spDetect, spAffinities
import numpy as np
import time
import scipy.io as sio
import matplotlib.pyplot as plt


def compute_obsEdge_fast_v2(img, model, freq_obs):
    model['opts']['nms'] = -1
    model['opts']['nThreads'] = 4
    model['opts']['multiscale'] = 0
    model['opts']['sharpen'] = 2

    opts = {"type": 'sticky', "nIter": 4,
            "nThreads": 4, "k": 1024,
            "alpha": 0.5000, "beta": 0.9000,
            "merge": 0, "bounds": 1, "seed": []}
    opts['seed'] = np.array(opts['seed'])

    Es = []
    ucms = []
    bb = np.asarray(freq_obs).copy()
    img_cropped = img[bb[3][0]: bb[3][0] + bb[3][2] + 1, bb[3][1]: bb[3][1] + bb[3][3] + 1, :]

    maxMag = len(freq_obs)
    Es_greatest, ignore, ignore, segs, segsDim = edgesDetect(img_cropped, model)

    for k in range(maxMag - 1):
        Es_temp = np.zeros_like(Es_greatest)
        c0 = bb[k][1] - bb[3][1]
        r0 = bb[k][0] - bb[3][0]
        c1 = c0 + bb[k][3]
        r1 = r0 + bb[k][2]
        Es_temp[r0: r1+1, c0: c1+1] = Es_greatest[r0: r1+1, c0: c1+1]
        Es.append(Es_temp)
    Es_greatest = Es[0] + Es[1] + Es[2] + Es_greatest
    Es.append(Es_greatest)
    sp, ignore = spDetect(img_cropped, Es_greatest, opts)

    ignore, ignore, edgeo = spAffinities(sp, Es_greatest, segs, segsDim, opts['nThreads'])
    ucms_greatest = edgeo.astype(np.float32)

    for k in range(maxMag - 1):
        c0 = bb[k][1] - bb[3][1]
        r0 = bb[k][0] - bb[3][0]
        c1 = c0 + bb[k][3]
        r1 = r0 + bb[k][2]
        ucms.append(ucms_greatest[r0: r1+1, c0: c1+1])
    ucms.append(ucms_greatest)
    return ucms


def compute_obsEdge_fast_v4(img, model, freq_obs):
    model['opts']['nms'] = 1
    model['opts']['nThreads'] = 4
    model['opts']['multiscale'] = 0
    model['opts']['sharpen'] = 2

    # set default param for opts
    opts = {"type": 'sticky', "nIter": 4,
            "nThreads": 4, "k": 512,
            "alpha": 0.5000, "beta": 0.9000,
            "merge": 0, "bounds": 1, "seed": []}
    opts['seed'] = np.array(opts['seed'])

    # custom setting
    opts['nThreads'] = 4
    opts['k'] = 1024
    opts['alpha'] = 0.5
    opts['beta'] = 0.9
    opts['merge'] = 0

    nlayer = len(freq_obs)
    imgs = []
    Es = []
    O = []
    sp = []
    segs = []
    segsDim = []
    ucms = []
    ignore = None

    bb = np.asarray(freq_obs).copy()
    bb[:, 2:4] = bb[:, 0:2] + bb[:, 2:4] - 1

    # splited process
    for k in range(nlayer):
        imgs.append(img[bb[k][0] - 1:bb[k][2], bb[k][1] - 1: bb[k][3], :])
        Es_temp, O_temp, ignore, segs_temp, segsDim_temp = edgesDetect(imgs[k], model)

        Es.append(Es_temp)
        O.append(Es_temp)
        segs.append(segs_temp)
        segsDim.append(segsDim_temp)
        if k != 0:
            x1 = bb[k - 1][1] - bb[k][1] + 1
            y1 = bb[k - 1][0] - bb[k][0] + 1
            x2 = bb[k - 1][3] - bb[k][1] + 1
            y2 = bb[k - 1][2] - bb[k][0] + 1
            Es[k][y1 - 1: y2, x1 - 1: x2] = Es[k][y1 - 1: y2, x1 - 1: x2] + Es[k-1]

    for k in range(nlayer):
        if k != nlayer - 1:
            c0 = freq_obs[k][1] - freq_obs[nlayer - 1][1] + 1
            r0 = freq_obs[k][0] - freq_obs[nlayer - 1][0] + 1
            c1 = c0 + freq_obs[k][3] - 1
            r1 = r0 + freq_obs[k][2] - 1
            Es[k] = Es[nlayer - 1][r0 - 1: r1, c0 - 1: c1]
        sp_temp, ignore = spDetect(imgs[k], Es[k], opts)
        sp.append(sp_temp)

        ignore, ignore, edgeo = spAffinities(sp[k], Es[k], segs[k], segsDim[k], opts['nThreads'])
        ucms.append(edgeo.astype(np.float32))
    return ucms


def compute_obsEdge_fastest(img, model, freq_obs):
    model['opts']['nms'] = -1
    model['opts']['nThreads'] = 4
    model['opts']['multiscale'] = 0
    model['opts']['sharpen'] = 2
    opts = {"type": 'sticky', "nIter": 4,
            "nThreads": 4, "k": 1024,
            "alpha": 0.5000, "beta": 0.9000,
            "merge": 0, "bounds": 1, "seed": []}
    opts['seed'] = np.array(opts['seed'])

    Es = []
    ucms = []
    bb = np.asarray(freq_obs).copy()
    img_regdetect = img[bb[3][0]-1: bb[3][0] + bb[3][2]-1, bb[3][1]-1: bb[3][1] + bb[3][3]-1, :]

    maxMag = len(freq_obs)
    Es_regdetect, ignore, ignore, segs, segsDim = edgesDetect(img_regdetect, model)

    for k in range(maxMag - 1):
        Es_temp = np.zeros_like(Es_regdetect)
        c0 = bb[k][1] - bb[3][1]
        r0 = bb[k][0] - bb[3][0]
        c1 = c0 + bb[k][3]
        r1 = r0 + bb[k][2]
        Es_temp[r0: r1+1, c0: c1+1] = Es_regdetect[r0: r1+1, c0: c1+1]
        Es.append(Es_temp)
    Es_regdetect = Es[0] + Es[1] + Es[2] + Es_regdetect
    sp, ignore = spDetect(img_regdetect, Es_regdetect, opts)
    Es_regdetect[Es_regdetect<0.0001] = 0
    ignore, ignore1, edgeo = spAffinities(sp, Es_regdetect, segs, segsDim, opts['nThreads'])

    ucms_greatest = edgeo.astype(np.float32)
    for k in range(maxMag - 1):
        c0 = bb[k][1] - bb[3][1]
        r0 = bb[k][0] - bb[3][0]
        c1 = c0 + bb[k][3]
        r1 = r0 + bb[k][2]
        # operation
        ucm = ucms_greatest[r0: r1, c0: c1].copy()
        ucms.append(ucm)
    ucms.append(ucms_greatest)
    return ucms


def calEdge(img, model):
    '''
    Obtain the initial edge map with the Structured Edge Detection Toolbox
    from "Fast Edge Detection Using Structured Forests" in PAMI 2015
    '''
    model['opts']['nms'] = -1
    model['opts']['nThreads'] = 4
    model['opts']['multiscale'] = 0
    model['opts']['sharpen'] = 2
    opts = {"type": 'sticky', "nIter": 4,
            "nThreads": 4, "k": 1024,
            "alpha": 0.5000, "beta": 0.9000,
            "merge": 0, "bounds": 1, "seed": []}
    opts['seed'] = np.array(opts['seed'])

    E, ignore, ignore, segs, segsDim = edgesDetect(img, model)
    S, ignore = spDetect(img, E, opts)
    ignore, ignore, edgMap = spAffinities(S, E, segs, segsDim, opts['nThreads'])

    return edgMap


