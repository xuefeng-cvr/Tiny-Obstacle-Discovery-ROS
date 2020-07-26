import numpy as np

def boxoverlap(gt, b, c):
    o = np.zeros(b.shape[0])
    for i in range(b.shape[0]):
        bboxpatch = gt[b[i,1]:b[i,3]+b[i,1], b[i,0]:b[i,2]+b[i,0]]
        pixel_count_c = np.sum(gt == c)
        pixel_count_cInpart = np.sum(bboxpatch == c)
        o[i] = pixel_count_cInpart / ((b[i,2]+1)*(b[i,3]+1) + pixel_count_c - pixel_count_cInpart)
    return o

def boxoverfree(gt, b):
    o = np.zeros(b.shape[0])
    gt_freespace = gt >= 1
    for i in range(b.shape[0]):
        bboxpatch = np.zeros(gt.shape)
        bboxpatch[b[i,1]:b[i,3]+b[i,1], b[i,0]:b[i,2]+b[i,0]] = 1
        bboxpatch = gt_freespace + bboxpatch
        pixel_freespace = np.sum(bboxpatch == 2)
        o[i] = pixel_freespace / ((b[i,3]+1)*(b[i,2]+1))
    return o


def compute_iou(bbox,gt):
    if np.max(gt) > 999:
        gt = gt / 1000

    C=np.unique(gt[gt>1])
    instance_num=np.shape(C)[0]
    xy = np.argwhere(gt > 1)
    x = xy[:, 0]
    y = xy[:, 1]
    instance_box = np.zeros((instance_num, 4))
    iou_insts=np.zeros((instance_num,np.shape(bbox)[0]))

    for k in range(instance_num):
        iou = boxoverlap(gt, bbox[:, 0:4].astype(np.int), C[k])
        iou_insts[k, :] = iou

    return iou_insts


def compute_iou_v2(bbox,gt):
    if np.max(gt) > 999:
        gt = gt / 1000

    C=np.unique(gt[gt>0])
    instance_num=np.shape(C)[0]
    xy = np.argwhere(gt > 1)
    x = xy[:, 0]
    y = xy[:, 1]
    iou_insts=np.zeros((instance_num-1,np.shape(bbox)[0]))

    for k in range(instance_num):
        if k == 0:
            perc_free = boxoverfree(gt, bbox[:, 0:4].astype(np.int))
        else:
            iou = boxoverlap(gt, bbox[:, 0:4].astype(np.int), C[k])
            iou_insts[k-1, :] = iou

    return iou_insts,perc_free
