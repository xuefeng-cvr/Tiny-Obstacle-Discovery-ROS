import os
import tools.pbcvt as pbcvt
import numpy as np
import scipy.io as sio
from tools.compute_iou_v5_withinte import compute_iou_v5_withinte
from tools.compute_iou import compute_iou, compute_iou_v2
import time
from sklearn.ensemble import RandomForestRegressor
from sklearn.externals import joblib
from PIL import Image
from tools.toolbox import str2bool, loadmat_data, imadjust_rgb, instancePart
from tools.OAOCC import compute_obsEdge_fastest, calEdge
from tools.bbox_extraction import get_proposal, calO
from tools.box_proc import filt_boxes
from tools.obs_regression import compute_obsScore as pred
from tools.feature_extraction import *
from tools.get_params_LAF import get_params_LAF
from tools.get_datalists_LAF import get_datalists_LAF
from multiprocessing import Pool
import gc

features20 = np.array([])
ious20 = np.array([])
filelog = np.array([])

is_train = True
is_save_fv = False
exp_dir = '/data/Experiment'
exp_name = '2020_07_08'
num_process = 5
MODEL_PATH = os.path.join(os.getcwd(), "Model")

def prepocess(img,gtLabel,gtInst,scene_idx, img_idx):
    '''
    edge detection and pre-processing
    '''
    global VARS
    tic = time.time()
    print("Start processing img", ' : %04d_%04d' % (scene_idx, img_idx))
    inst_id = np.unique(gtInst[gtInst > 1000])
    for k in range(inst_id.shape[0]):
        thresh = 15
        area_sampling = instancePart(gtInst, inst_id[k], thresh)
        imgPart = img[area_sampling['yl']:area_sampling['yh']+1,area_sampling['xl']:area_sampling['xh']+1,:]
        gtLabelPart = gtLabel[area_sampling['yl']:area_sampling['yh']+1,area_sampling['xl']:area_sampling['xh']+1]
        gtInstPart = gtInst[area_sampling['yl']:area_sampling['yh']+1,area_sampling['xl']:area_sampling['xh']+1]
        Eedge = calEdge(imgPart, VARS['mod_sed'])
        occ = Eedge

        sceneid = "%04d" % scene_idx
        imgid = "%04d" % img_idx
        prefix = str(sceneid) + "_" + str(imgid)+ '_' + str(k+1)

        sio.savemat(os.path.abspath(os.path.join(VARS['abspath_out'],
                                                VARS['relpath_out_preprocess'],prefix+'_preprocess.mat')),
                                                {'occ':occ,'imgPart':imgPart,'gtTypePart':gtLabelPart,
                                                'gtInstPart':gtInstPart,'rect': area_sampling})

    print("Prepocess", ' : %04d_%04d' % (scene_idx, img_idx), (time.time() - tic) * 1000, ' ms')

