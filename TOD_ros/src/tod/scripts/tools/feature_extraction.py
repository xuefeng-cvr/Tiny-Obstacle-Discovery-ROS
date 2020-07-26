import numpy as np
from multiprocessing import pool
from itertools import product
import tools.box_proc as u
import copy
import scipy.io as sio
import scipy.spatial.distance as dist
from scipy.stats import mode
import cv2
import time
import tools.pbcvt as pbcvt


def compute_feature_20_v2_fast(bboxs, ucm_reg, img_hsv, h_full, w_full, reg, integralhsv_whole):
    '''
    :param bboxs:   box_count * 5 (0:x,1:y,2:h,3:w,4:score)
    :param ucm_reg: ucms of reg region
    :param img_hsv: full hsv img
    :param h_full:  full h size of one image
    :param w_full:  full w size of one image
    :param reg:     (miny,minx,h,w)
    :param integralhsv_whole:   integral of full hsv img
    :return:
    '''
    bbox=bboxs.copy()
    h_reg = np.size(ucm_reg, 0)
    w_reg = np.size(ucm_reg, 1)
    bbox = u.align_boxes(bbox, 0, w_reg-1, 0, h_reg-1)
    windows = copy.deepcopy(bbox)
    windows[:, 0:2:1] = windows[:, 0:2:1] + windows[:, 2:4:1]/2
    integralhsv_reg = integralhsv_whole[reg[0]-1:reg[2] + reg[0], reg[1]-1:reg[3] + reg[1], :]
    hsv_reg = img_hsv[reg[0]-1:reg[2] + reg[0], reg[1]-1:reg[3] + reg[1], :]

    pb_feat = calMaxPb(windows, ucm_reg)
    bb_feat = calBBFeat(windows, reg, h_full, w_full)
    hsistd_feat = calHSVStd(windows, hsv_reg)
    hsi_feat = calHSVdist(bbox, integralhsv_reg)

    feat = np.c_[pb_feat, bb_feat, hsistd_feat, hsi_feat]
    return feat


def mode1D(a):
    vals, cnts = np.unique(a, return_counts=True)
    b,c=vals[cnts.argmax()], cnts.max()
    return vals[cnts.argmax()]


def calMaxPb(windows, ucm):
    r = windows[:, 0:4].copy()
    r[:, 0: 2] = r[:, 0: 2] - r[:, 2: 4] / 2  # minx, miny
    r[:, 2: 4] = r[:, 0: 2] + r[:, 2: 4] - 1  # maxx, maxy
    r[r[:, 2] > np.size(ucm, 1), 2] = np.size(ucm, 1)-1
    r[r[:, 3] > np.size(ucm, 0), 3] = np.size(ucm, 0)-1
    r_c = windows[:, 0:4].copy()
    r_c[:, 2: 4] = r_c[:, 2: 4] / 2  # h/2 w/2
    r_c[:, 0: 2] = r_c[:, 0: 2] - r_c[:, 2: 4] / 2  # minx, miny
    r_c[:, 2: 4] = r_c[:, 0: 2] + r_c[:, 2: 4] - 1  # maxx, maxy
    ucm_0=np.array(ucm>0,dtype=np.float64)
    r = np.round(r+0.00001).astype(np.int32)
    r_c = np.round(r_c+0.00001).astype(np.int32)
    b_count = np.size(windows, 0)

    skip = np.array([0.01, 0.0114, 0.0155, 0.024, 0.043, 0.08, 0.154, 0.27, 0.38, 0.55, 1],dtype=np.float64)
    r=np.array(r,dtype=int)
    r_c=np.array(r_c,dtype=int)
    ucm=np.array(ucm,dtype=np.float64)
    pb_feat=pbcvt.MaxPb(ucm,ucm_0,windows,r,r_c,skip)

    for i in range(b_count):
        ucmPatch = ucm[r[i, 1]:r[i, 3] + 1, r[i, 0]:r[i, 2] + 1]
        pb_feat[i, 0]=ucmPatch.max()
    
    pb_feat[np.isnan(pb_feat[:, 2]), 2] = 0
    pb_feat[np.isnan(pb_feat[:, 1]), 1] = 0
    pb_feat[np.isnan(pb_feat[:, 0]), 0] = 0

    return pb_feat


def Pbfeat(i,ucm,ucm_inte,edge_map_inte,r,r_c,window):
    ucmPatch = ucm[r[i, 1]:r[i, 3]+1, r[i, 0]:r[i, 2]+1]
    ucmPatch_no0 = ucmPatch.ravel()[np.flatnonzero(ucmPatch)]
    f1 = np.max(ucmPatch_no0)
    f2 = mode(ucmPatch_no0)[0][0]
    f3 = np.sum((ucmPatch_no0 == f2)+0) / np.size(ucmPatch_no0)
    f4 = u.compute_feat_byintmap(r_c[i, 0], r_c[i, 1], r_c[i, 2], r_c[i, 3], ucm_inte) / (
                window[2] * window[3] / 4)
    f5 = u.compute_feat_byintmap(r_c[i, 0], r_c[i, 1], r_c[i, 2], r_c[i, 3], edge_map_inte) / (
                window[2] * window[3] / 4)
    f6 = u.compute_feat_byintmap(r[i, 0], r[i, 1], r[i, 2], r[i, 3], ucm_inte) / (
                window[2] * window[3])
    f7 = u.compute_feat_byintmap(r[i, 0], r[i, 1], r[i, 2], r[i, 3], edge_map_inte) / (
                window[2] * window[3])
    if f3 is np.nan: f3 = 0
    if f2 is np.nan: f2 = 0
    if f1 is np.nan: f1 = 0
    return [f1,f2,f3,f4,f5,f6,f7]


