import os
import numpy as np
import scipy.io as sio
from tools.toolbox import str2bool, loadmat_data, imadjust_rgb
import joblib


def get_params_LAF(is_train, save_fv=None, exp_name=None, exp_dir=None, MODEL_PATH=None, modelname=None):
    """
    load models written in ".mat" format depending on model type
    :param is_train: is in the training process or not
    :param save_fv: need to save the feature vector or not
    :param exp_name: the name of this experiment
    :param exp_dir: the path to save result of experiment
    :param MODEL_PATH: the path to the models
    :param modelname: the filename of random forest model
    :return: load all the data into VARS
    """
    VARS = {}

    VARS['abspath_LAF']='/data/lostandfound/'    # change the variable to the root path of lost and found on your computer
    VARS['relpath_IMG']='leftImg8bit/'
    VARS['relpath_GT']='gtCoarse/'
    VARS['relpath_DISP']='disparity/'
    VARS['relpath_TRAIN']='train/'
    VARS['relpath_TEST']='test/'
    VARS['suffix_gtLabel']='_gtCoarse_labelIds.png'
    VARS['suffix_gtInst']='_gtCoarse_instanceIds.png'
    VARS['suffix_gtJson']='_gtCoarse_polygons.json'
    VARS['suffix_disparity']='_disparity.png'
    VARS['suffix_out']='proposal.png'

    if is_train:
        VARS['subdir']=VARS['relpath_TRAIN']
    else:
        VARS['subdir']=VARS['relpath_TEST']

    if exp_name != None:
        path=os.path.abspath(os.path.join(MODEL_PATH, 'box_size_filter.mat'))
        VARS['box_size_filter'] = loadmat_data(path)['obs_size_filter']
        path=os.path.abspath(os.path.join(MODEL_PATH, 'region_LAF.mat'))
        VARS['region_LAF']=loadmat_data(path)['freq_obs']
        path=os.path.abspath(os.path.join(MODEL_PATH, 'modelBsds.mat'))
        VARS['mod_sed']=loadmat_data(path)['model']
        path=os.path.abspath(os.path.join(MODEL_PATH, 'obsnumber.mat'))
        VARS['obsnumber']=loadmat_data(path)['obsnumber']

        # reversing dimensions
        VARS['mod_sed']['thrs']=VARS['mod_sed']['thrs'].T
        VARS['mod_sed']['fids']=VARS['mod_sed']['fids'].T
        VARS['mod_sed']['child']=VARS['mod_sed']['child'].T
        VARS['mod_sed']['count']=VARS['mod_sed']['count'].T
        VARS['mod_sed']['depth']=VARS['mod_sed']['depth'].T
        VARS['mod_sed']['segs']=VARS['mod_sed']['segs'].T
        VARS['mod_sed']['nSegs']=VARS['mod_sed']['nSegs'].T
        VARS['mod_sed']['eBins']=VARS['mod_sed']['eBins'].T
        VARS['mod_sed']['eBnds']=VARS['mod_sed']['eBnds'].T

        
        if is_train:
            # training path setup
            VARS['abspath_out']=os.path.join(exp_dir, 'TrainingData_python_' + exp_name)
            path=os.path.abspath(VARS['abspath_out'])
            if not os.path.exists(path):
                os.makedirs(path)
            VARS['relpath_out_preprocess']='preproc/'
            path=os.path.abspath(os.path.join(VARS['abspath_out'], VARS['relpath_out_preprocess']))
            if not os.path.exists(path):
                os.makedirs(path)
            VARS['relpath_out_cand']='cand/'
            path=os.path.abspath(os.path.join(VARS['abspath_out'], VARS['relpath_out_cand']))
            if not os.path.exists(path):
                os.makedirs(path)
            VARS['relpath_out_trData']='traindata/'
            path=os.path.abspath(os.path.join(VARS['abspath_out'], VARS['relpath_out_trData']))
            if not os.path.exists(path):
                os.makedirs(path)
            VARS['relpath_train_allucms']='ucms/'
            path=os.path.abspath(os.path.join(VARS['abspath_out'], VARS['relpath_train_allucms']))
            if not os.path.exists(path):
                os.makedirs(path)
            path=os.path.abspath(os.path.join(MODEL_PATH, 'dist_obs_trainset.mat'))
            VARS['dist_obs']=loadmat_data(path)['dist_obs']
        else:
            # testing path setup
            VARS['abspath_test']=os.path.join(exp_dir, 'TestingData_python_' + exp_name)
            path=os.path.abspath(VARS['abspath_test'])
            if not os.path.exists(path):
                os.makedirs(path)
            VARS['relpath_test_result']='result/'
            path=os.path.abspath(os.path.join(VARS['abspath_test'], VARS['relpath_test_result']))
            if not os.path.exists(path):
                os.makedirs(path)
            VARS['relpath_test_tops']='top/'
            path=os.path.abspath(os.path.join(VARS['abspath_test'], VARS['relpath_test_tops']))
            if not os.path.exists(path):
                os.makedirs(path)
            VARS['relpath_test_allbox']='allbbox/'
            path=os.path.abspath(os.path.join(VARS['abspath_test'], VARS['relpath_test_allbox']))
            if not os.path.exists(path):
                os.makedirs(path)
            VARS['relpath_test_times']='times/'
            path=os.path.abspath(os.path.join(VARS['abspath_test'], VARS['relpath_test_times']))
            if not os.path.exists(path):
                os.makedirs(path)
            VARS['relpath_test_allucms']='ucms/'
            path=os.path.abspath(os.path.join(VARS['abspath_test'], VARS['relpath_test_allucms']))
            if not os.path.exists(path):
                os.makedirs(path)
            VARS['relpath_test_probmaps']='probmaps/'
            path=os.path.abspath(os.path.join(VARS['abspath_test'], VARS['relpath_test_probmaps']))
            if not os.path.exists(path):
                os.makedirs(path)
            if save_fv:
                VARS['relpath_test_FeatVec']='featvecs/'
                path=os.path.abspath(os.path.join(VARS['abspath_test'], VARS['relpath_test_FeatVec']))
                if not os.path.exists(path):
                    os.makedirs(path)
            VARS['roc_mat']='ROC_' + exp_name + '.mat'
            VARS['recall_mat']='recall_' + exp_name + '.mat'
            VARS['time_mat']='time_' + exp_name + '.mat'
            path=os.path.abspath(os.path.join(MODEL_PATH, 'dist_obs_testset.mat'))
            VARS['dist_obs']=loadmat_data(path)['dist_obs']

            if modelname:
                path=os.path.abspath(os.path.join(MODEL_PATH, modelname))
                VARS['rf'] = joblib.load(path)
    return VARS

















