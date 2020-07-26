import numpy as np
from tools.seg2proposal import seg2proposal
import scipy.io as scio

def compute_integralMap(map):
    integral_map = np.cumsum(np.cumsum(map, 0), 1)
    integral_map = np.insert(integral_map, 0, values=np.zeros(np.shape(integral_map)[1]), axis=0)
    integral_map = np.insert(integral_map, 0, values=np.zeros(np.shape(integral_map)[0]), axis=1)
    return integral_map

def compute_feat_byintmap(minx, miny, maxx, maxy, integral_map):
    if integral_map.ndim == 2:
        return integral_map[maxy + 1, maxx + 1] \
               + integral_map[miny, minx] \
               - integral_map[maxy + 1, minx] \
               - integral_map[miny, maxx + 1]
    elif integral_map.ndim > 2:
        return integral_map[maxy + 1, maxx + 1, :] \
               + integral_map[miny, minx, :] \
               - integral_map[maxy + 1, minx, :] \
               - integral_map[miny, maxx + 1, :]
    else:
        print('dim is wrong %d', integral_map.ndim)

def compute_iou_v6(bboxs, gt):
    '''
    Compute IoU between box and true bounding box
    '''
    if np.max(gt) > 999:
        fsid = 1000
    else:
        fsid = 1

    bbox = bboxs.copy()
    bbox = bbox.astype(np.int)
    h, w = np.shape(gt)
    inst_box = seg2proposal(gt)

    instance_num = np.shape(inst_box)[0]

    iou_insts = np.zeros((instance_num, np.shape(bbox)[0]))
    perc_free = np.zeros(np.shape(bbox)[0])
    area = bbox[:, 3] * bbox[:, 2]
    bbox[:, 3] = bbox[:, 3] + bbox[:, 1] - 1
    bbox[:, 2] = bbox[:, 2] + bbox[:, 0] - 1
    bbox[:,0:4]=np.round(bbox[:,0:4])

    bbox[bbox[:, 0] < 1, 0] = 1
    bbox[bbox[:, 1] < 1, 1] = 1
    bbox[bbox[:, 2] > w, 2] = w
    bbox[bbox[:, 3] > h, 3] = h

    y,ignore=np.where(bbox[:,0:4]==0)
    y=np.unique(y)
    bbox=np.delete(bbox,y,axis=0)

    for k in range(instance_num):
        instmap=np.zeros(np.shape(gt))
        instmap[inst_box[k,1]:(inst_box[k,1]+inst_box[k,3]+1),inst_box[k,0]:(inst_box[k,0]+inst_box[k,2]+1)]=1

        a1=np.sum(instmap)
        integral_inst=compute_integralMap(instmap)
        for i in range(np.shape(bbox)[0]):
            a3=compute_feat_byintmap(bbox[i,0],bbox[i,1],bbox[i,2],bbox[i,3],integral_inst)
            a2=area[i]
            iou_insts[k,i]=float(a3) /float(a1+a2-a3)

    freespacemap=np.zeros(np.shape(gt))
    freespacemap[gt>=fsid]=1

    integral_fs=compute_integralMap(freespacemap)
    for i in range(np.shape(bbox)[0]):
        a3=compute_feat_byintmap(bbox[i,0],bbox[i,1],bbox[i,2],bbox[i,3],integral_fs)
        a2=area[i]
        perc_free[i]=float(a3)/float(a2)

    return iou_insts,perc_free
