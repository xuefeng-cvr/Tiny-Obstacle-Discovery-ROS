import argparse
import scipy.io as spio
import numpy as np
import bisect
import json
import cv2
import math


def str2bool(v):
    if isinstance(v, bool):
       return v
    if v.lower() in ('yes', 'true', 't', 'y', '1'):
        return True
    elif v.lower() in ('no', 'false', 'f', 'n', '0'):
        return False
    else:
        raise argparse.ArgumentTypeError('Boolean value expected.')


def loadmat(filename):
    '''
    this function should be called instead of direct spio.loadmat
    as it cures the problem of not properly recovering python dictionaries
    from mat files. It calls the function check keys to cure all entries
    which are still mat-objects
    '''
    data = spio.loadmat(filename, struct_as_record=False, squeeze_me=True)
    return _check_keys(data)


def _check_keys(dict):
    '''
    checks if entries in dictionary are mat-objects. If yes
    todict is called to change them to nested dictionaries
    '''
    for key in dict:
        if isinstance(dict[key], spio.matlab.mio5_params.mat_struct):
            dict[key] = _todict(dict[key])
    return dict


def _todict(matobj):
    '''
    A recursive function which constructs from matobjects nested dictionaries
    '''
    dict = {}
    for strg in matobj._fieldnames:
        elem = matobj.__dict__[strg]
        if isinstance(elem, spio.matlab.mio5_params.mat_struct):
            dict[strg] = _todict(elem)
        else:
            dict[strg] = elem
    return dict


def loadmat_data(filename):
    mat = loadmat(filename)
    data = {}
    for key in mat.keys():
        if key[0:2] != "__":
            data[key] = mat[key]

    return data


def stretchlim(img, tol_low=0.01, tol_high=0.99):
    """
    recast from matlab implementation
    takes in img in float32
    :param img: input image data np.array, dtype = fload32
    :param tol_low:
    :param tol_high:
    :return: stretched img
    """
    if str(img.dtype) == "uint8":
        nbins = 256
    else:
        nbins = 65536

    p = img.shape[2]

    if tol_low < tol_high:
        ilowhigh = np.zeros((2, p))
        for i in range(p):
            N = np.histogram(np.reshape(img[:, :, i], -1), nbins)[0]
            cdf = np.cumsum(N) / np.sum(np.sum(N))
            ilow = np.argwhere(cdf > tol_low)[0] + 1
            ihigh = np.argwhere(cdf >= tol_high)[0]
            if ilow == ihigh:
                ilowhigh[:, i] = np.reshape(np.array([1, nbins]), -1)
            else:
                ilowhigh[:, i] = np.reshape(np.array([ilow, ihigh]), -1)
        lowhigh = (ilowhigh - 1) / (nbins - 1)
    else:
        raise Exception("tol_low is greater than tol_high")

    return lowhigh


def imadjust_rgb(src, tol=1, vin=[0, 255], vout=(0, 255)):
    output = np.zeros(src.shape, dtype=src.dtype)
    for p in range(src.shape[2]):
        out = imadjust(src[:, :, p], tol=1, vin=[0, 255], vout=(0, 255))
        output[:, :, p] = out
    output[:, :, 0][np.where(output[:, :, 0] == 128)] = 127
    return output


def imadjust(src, tol=1, vin=[0, 255], vout=(0, 255)):
    '''
    src : input one-layer image (numpy array)
    tol : tolerance, from 0 to 100.
    vin  : src image bounds
    vout : dst image bounds
    return : output img
    '''
    assert len(src.shape) == 2, 'Input image should be 2-dims'

    tol = max(0, min(100, tol))

    if tol > 0:
        # Compute in and out limits
        # Histogram
        hist = np.histogram(src, bins=list(range(256)), range=(0, 255))[0]
        hist = np.concatenate((hist, np.array([0])))
        # Cumulative histogram
        cum = np.cumsum(hist)

        # Compute bounds
        total = src.shape[0] * src.shape[1]
        low_bound = total * tol / 100
        upp_bound = total * (100 - tol) / 100
        vin[0] = bisect.bisect_left(cum, low_bound)
        vin[1] = bisect.bisect_left(cum, upp_bound)

    # Stretching
    scale = (vout[1] - vout[0]) / (vin[1] - vin[0])
    vs = src-vin[0]
    vs[src<vin[0]]=0
    vd = vs*scale+0.5 + vout[0]
    vd[vd>vout[1]] = vout[1]
    dst = vd
    return dst


def instancePart(gt, instID, k):
    '''
    clip a rectangle region with the obstacle as the center
    '''
    yx = np.argwhere(gt == instID)
    y = yx[:, 0]
    x = yx[:, 1]

    xl = np.min(x)
    xh = np.max(x)
    yl = np.min(y)
    yh = np.max(y)
    w = xh - xl
    h = yh - yl
    if h > w:
        lenth = h
    else:
        lenth = w
    thresh = k * lenth
    if thresh < 100:
        thresh = 100
    
    # if object is small ,use small box, if object is large, use big box.
    if h < thresh:
        h = (thresh - h)/2
    else:
        h = thresh
    yh = yh + h
    yl = yl - h

    if w < thresh:
        w = (thresh - w)/2
    else:
        w = thresh
    xh = xh + w
    xl = xl - w

    yl = math.ceil(yl)
    yh = math.ceil(yh)
    xl = math.ceil(xl)
    xh = math.ceil(xh)

    if yl < 0:
        yl = 0
    if xl < 0:
        xl = 0
    if yh > gt.shape[0] - 1:
        yh = gt.shape[0] - 1
    if xh > gt.shape[1] - 1:
        xh = gt.shape[1] - 1

    # rect info
    rect = {}
    rect['yl'] = yl
    rect['xl'] = xl
    rect['yh'] = yh
    rect['xh'] = xh
    return rect


def drawProposals(bbox_top, top, img, gtJson_filepath):
    '''
    draw the boxes on the image, red indicates the top 10, blue indicates
    '''
    jsondata = json.load(open(gtJson_filepath,'r'))
    freespace = jsondata['objects'][0]['polygon']

    back = np.zeros(img.shape, dtype=np.uint8)
    freespace = np.array(freespace)
    cv2.fillPoly(back,[freespace],(0,0,255))

    for i in range(1, len(jsondata['objects'])-2):
        obstacle = jsondata['objects'][i]['polygon']
        obstacle = np.array(obstacle)
        cv2.fillPoly(back,[obstacle],(0,255,0))

    idx = (back==0)
    img_marked = cv2.addWeighted(img, 0.3, back, 0.7, 0)
    img_marked[idx] = img[idx]

    if top > bbox_top.shape[0]:
        top = bbox_top.shape[0]

    for i in range(10):
        cv2.rectangle(img_marked,(bbox_top[i, 1],bbox_top[i, 0]),(bbox_top[i, 3]+bbox_top[i, 1],bbox_top[i, 2]+bbox_top[i, 0]),(255,0,0),2)
    
    for i in range(10, top):
        cv2.rectangle(img_marked,(bbox_top[i, 1],bbox_top[i, 0]),(bbox_top[i, 3]+bbox_top[i, 1],bbox_top[i, 2]+bbox_top[i, 0]),(0,0,255),1)
    
    return img_marked


