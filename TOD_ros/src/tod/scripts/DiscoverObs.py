#!/usr/bin/env python3
#-*-coding:utf-8-*-
import rospy
import cv2
import numpy as np
import os
import tools.ros_np_multiarray as rnm
import tools.pbcvt as pbcvt
import scipy.io as sio
import time
from tod.srv import ReqImage
from tod.msg import CoData, IMG
from sklearn.ensemble import RandomForestRegressor
import joblib
from std_msgs.msg import String
from sensor_msgs.msg import Image
from rospy.numpy_msg import numpy_msg
from tools.toolbox import str2bool, loadmat_data, imadjust_rgb, drawProposals
from tools.OAOCC import compute_obsEdge_fastest, compute_obsEdge_fast_v4
from tools.bbox_extraction import get_proposal
from tools.box_proc import filt_boxes
from tools.obs_regression import compute_obsScore as pred
from tools.feature_extraction import *
from tools.get_params_LAF import get_params_LAF
from tools.get_datalists_LAF import get_datalists_LAF
from multiprocessing import Pool
from itertools import product
import psutil
import matplotlib.pyplot as plt

np.set_printoptions(formatter={'float': '{: 0.4f}'.format})

def log(strs):
    rospy.loginfo('[' + rospy.get_caller_id() + ']' + '\t' + strs)