def calBBFeat(windows, reg, h_img, w_img):
    b_count = np.size(windows, 0)
    minx = reg[0]
    miny = reg[1]
    bb_feat = np.zeros((b_count, 7))
    bb_feat[:, 0] = windows[:, 2] * windows[:, 3] / (h_img * w_img)
    bb_feat[:, 1] = windows[:, 2] / windows[:, 3]
    bb_feat[:, 2] = windows[:, 4]
    bb_feat[:, 3] = (windows[:, 0]) / w_img
    bb_feat[:, 4] = (windows[:, 1]) / h_img
    bb_feat[:, 5] = windows[:, 2] / w_img
    bb_feat[:, 6] = windows[:, 3] / h_img
    return bb_feat


def calHSVdist(bbox, integralhsv):
    b_count = np.size(bbox, 0)
    w = np.size(integralhsv, 1) - 1
    h = np.size(integralhsv, 0) - 1
    f_bbox = bbox.copy()
    b_bbox = np.zeros((b_count, 4))
    f_bbox[:, 2] = f_bbox[:, 0] + f_bbox[:, 2] - 1
    f_bbox[:, 3] = f_bbox[:, 1] + f_bbox[:, 3] - 1
    b_bbox[:, 0] = bbox[:, 0] - bbox[:, 2] / 2
    b_bbox[:, 1] = bbox[:, 1] - bbox[:, 3] / 2
    b_bbox[:, 2] = bbox[:, 0] + bbox[:, 2] + bbox[:, 2] / 2
    b_bbox[:, 3] = bbox[:, 1] + bbox[:, 3] + bbox[:, 3] / 2
    b_bbox[b_bbox[:, 0] < 0, 0] = 0
    b_bbox[b_bbox[:, 1] < 0, 1] = 0
    b_bbox[b_bbox[:, 2] > w-1, 2] = w-1
    b_bbox[b_bbox[:, 3] > h-1, 3] = h-1
    f_bbox = np.round(f_bbox+0.00001).astype(np.int)
    b_bbox = np.round(b_bbox+0.00001).astype(np.int)

    hsv_dist=pbcvt.HSVdist(f_bbox,b_bbox,integralhsv)
    hsv_dist[np.isnan(hsv_dist)] = 0
    return hsv_dist


def calHSVStd(window, imghsv):
    windows=window.copy()
    b_count = np.size(windows, 0)
    area = windows[:, 2] * windows[:, 3]
    windows[:, 0: 2] = windows[:, 0: 2] - windows[:, 2: 4] / 2
    windows[:,2]=windows[:,0]+windows[:,2]-1
    windows[:,3]=windows[:,1]+windows[:,3]-1
    hl = np.size(imghsv, 0)
    wl = np.size(imghsv, 1)
    windows[windows[:,0] < 0] = 0
    windows[windows[:,1] < 0] = 0
    windows[windows[:,2] >wl-1]=wl-1
    windows[windows[:,3] >hl-1]=hl-1
    windows=np.array(windows,dtype=np.int)

    H = imghsv[:, :, 0]
    S = imghsv[:, :, 1]
    V = imghsv[:, :, 2]
    H2=H**2
    S2=S**2
    V2=V**2

    hsv_std=pbcvt.hsi_std(H,S,V,H2,S2,V2,windows,area)
    hsv_std[:,0]=np.sqrt(hsv_std[:,0]/area)
    hsv_std[:,1]=np.sqrt(hsv_std[:,1]/area)
    hsv_std[:,2]=np.sqrt(hsv_std[:,2]/area)

    return hsv_std


def compute_feathsv_intgeral(hsv):
    edges_1 = [0, 15, 25, 45, 55, 80, 108, 140, 165, 190, 220, 255, 275, 290, 316, 330, 345, 361]
    edges_2 = [0, 0.0625, 0.1250, 0.1875, 0.2500, 0.3125, 0.3750, 0.4375, 0.5000, 0.5625, 0.6250, 0.6875, 0.7500,
               0.8125, 0.8750, 0.9375, 1.1]
    edges_1=np.array(edges_1,dtype=np.float64)
    edges_2=np.array(edges_2, dtype=np.float64)

    H = hsv[:, :, 0] * 360
    S = hsv[:, :, 1]
    V = hsv[:, :, 2]
    h = np.size(hsv, 0)
    w = np.size(hsv, 1)
    integral_hsv=pbcvt.compute_feathsv_intgeral(edges_1,edges_2,H,S,V)
    integral_hsv=np.array(integral_hsv,dtype=np.int32)
    return integral_hsv