def calonepic(area_sampling, occ, img, gtClass, gtInst,  img_full, prefix):
    '''
    proposal and feature extraction
    '''
    global VARS
    print("Start calonepic : ", prefix)

    # compute bounding boxes by Object-level Proposal
    tOLP = time.time()
    bbox, O = get_proposal(occ, 0, VARS['box_size_filter'], [area_sampling['xl'], area_sampling['yl']])
    print('Bounding Boxes: OLP \t time:', (time.time() - tOLP) * 1000, 'ms')

    bbox_filter, idx_filtBBox = filt_boxes(bbox, VARS['box_size_filter'],
                                           [area_sampling['xl'], area_sampling['yl']])
    bbox_filter[:, 0:2] = bbox_filter[:, 0:2] - 1
    iou_fusion, overlap_freespace = compute_iou_v5_withinte(bbox_filter, gtInst)

    # compute iou of box of ground truth
    ist_id = np.unique(gtInst[gtInst > 1000])
    bbox_inst = np.zeros((ist_id.shape[0],5))    # re-score these bounding boxes
    for k in range(ist_id.shape[0]):
        yx = np.argwhere(gtInst == ist_id[k])
        y1 = yx[:, 0]
        x1 = yx[:, 1]

        maxX_s = np.max(x1) + 1
        if maxX_s >= np.shape(gtInst)[1]:
            maxX_s = np.shape(gtInst)[1] - 1

        maxY_s = np.max(y1) + 3
        if maxY_s >= np.shape(gtInst)[0]:
            maxY_s = np.shape(gtInst)[0] - 1

        minX_s = np.min(x1) - 2
        if minX_s < 0:
            minX_s = 0

        minY_s = np.min(y1) - 1
        if minY_s < 0:
            minY_s = 0

        h = maxY_s - minY_s
        w = maxX_s - minX_s

        box_sample = np.expand_dims(np.array([minX_s, minY_s, maxX_s - minX_s, maxY_s - minY_s]), axis=0)
        box_sample = box_sample.astype(np.float32)
        b = pbcvt.scoreboxesMex(occ, O, 0.65, 0.75, 0, 1e3, 0.1, 0.5, 0.5, 3, 1000, 2, 1.2, box_sample)
        b = np.squeeze(b)
        bbox_inst[k,:] = np.array([minX_s,minY_s,w,h,b[4]])
    
    iou_gt, ignore = compute_iou_v5_withinte(bbox_inst, gtInst)
    iou_gt = np.max(iou_gt, axis=0)

    # Training sample selection
    iou_desc = np.sort(-iou_fusion,axis=1)
    iou_desc = -iou_desc
    index_iou = np.argsort(-iou_fusion,axis=1)
    ids = index_iou[:,0:2]
    ids = ids.flatten('F')

    if len(ids) > n_samples:
        ids = ids[0:n_samples]
    
    idx_freespacebox = np.argwhere(overlap_freespace > 0.4)
    idx_backgroundbox = np.argwhere(overlap_freespace <= 0.4)

    ids_rest = np.setdiff1d(idx_freespacebox, ids)

    if iou_fusion.shape[1]-len(ids) > n_samples:
        if len(ids_rest) > n_samples-len(ids):
            ids_rest2 = ids_rest[np.random.choice(len(ids_rest), n_samples-len(ids), replace=False)]
        else:
            ids_rest2 = ids_rest[np.random.choice(len(ids_rest), len(ids_rest), replace=False)]

    sel_ids2 = np.concatenate((ids, ids_rest2),axis=0)
    bbox_ids2 = bbox_filter[sel_ids2, :]

    # feature extraction
    tfeature = time.time()
    h_full = np.shape(img_full)[0]
    w_full = np.shape(img_full)[1]
    img_hsv = np.array(cv2.cvtColor(img_full, cv2.COLOR_RGB2HSV_FULL)) / 255
    integral_hsv = compute_feathsv_intgeral(img_hsv)
    bbox_feat_all = np.r_[bbox_ids2, bbox_inst]
    reg = [area_sampling['yl']+1,area_sampling['xl']+1,area_sampling['yh']-area_sampling['yl'],area_sampling['xh']-area_sampling['xl']]

    feat = compute_feature_20_v2_fast(bbox_feat_all, occ, img_hsv, h_full, w_full, reg, integral_hsv)
    print('features \t time:', (time.time() - tfeature) * 1000, 'ms')

    # save training samples
    max_iou = np.max(iou_fusion,axis=0)
    trainD_20 = np.c_[feat, (np.r_[max_iou[sel_ids2],iou_gt])]

    sio.savemat(os.path.abspath(os.path.join(VARS['abspath_out'],
                                            VARS['relpath_out_trData'],prefix+'train_data_20D.mat')),
                                            {'feat':feat,'bbox':bbox_filter,'iou_fusion':iou_fusion,'trainD': trainD_20})

    gc.collect()


