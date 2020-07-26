#!/usr/bin/env python3
#-*-coding:utf-8-*-
import rospy
import os
import logging
import cv2
import numpy as np
from tod.srv import ReqImage,ReqImageResponse
from tod.srv import StartEval
from tod.msg import CoData, IMG
from std_msgs.msg import String
from sensor_msgs.msg import Image
import tools.ros_np_multiarray as rnm
from tools.get_params_LAF import get_params_LAF
from tools.get_datalists_LAF import get_datalists_LAF

def log(strs):
    rospy.loginfo('[' + rospy.get_caller_id() + ']' + '\t' + strs)

class ImagePubliser:
    def __init__(self, exp_path, exp_name, MODEL_PATH, save_fv, is_train):
        self.image_pub = rospy.Publisher("img_rgb",IMG, queue_size=1)
        self.serv  = rospy.Service('request_image_test', ReqImage, self.send_image)
        log(' Image Publisher start')
        self.scene_id = 0
        self.img_id = 0
        self.VARS = get_params_LAF(is_train, save_fv, exp_name, exp_path, MODEL_PATH)
        self.datalist = get_datalists_LAF(self.VARS)
    
    def send_image(self,req):
        if self.scene_id >= len(self.datalist['imgslist']):
            rospy.wait_for_service('start_eval_srv')
            try:
                start_eval = rospy.ServiceProxy('start_eval_srv', StartEval)
                start_eval(1)
            except rospy.ServiceException as e:
                log('Service call failed: %s'%e)
            return
        imgmsg = IMG()
        scene_id = "%04d" % self.scene_id
        img_id = "%04d" % self.img_id
        prefix = str(scene_id) + "_" + str(img_id)
        logging.info("starting " + prefix)
        img_path = os.path.abspath(os.path.join(self.VARS['abspath_LAF'], self.VARS['relpath_IMG'], self.VARS['subdir']))
        gt_path = os.path.abspath(os.path.join(self.VARS['abspath_LAF'], self.VARS['relpath_GT'], self.VARS['subdir']))
        imgcv =  cv2.imread(os.path.join(img_path, self.datalist['scenelist'][self.scene_id], self.datalist['imgslist'][self.scene_id][self.img_id]), )
        imgcv = cv2.cvtColor(imgcv,cv2.COLOR_BGR2RGB)
        gtJson_filepath = os.path.join(gt_path, self.datalist['scenelist'][self.scene_id], self.datalist['gtJsonlist'][self.scene_id][self.img_id])
        imgmsg.data = rnm.to_multiarray_u8(np.array(imgcv))
        imgmsg.gtJson_filepath = gtJson_filepath
        imgmsg.msglabel = 'RGB_Image'
        imgmsg.sceneid = self.scene_id
        imgmsg.imgid = self.img_id
        self.image_pub.publish(imgmsg)

        log(' publish image : ' + self.datalist['imgslist'][self.scene_id][self.img_id])
        self.img_id = self.img_id + 1
        if len(self.datalist['imgslist'][self.scene_id]) <= self.img_id:
            self.img_id = 0
            self.scene_id = self.scene_id + 1
        return ReqImageResponse(1)


if __name__ == '__main__':
    try:
        # initialize
        rospy.init_node('data_loader')
        exp_path   = rospy.get_param('~exp_path')
        exp_name   = rospy.get_param('~exp_name')
        MODEL_PATH = rospy.get_param('~MODEL_PATH')
        save_fv    = rospy.get_param('~save_fv')
        is_train   = rospy.get_param('~is_train')
        IP = ImagePubliser(exp_path = exp_path, 
                           exp_name = exp_name, 
                           MODEL_PATH = MODEL_PATH, 
                           save_fv = save_fv, 
                           is_train=is_train)
        log(' Node [data_loader_tod] start')
        rospy.spin()
    except rospy.ROSInterruptException:
        pass
