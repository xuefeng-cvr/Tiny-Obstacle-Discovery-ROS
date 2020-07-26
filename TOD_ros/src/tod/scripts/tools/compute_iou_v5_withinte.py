import numpy as np

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

def compute_iou_v5_withinte(bboxs,gt):
    '''
    Compute IoU between box and true segmentation
    '''
    if np.max(gt)<999:
        th=1
    else:
        th=1000

    bbox = bboxs.copy()
    bbox = bbox.astype(np.int)
    C=np.unique(gt[gt>th])
    H,W=np.shape(gt)
    instance_num=np.shape(C)[0]
    iou_insts=np.zeros((instance_num,np.shape(bbox)[0]))
    perc_free=np.zeros(np.shape(bbox)[0])
    area=bbox[:,3]*bbox[:,2]

    bbox[:,2:4]=bbox[:,2:4]+bbox[:,0:2]-1
    bbox[bbox[:,2]>W,2]=W
    bbox[bbox[:,3]>H,3]=H

    for k in range(instance_num):
        instmap=np.zeros(np.shape(gt))
        instmap[gt==C[k]]=1

        a1=np.sum(instmap)
        integral_inst=compute_integralMap(instmap)
        for i in range(np.shape(bbox)[0]):
            a3=compute_feat_byintmap(bbox[i,0],bbox[i,1],bbox[i,2],bbox[i,3],integral_inst)
            a2=area[i]
            iou_insts[k,i]=float(a3) /float(a1+a2-a3)

    freespacemap = np.zeros(np.shape(gt))
    freespacemap[gt >= th] = 1
    integral_fs = compute_integralMap(freespacemap)
    for i in range(np.shape(bbox)[0]):
        a3 = compute_feat_byintmap(bbox[i, 0], bbox[i, 1], bbox[i, 2], bbox[i, 3], integral_fs)
        a2 = area[i]
        perc_free[i] = float(a3) / float(a2)
    return iou_insts,perc_free