if __name__=='__main__':
    global VARS
    VARS = get_params_LAF(is_train, is_save_fv, exp_name, exp_dir, MODEL_PATH)
    datalist = get_datalists_LAF(VARS)

    # create Occlusion edges & initial training samples selection
    for scene_idx, scene in enumerate(datalist['imgslist']):
        length=len(scene)
        i=0    # i for controling the multi processes
        for img_idx, imgname in enumerate(scene):
            gc.collect()
            if i==0:
                pool=Pool(num_process)
            i=i+1
            sceneid="%04d"%scene_idx
            imgid="%04d"%img_idx
            prefix=str(sceneid)+"_"+str(imgid)+"_"+str(1)

            img_path=os.path.abspath(os.path.join(VARS['abspath_LAF'], VARS['relpath_IMG'], VARS['subdir']))
            gt_path=os.path.abspath(os.path.join(VARS['abspath_LAF'], VARS['relpath_GT'], VARS['subdir']))
            if not os.path.isfile(os.path.abspath(os.path.join(VARS['abspath_out'],
                                                               VARS['relpath_out_preprocess'],
                                                               prefix+"_preprocess.mat"))):
                img=np.array(Image.open(os.path.join(img_path, datalist['scenelist'][scene_idx], imgname)))
                gtLable=np.array(Image.open(os.path.join(gt_path, datalist['scenelist'][scene_idx],
                                                         datalist['gtLablelist'][scene_idx][img_idx])))
                gtInst=np.array(Image.open(os.path.join(gt_path, datalist['scenelist'][scene_idx],
                                                        datalist['gtInstlist'][scene_idx][img_idx])))
                pool.apply_async(prepocess, (img, gtLable, gtInst, scene_idx, img_idx))
            if i==num_process or img_idx==length-1:
                pool.close()
                pool.join()
                i = 0

    # Proposal and feature extraction
    path = os.path.abspath(os.path.join(VARS['abspath_out'],VARS['relpath_out_preprocess']))
    prelist = os.listdir(path)
    prelist.sort()
    n_samples = 40

    onlyground = 1
    i = 0    # i for controling the multi processes
    for k in range(len(prelist)):
        if i==0:
            pool=Pool(num_process)
        i=i+1
        tpfe = time.time()
        prefix = prelist[k][0:12]
        iouFname = os.path.abspath(os.path.join(VARS['abspath_out'], 
                                                VARS['relpath_out_trData'], 
                                                prefix+'train_data_20D.mat'))

        scene_idx = int(prefix[0:4])
        img_idx = int(prefix[5:9])

        img_path = os.path.abspath(os.path.join(VARS['abspath_LAF'], VARS['relpath_IMG'], VARS['subdir']))
        img_full = np.array(Image.open(os.path.join(img_path, datalist['scenelist'][scene_idx], datalist['imgslist'][scene_idx][img_idx])))
        
        if not os.path.isfile(iouFname):
            # Load data
            pre_data = loadmat_data(os.path.abspath(os.path.join(VARS['abspath_out'], 
                                                                VARS['relpath_out_preprocess'], 
                                                                prelist[k])))
            area_sampling = pre_data['rect']
            occ = pre_data['occ'].astype(np.float32)
            img = pre_data['imgPart']
            gtClass = pre_data['gtTypePart']
            gtInst = pre_data['gtInstPart']

            pool.apply_async(calonepic, (area_sampling, occ, img, gtClass, gtInst, img_full, prefix))
            print('Cal one sample \t time:', (time.time() - tpfe) * 1000, 'ms')

        if i==num_process or k==len(prelist)-1:
            pool.close()
            pool.join()
            i = 0

    # training random forest
    path = os.path.abspath(os.path.join(VARS['abspath_out'],VARS['relpath_out_trData']))
    datalist20 = os.listdir(path)
    datalist20.sort()
    for i in range(len(datalist20)):
        data20 = loadmat_data(os.path.abspath(os.path.join(VARS['abspath_out'],
                                                            VARS['relpath_out_trData'],
                                                            datalist20[i])))
        feature = data20['trainD'][:,0:-1]
        iou = data20['trainD'][:, -1]

        if np.size(features20) == 0:
            features20 = feature
            ious20 = iou
        else:
            features20 = np.r_[features20, feature]
            ious20 = np.r_[ious20, iou]

    sio.savemat(os.path.join(MODEL_PATH, 'samples_feat20D_python_' + exp_name + '.mat'),
                            {'features20':features20, 'ious20':ious20})
    rf = RandomForestRegressor(n_estimators=50, criterion='mse', max_depth=None,
                                 min_samples_split=2, min_samples_leaf=5, min_weight_fraction_leaf=0.0,
                                 max_features='auto', max_leaf_nodes=None, min_impurity_decrease=0.0,
                                 min_impurity_split=None, bootstrap=True, oob_score=True,
                                 n_jobs=None, random_state=None, verbose=0, warm_start=False,
                                 ccp_alpha=0.0, max_samples=None)
    
    rf.fit(features20, ious20)
    joblib.dump(rf, os.path.join(MODEL_PATH, 'OD_rfmodel_python_' + exp_name + '_50.m'))
    print("Training done")