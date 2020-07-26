import os
import numpy as np
import scipy.io as sio
from tools.toolbox import str2bool, loadmat_data, imadjust_rgb
from PIL import Image

def get_datalists_LAF(VARS):
    '''
    using route infomation in VARS, generate datalists
    :return: list of all data
    '''
    imgslist=[]
    scene_img_list=[]
    gtLablelist=[]
    scene_gtLabel_list=[]
    gtInstlist=[]
    scene_gtInst_list=[]
    gtJsonlist=[]
    scene_gtJson_list=[]
    disparitylist=[]
    scene_disparity_list = []

    data_path=os.path.abspath(os.path.join(VARS['abspath_LAF'], VARS['relpath_IMG'], VARS['subdir']))
    gt_path=os.path.abspath(os.path.join(VARS['abspath_LAF'], VARS['relpath_GT'], VARS['subdir']))
    disp_path=os.path.abspath(os.path.join(VARS['abspath_LAF'], VARS['relpath_DISP'], VARS['subdir']))

    scenename=os.listdir(data_path)
    scenename.sort()
    is_folder=True
    for item in scenename:
        if not os.path.exists(os.path.abspath(os.path.join(data_path, item))):
            is_folder=False
            gt_folder=[]
            data_folder=[]
            disp_folder=[]

    for scene in scenename:
        if not is_folder:
            data_folder.append(os.path.join(data_path, scene))
            gt_folder.append(os.path.join(gt_path, scene))
            disp_folder.append(os.path.join(disp_path, scene))

        for name in os.listdir(os.path.join(data_path, scene)):
            if os.path.splitext(name)[1]=='.png':
                scene_img_list.append(name)

        for name in os.listdir(os.path.join(gt_path, scene)):
            if name.find(VARS['suffix_gtLabel'])!=-1:
                scene_gtLabel_list.append(name)
            elif name.find(VARS['suffix_gtJson'])!=-1:
                scene_gtJson_list.append(name)
            elif name.find(VARS['suffix_gtInst'])!=-1:
                scene_gtInst_list.append(name)

        scene_img_list.sort()
        scene_gtLabel_list.sort()
        scene_gtInst_list.sort()
        scene_gtJson_list.sort()
        imgslist.append(scene_img_list[:])
        gtLablelist.append(scene_gtLabel_list[:])
        gtInstlist.append(scene_gtInst_list[:])
        gtJsonlist.append(scene_gtJson_list[:])

        scene_img_list[:]=[]
        scene_gtJson_list[:]=[]
        scene_gtInst_list[:]=[]
        scene_gtLabel_list[:]=[]

    datalist={'imgslist': imgslist, 'gtLablelist': gtLablelist,
              'gtInstlist': gtInstlist, 'gtJsonlist': gtJsonlist,
              'disparitylist': disparitylist, 'scenelist': scenename}

    if not is_folder:
        datalist['img_folder']=data_folder
        datalist['gt_folder']=gt_folder
        datalist['disp_folder']=disp_folder

    return datalist