class ObsDetector:
    def __init__(self, exp_path, exp_name, MODEL_PATH, save_fv, is_train, save_bbox=0, save_edge=0):
        self.codata_pub = rospy.Publisher("codatas", CoData, queue_size=10)
        self.request_img = rospy.ServiceProxy('request_image_test', ReqImage)
        self.exp_name = exp_name
        self.save_fv = save_fv
        self.save_bbox = save_bbox
        self.save_edge = save_edge

        self.modelname = 'OD_rfmodel_python_' + self.exp_name + '_50.m'
        self.VARS = get_params_LAF(is_train, save_fv, exp_name, exp_path, MODEL_PATH, self.modelname)

        self.Nlayer = self.VARS['region_LAF'].shape[0]
        log(' Detector initialized')
    
    def predict(self, feat, bbox):
        score = self.VARS['rf'].predict(feat)
        idx = -1
        return score, idx

    def gen_probmap(self, bbox_top, h_full, w_full):
        t = bbox_top[:,[0,1,2,3,5]]
        t = pbcvt.bbsnms(t,0.7,1000)
        prob_map = np.zeros((h_full,w_full))
        #accumulate proposal with the predicted IoU
        for i in range(int(np.shape(t)[0]*0.5)):
            w = np.int32(t[i,:])
            weight = t[i, 4]
            prob_map[w[0]-1:w[0]+w[2]-1, w[1]-1:w[1]+w[3]-1] = prob_map[w[0]-1:w[0]+w[2]-1, w[1]-1:w[1]+w[3]-1] + weight * np.ones((w[2],w[3]))
        return prob_map/np.max(prob_map)
        

    def detect(self, img, gtJson_filepath, sceneid,  imgid):
        # pre-process
        t_all = time.time()
        h_full = np.size(img, 0)
        w_full = np.size(img, 1)
        img_hsv = np.array(cv2.cvtColor(img, cv2.COLOR_RGB2HSV_FULL))/255
        hsv_integal = compute_feathsv_intgeral(img_hsv)
        # Edge Extraction
        t_edge = time.time()
        ucms = compute_obsEdge_fast_v4(img, self.VARS['mod_sed'], self.VARS['region_LAF'])
        np_ucms = np.array(ucms)
        time_edge = (time.time() - t_edge) * 1000
        log(' OAOCC : %04d_%04d  %f ms' %(sceneid,imgid,time_edge))

        bbox_all = np.array([])
        feat_all = np.array([])
        tl_olp = 0
        tl_filt = 0
        tl_feat = 0
        tl_all = 0
        for k in range(self.Nlayer):
            # obtain the proposal by Object-level Proposal
            t_1 = time.time()
            t_olp = time.time()
            bbox_olp, O = get_proposal(np_ucms[k], k, self.VARS['box_size_filter'], [self.VARS['region_LAF'][k,1], self.VARS['region_LAF'][k,0]])
            t_olp = (time.time() - t_olp) * 1000
            log(' OLP %d : %04d_%04d  %f ms' %(k, sceneid,imgid,t_olp))

            # filt the proposal with abnormal sizes
            t_filt = time.time()
            bbox_klayer, idx_filtBBox = filt_boxes(bbox_olp, self.VARS['box_size_filter'], [self.VARS['region_LAF'][k,1], self.VARS['region_LAF'][k,0]])
            t_filt = (time.time() - t_filt) * 1000

            # feature extraction
            bbox_klayer[:,0:2] = bbox_klayer[:,0:2] - 1
            t_feat = time.time()
            feat_klayer = compute_feature_20_v2_fast(bbox_klayer, np_ucms[k], img_hsv, h_full, w_full, self.VARS['region_LAF'][k,:], hsv_integal)
            t_feat = (time.time() - t_feat) * 1000
            log(' Feat %d : %04d_%04d  %f ms' %(k, sceneid, imgid, t_feat)) 

            # change the coordinate system to the original image
            bbox_klayer[:,0] = bbox_klayer[:,0] + self.VARS['region_LAF'][k,1]
            bbox_klayer[:,1] = bbox_klayer[:,1] + self.VARS['region_LAF'][k,0]
            if k == 0:
                bbox_all = bbox_klayer
                feat_all = feat_klayer
            else:
                bbox_all = np.r_[bbox_all, bbox_klayer]
                feat_all = np.r_[feat_all, feat_klayer]
            t_1 = (time.time() - t_1) * 1000
            tl_olp = tl_olp + t_olp
            tl_filt = tl_filt + t_filt
            tl_feat = tl_feat + t_feat
            tl_all = tl_all + t_1
        # predicting by random forest
        t_pre = time.time()
        class_scores,idx = self.predict(feat = feat_all, bbox = bbox_all)
        time_pre = (time.time() - t_pre) * 1000
        if idx != -1:
            bbox_predicted = bbox_all[idx,:]
            feat_predicted = feat_all[idx,:]
        else:
            bbox_predicted = bbox_all
            feat_predicted = feat_all
        ids = np.argsort(class_scores,axis=0)
        ids = ids[::-1]
        class_scores = class_scores[np.newaxis].T
        bbox_top = np.c_[bbox_predicted[ids,:],class_scores[ids,:]]
        bbox_top = bbox_top[np.where(bbox_top[:,-1] > 0.25),:].squeeze()

        # generate the results
        top_nms = pbcvt.bbsnms(bbox_top[:,[0,1,2,3,5]],0.7,1000)
        img_marked = drawProposals(top_nms,200,img,gtJson_filepath)
        img_marked = cv2.cvtColor(img_marked,cv2.COLOR_RGB2BGR)

        prob_map = self.gen_probmap(bbox_top, h_full, w_full)

        t_all = (time.time() - t_all) * 1000
        log(' Discover : %04d_%04d  %f ms' %(sceneid, imgid, t_all))

        times = np.array([tl_all, time_edge, tl_olp, tl_filt, tl_feat, time_pre])

        return img_marked, prob_map, feat_all, bbox_all, bbox_top, np_ucms[self.Nlayer-1], times

    def send_result(self,imgmsg):
        img = rnm.to_numpy_u8(imgmsg.data)
        gtJson_filepath = imgmsg.gtJson_filepath
        img_marked, probmap, feat_all, bbox_all, bbox_top, edge, times = self.detect(img, gtJson_filepath, imgmsg.sceneid, imgmsg.imgid)
        imgmarkmsg = CoData()
        imgmarkmsg.data = rnm.to_multiarray_f32(img_marked)
        imgmarkmsg.msglabel = 'img'
        imgmarkmsg.sceneid = imgmsg.sceneid
        imgmarkmsg.imgid = imgmsg.imgid
        probmapmsg = CoData()
        probmapmsg.data = rnm.to_multiarray_f32(probmap)
        probmapmsg.msglabel = 'prob_map'
        probmapmsg.sceneid = imgmsg.sceneid
        probmapmsg.imgid = imgmsg.imgid
        topsmsg = CoData()
        topsmsg.data = rnm.to_multiarray_f32(bbox_top)
        topsmsg.msglabel = 'bbtop'
        topsmsg.sceneid = imgmsg.sceneid
        topsmsg.imgid = imgmsg.imgid
        timesmsg = CoData()
        timesmsg.data = rnm.to_multiarray_f32(times)
        timesmsg.msglabel = 'times'
        timesmsg.sceneid = imgmsg.sceneid
        timesmsg.imgid = imgmsg.imgid
        self.codata_pub.publish(imgmarkmsg)
        self.codata_pub.publish(probmapmsg)
        self.codata_pub.publish(topsmsg)
        self.codata_pub.publish(timesmsg)
        log(' Result published: %04d_%04d' %(imgmsg.sceneid, imgmsg.imgid))
        if self.save_bbox == 1:
            bboxmsg = CoData()
            bboxmsg.data = rnm.to_multiarray_f32(bbox_all)
            bboxmsg.msglabel = 'bbox'
            bboxmsg.sceneid = imgmsg.sceneid
            bboxmsg.imgid = imgmsg.imgid
            self.codata_pub.publish(bboxmsg)
            log(' BBox published: %04d_%04d' %(imgmsg.sceneid, imgmsg.imgid))
        if self.save_fv == 1:
            featmsg = CoData()
            featmsg.data = rnm.to_multiarray_f32(feat_all)
            featmsg.msglabel = 'FeatVec'
            featmsg.sceneid = imgmsg.sceneid
            featmsg.imgid = imgmsg.imgid
            self.codata_pub.publish(featmsg)
            log(' Feature published: %04d_%04d' %(imgmsg.sceneid, imgmsg.imgid))
        if self.save_edge == 1:
            edgemsg = CoData()
            edgemsg.data = rnm.to_multiarray_f32(edge)
            edgemsg.msglabel = 'edge'
            edgemsg.sceneid = imgmsg.sceneid
            edgemsg.imgid = imgmsg.imgid
            self.codata_pub.publish(edgemsg)
            log(' Edge published: %04d_%04d' %(imgmsg.sceneid, imgmsg.imgid))
        try:
            self.request_img(1)
            log(' Request Next Image')
        except rospy.ServiceException as e:
            log(' Service call failed: %s' %(e))

if __name__ == '__main__':
    rospy.init_node('obs_detector')
    exp_path   = rospy.get_param('~exp_path')
    exp_name   = rospy.get_param('~exp_name')
    MODEL_PATH = rospy.get_param('~MODEL_PATH')
    save_fv    = rospy.get_param('~save_fv')
    is_train   = rospy.get_param('~is_train')
    save_bbox  = rospy.get_param('~save_bbox')
    save_edge  = rospy.get_param('~save_edge')
    od = ObsDetector(exp_path, exp_name, MODEL_PATH, save_fv, is_train, save_bbox, save_edge)
    rospy.Subscriber("img_rgb", IMG, od.send_result)
    log(' Node [obs_detector_tod] start')
    rospy.wait_for_service('request_image_test')
    od.request_img(1)
    rospy.spin()
