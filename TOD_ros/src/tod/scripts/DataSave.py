#!/usr/bin/env python3
#-*-coding:utf-8-*-
import rospy
import cv2
import os
import numpy as np
import scipy.io as sio
from std_msgs.msg import String
from tod.srv import ReqImage
from tod.msg import CoData
from rospy.numpy_msg import numpy_msg
from sensor_msgs.msg import Image
from tools.toolbox import str2bool, loadmat_data, imadjust_rgb
from tools.OAOCC import compute_obsEdge_fast_v4
from tools.obs_regression import compute_obsScore as pred
import tools.ros_np_multiarray as rnm
from tools.get_params_LAF import get_params_LAF
from tools.get_datalists_LAF import get_datalists_LAF

def log(strs):
    rospy.loginfo('[' + rospy.get_caller_id() + ']' + '\t' + strs)

class DataSaver:
    def __init__(self, exp_path, exp_name, MODEL_PATH, save_fv, is_train):
        self.VARS = get_params_LAF(is_train, save_fv, exp_name, exp_path, MODEL_PATH) 
        rospy.Subscriber("codatas", CoData, self.cb_save_codata)

    # callbacks
    def cb_save_codata(self,rd):
        if rd.msglabel == 'img':
            save_path = os.path.abspath(os.path.join(self.VARS['abspath_test'], self.VARS['relpath_test_result']))
            suffix = '_bbox.png'
        elif rd.msglabel == 'bbtop':
            save_path = os.path.abspath(os.path.join(self.VARS['abspath_test'], self.VARS['relpath_test_tops']))
            suffix = '_tops.mat'
        elif rd.msglabel == 'FeatVec':
            save_path = os.path.abspath(os.path.join(self.VARS['abspath_test'], self.VARS['relpath_test_FeatVec']))
            suffix = '_featvecs.mat'
        elif rd.msglabel == 'bbox':
            save_path = os.path.abspath(os.path.join(self.VARS['abspath_test'], self.VARS['relpath_test_allbox']))
            suffix = '_bbox.mat'
        elif rd.msglabel == 'edge':
            save_path = os.path.abspath(os.path.join(self.VARS['abspath_test'], self.VARS['relpath_test_allucms']))
            suffix = '_edge.mat'
        elif rd.msglabel == 'prob_map':
            save_path = os.path.abspath(os.path.join(self.VARS['abspath_test'], self.VARS['relpath_test_probmaps']))
            suffix = '_probmap.mat'
        elif rd.msglabel == 'times':
            save_path = os.path.abspath(os.path.join(self.VARS['abspath_test'], self.VARS['relpath_test_times']))
            suffix = '_times.mat'
        else:
            log(' Undefined Types for Saving Data')
            return
        data_np = rnm.to_numpy_f32(rd.data)
        save_abspath = os.path.join(save_path, '%04d_%04d%s'%(rd.sceneid, rd.imgid, suffix))
        if rd.msglabel == 'img':
            cv2.imwrite(save_abspath, data_np)
        else:
            sio.savemat(save_abspath,{rd.msglabel:data_np})
            

if __name__ == '__main__':
    rospy.init_node ('data_saver')
    exp_path   = rospy.get_param('~exp_path')
    exp_name   = rospy.get_param('~exp_name')
    MODEL_PATH = rospy.get_param('~MODEL_PATH')
    save_fv    = rospy.get_param('~save_fv')
    is_train   = rospy.get_param('~is_train')
    ds = DataSaver(exp_path, exp_name, MODEL_PATH, save_fv, is_train)
    log(' Node [data_saver_tod] start')
    rospy.spin()


