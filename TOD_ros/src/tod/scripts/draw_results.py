import os
import scipy.io as sio
import matplotlib.pyplot as plt
import numpy as np

def show_ROC(ax,roc,linewidth,isSmooth,LineStyle,color,label):
    tpr=roc['TPR'].squeeze()
    fpr=roc['FPR'].squeeze()
    if isSmooth:
        pass
    ax.plot(fpr,tpr,linewidth=linewidth,LineStyle=LineStyle,color=color,label=label)

def show_iou_recall(ax,recalls,thresh_idx_cnt,ious,linewidth,LineStyle,color,label):
    ignore,nious,ninsts=np.shape(recalls)
    iou_recalls1=np.reshape(recalls[thresh_idx_cnt,:,:],(nious,ninsts))
    # print(np.mean(iou_recalls1,1))
    plot1 = np.mean(iou_recalls1,1)# / np.shape(iou_recalls1)[1]
    ax.plot(ious,plot1,linewidth=linewidth,LineStyle=LineStyle,color=color,label=label)

def show_prop_recall(ax,recalls,thresh_idx_iou,cnts,linewidth,LineStyle,color,label):
    ncnts, ignore, ninsts = np.shape(recalls)
    cnts_recalls1  = np.reshape(recalls[:, thresh_idx_iou, :], (ncnts, ninsts))
    plot1 = np.mean(cnts_recalls1, 1)# / np.shape(cnts_recalls1)[1]
    ax.plot(cnts, plot1, linewidth=linewidth, LineStyle=LineStyle, color=color, label=label)

def show_AR(ax,recalls,cnts,linewidth,LineStyle,color,label):
    ncnts, nious, ninsts = np.shape(recalls)
    AR1_iou = np.zeros((1, ncnts))
    for i in range(0, nious):
        cnts_AR1_iou = np.reshape(recalls[:, i, :], (ncnts, ninsts))
        AR1_iou = AR1_iou + np.mean(cnts_AR1_iou, 1)# / np.shape(cnts_AR1_iou)[1]
    AR1_iou = AR1_iou / nious
    AR1_iou=AR1_iou.squeeze()
    ax.plot(cnts, AR1_iou, linewidth=linewidth, LineStyle=LineStyle, color=color, label=label)

def draw_results(root_roc,root_recall,roc_names,recall_names,color_box,labels,save_path):
    ################# Exp1roc #####################
    roc_data = {} 
    for i in range(len(roc_names)):
        roc_data[i] = sio.loadmat(os.path.join(root_roc, roc_names[i]))

    fig, ax = plt.subplots()
    for i in range(len(roc_names)):
        show_ROC(ax,roc_data[i],2, 0, '-',color_box[i],labels[i])

    ax.set(xlabel='False Positive Rate(FPR)', ylabel='True Positive Rate(TPR)', title='Receiver Operating Characteristic')
    plt.xlim(0, 0.04)
    plt.ylim(0, 1)
    y = np.arange(0,1.1,0.1)
    plt.yticks(y)
    ax.grid()
    ax.legend()
    fig.savefig(os.path.join(save_path,'test1_1.png'))

    ############ recall - Proposals - LAF ###################
    recalls_data = {} 
    for i in range(len(recall_names)):
        recalls_data[i] = sio.loadmat(os.path.join(root_recall, recall_names[i]))['recalls']

    cnts=np.arange(0,1001,5)
    ious=np.arange(0.5,1.01,0.05)
    iou_thresh=0.7
    thresh_idx_cnt=200
    thresh_idx_iou=4

    fig, ax = plt.subplots()
    for i in range(len(recall_names)):
        show_iou_recall(ax,recalls_data[i],thresh_idx_cnt,ious,2,'-',color_box[i],labels[i])
    ax.set(xlabel='IoU overlap threshold', ylabel='Recalls', title='Proposals = 1000')
    plt.xlim(0.5, 1)
    plt.ylim(0, 1)
    y = np.arange(0,1.1,0.1)
    plt.yticks(y)
    ax.grid()
    ax.legend()
    fig.savefig(os.path.join(save_path,'test1_2.png'))

    fig, ax = plt.subplots()
    for i in range(len(recall_names)):
        show_prop_recall(ax,recalls_data[i], thresh_idx_iou, cnts, 2, '-', color_box[i], labels[i])
    ax.set(xlabel='IoU overlap threshold', ylabel='Recalls',title='IoU = 0.7')
    plt.xlim(0, 1000)
    plt.ylim(0, 0.6)
    ax.grid()
    ax.legend()
    fig.savefig(os.path.join(save_path,'test1_3.png'))

    fig, ax = plt.subplots()
    for i in range(len(recall_names)):
        show_AR(ax,recalls_data[i], cnts, 2, '-', color_box[i], labels[i])
    ax.set(xlabel='Proposals', ylabel='Average Recalls', title='IoU between [0.5 1]')
    plt.xlim(0,1000)
    plt.ylim(0, 0.6)
    ax.grid()
    ax.legend()
    fig.savefig(os.path.join(save_path,'test1_4.png'))
    plt.show()

if __name__ == '__main__':
    color_box   = ['darkblue','red','olive','darkorange', 'darkturquoise','forestgreen','maroon','lightcoral','rosybrown','dimgrey']
    labels      = ['2020_07_15_python','2020_07_03_Matlab']
    roc_names   = ['ROC_2020_07_15.mat', 'ROC_2020_07_03.mat']
    recall_names = ['recall_2020_07_15.mat', 'recall_2020_07_03.mat']

    # exp_name = '2020_07_15'
    # change these two paths to the root paths of ROC and recall files
    root_roc = os.path.abspath(os.path.join(os.getcwd(),'result/ROC'))
    root_recall = os.path.abspath(os.path.join(os.getcwd(),'result/recall'))

    save_path = os.path.abspath(os.path.join(os.getcwd(),'result/figure/'))

    draw_results(root_roc,root_recall,roc_names,recall_names,color_box,labels,save_path)