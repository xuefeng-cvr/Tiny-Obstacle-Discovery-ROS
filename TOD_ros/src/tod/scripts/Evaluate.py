#!/usr/bin/env python3
#-*-coding:utf-8-*-
import rospy
import os
import logging
import sys
sys.path.remove('/opt/ros/kinetic/lib/python2.7/dist-packages')
import cv2
from PIL import Image
from tod.srv import StartEval
from tools.compute_iou_v6 import compute_iou_v6
from tools.seg2proposal_withLabel import seg2proposal_withLabel
from tools.get_params_LAF import get_params_LAF
from tools.get_datalists_LAF import get_datalists_LAF
import time
import numpy as np
import scipy.io as sio

def log(strs):
    rospy.loginfo('[' + rospy.get_caller_id() + ']' + '\t' + strs)

class Evaluator:
    def __init__(self, exp_path, exp_name, MODEL_PATH, save_fv, is_train, save_path):
        self.VARS = get_params_LAF(is_train, save_fv, exp_name, exp_path, MODEL_PATH)
        self.datalist = get_datalists_LAF(self.VARS)

        self.top_list = os.path.abspath(os.path.join(self.VARS['abspath_test'], self.VARS['relpath_test_tops']))
        self.probmap_list = os.path.abspath(os.path.join(self.VARS['abspath_test'], self.VARS['relpath_test_probmaps']))
        self.gt_path = os.path.abspath(os.path.join(self.VARS['abspath_LAF'], self.VARS['relpath_GT'], self.VARS['subdir']))

        self.roc_path    = os.path.abspath(os.path.join(save_path, 'ROC'))
        self.recall_path = os.path.abspath(os.path.join(save_path, 'recall'))
        # self.roc_name    = 'ROC_' + exp_name + '.mat'
        # self.recall_name = 'Recall_' + exp_name + '.mat'

        if not os.path.exists(self.roc_path):
            os.makedirs(self.roc_path)
        if not os.path.exists(self.recall_path):
            os.makedirs(self.recall_path)
    
    def Func_evaluation_DR(self):
        D1_prop_num = np.arange(0, 1001, 5)  # The 1st dimension, save the threshold of the top proposals number
        D2_ious = np.arange(0.5, 1.01,0.05)  # The 2nd dimension, save the threshold of the IoU between proposal and groundtruth proposal
        recalls = np.array([])  # [prop_num x iou_num x inst_num] [k,l,j] Taking k top proposal, l iou, whether the j obstacle is discoverd?
        idx = np.array([])  # the image index of each obstacle.
        n = 0  # count the obstacle
        self.top_list = os.listdir(self.top_list)
        self.top_list.sort()
        for i in range(len(self.top_list)): #iterate all the file saving bounding boxes
            rospy.loginfo("Recall : " + str(i) + '-th image')
            top_name = self.top_list[i]
            sID = int(top_name[0:4])  # index of scene
            iID = int(top_name[5:9])  # index of image
            gt = np.array(Image.open(os.path.join(self.gt_path, self.datalist['scenelist'][sID], self.datalist['gtInstlist'][sID][iID])))
            top = sio.loadmat(os.path.abspath(os.path.join(self.VARS['abspath_test'], self.VARS['relpath_test_tops'], self.top_list[i])))["bbtop"]
            inst_num = max(np.shape(np.unique(gt[gt > 1000])))
            IoU, ignore = compute_iou_v6(top,
                                         gt)  # [obstacle_num x bbox_num]compute the iou between each proposal and each obstacle
            ignore, types = seg2proposal_withLabel(gt)
            types = types.reshape((-1))
            recall = np.zeros((len(D1_prop_num), len(D2_ious), inst_num))
            if np.size(top) > 0:
                for j in range(inst_num):  # loop different obstacle
                    if np.size(idx) == 0:
                        idx = np.array([sID, iID, types[j]])
                    else:
                        idx = np.row_stack((idx, np.array([sID, iID, types[j]])))
                    for k in range(len(D1_prop_num)):
                        for l in range(len(D2_ious)):
                            count = D1_prop_num[k]
                            if D1_prop_num[k] >= np.shape(IoU)[1]:
                                count = np.shape(IoU)[1]
                                recall[k, l, j] = np.any(IoU[j, 0:count] > D2_ious[l])
                            else:
                                recall[k, l, j] = np.any(IoU[j, 0:count + 1] > D2_ious[l])
            if np.size(recalls) == 0:
                recalls = recall
            else:
                recalls = np.concatenate((recalls, recall), axis=2)
            n = n + inst_num
        # print('recall.shap:',recalls.shape)
        recalls = np.array(recalls, dtype=np.uint8)
        idx = np.array(idx, dtype=np.uint16)
        sio.savemat(os.path.abspath(os.path.join(self.recall_path,self.VARS['recall_mat'])),{'recalls':recalls,'idx':idx})


    def Func_evaluation_roc(self):
        self.probmap_list = os.listdir(self.probmap_list)
        self.probmap_list.sort()
        img_count = len(self.probmap_list)
        threshs = np.arange(0, 1.001, 0.005)
        idx = np.arange(np.size(threshs))
        thr2idx = {}
        for i in range(len(threshs)):
            thr2idx[str(threshs[i])] = idx[i]

        TP = np.zeros((img_count, len(threshs)))
        FP = np.zeros((img_count, len(threshs)))
        GT_Obstacle = np.zeros((img_count, len(threshs)))
        GT_FreeSpace = np.zeros((img_count, len(threshs)))
        obs_count = 0
        for i in range(len(self.probmap_list)):
            start = time.time()
            name = self.probmap_list[i]
            sID = int(name[0:4])
            iID = int(name[5:9])
            gt = np.array(Image.open(os.path.join(self.gt_path, self.datalist['scenelist'][sID], self.datalist['gtInstlist'][sID][iID])))
            probmap = \
            sio.loadmat(os.path.abspath(os.path.join(self.VARS['abspath_test'], self.VARS['relpath_test_probmaps'], self.probmap_list[i])))[
                "prob_map"]

            gt_freeobs = gt >= 1000
            gt_free = gt == 1000
            gt_obs = gt > 1000
            gt_free = np.array(gt_free, dtype=np.uint8)
            gt_obs = np.array(gt_obs, dtype=np.uint8)
            # print(np.sum(gt_obs), np.sum(gt_free))
            for j in range(len(threshs)):
                idx = thr2idx[str(threshs[j])]
                res_obs = probmap > threshs[j]
                res_obs = np.array(res_obs, dtype=np.uint8)
                TP[i, idx] = np.sum((res_obs[:] + gt_obs[:]) == 2)
                FP[i, idx] = np.sum((res_obs[:] + gt_free[:]) == 2)
                GT_Obstacle[i, idx] = np.sum(gt_obs[:] == 1)
                GT_FreeSpace[i, idx] = np.sum(gt_free[:] == 1)
            end = time.time()
            end=time.time()
            rospy.loginfo("ROC : " + str(i) + '-th image')
        TPR = np.sum(TP, axis=0) / np.sum(GT_Obstacle, axis=0)
        FPR = np.sum(FP, axis=0) / np.sum(GT_FreeSpace, axis=0)
        sio.savemat(os.path.abspath(os.path.join(self.roc_path,self.VARS['roc_mat'])),{'TPR':TPR,'FPR':FPR})

    def start_eval(self, req):
        self.Func_evaluation_DR()
        self.Func_evaluation_roc()
        log('Node[evaluator_tod] end')


if __name__ == '__main__':
    try:
        # initialize
        rospy.init_node('evaluator')
        exp_path   = rospy.get_param('~exp_path')
        exp_name   = rospy.get_param('~exp_name')
        MODEL_PATH = rospy.get_param('~MODEL_PATH')
        save_fv    = rospy.get_param('~save_fv')
        is_train   = rospy.get_param('~is_train')
        save_path  = rospy.get_param('~save_path')
        evaluator = Evaluator(exp_path, exp_name, MODEL_PATH, save_fv, is_train, save_path)
        s  = rospy.Service('start_eval_srv', StartEval, evaluator.start_eval)
        log(' Node [evaluator_tod] start')
        rospy.spin()
    except rospy.ROSInterruptException:
        pass