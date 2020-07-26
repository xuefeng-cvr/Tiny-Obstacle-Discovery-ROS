import numpy as np

def align_boxes(bbox, minx, maxx, miny, maxy):
    bbox[:, [2, 3]] = bbox[:, [0, 1]] + bbox[:, [2, 3]] - 1
    bbox[bbox[:, 0] < minx, 0] = minx
    bbox[bbox[:, 1] < miny, 1] = miny
    bbox[bbox[:, 2] > maxx, 2] = maxx
    bbox[bbox[:, 3] > maxy, 3] = maxy
    bbox[:, [2, 3]] = bbox[:, [2, 3]] - bbox[:, [0, 1]] + 1
    return bbox


def resize_boxes(bbox, offset, ucm, O):
    bbox[:, 0] = bbox[:, 0] + offset[0]
    bbox[:, 1] = bbox[:, 1] + offset[1]
    bbox[:, 2] = bbox[:, 2] + offset[2]
    bbox[:, 3] = bbox[:, 3] + offset[3]
    h = np.size(ucm, 0)
    w = np.size(ucm, 1)
    bbox = align_boxes(bbox, 0, w - 1, 0, h - 1)
    bbox = scoreboxes(ucm, O, 0.65, 0.75, 0, 1e3, 0.1, 0.5, 0.5, 6, 1000, 2, 1.5, bbox)  # C++ 代码
    bbox = bbox[:, 0:4]
    return bbox


def filt_boxes(box, obsSzFilter_model, rect):
    area_thresh = 20
    bbox = box.copy()
    bbox[:, 0] = bbox[:, 0] + rect[0] - 1
    bbox[:, 1] = bbox[:, 1] + rect[1] - 1
    bbox[:, 0:2] = bbox[:, 0:2] + bbox[:, 2:4] / 2
    area_bboxes = bbox[:, 2] * bbox[:, 3]
    regionY = obsSzFilter_model['regionY']
    nlayers = np.size(regionY, 0)
    for i in range(nlayers):
        idx_r = np.where(((bbox[:, 1] > regionY[i, 0]) &
                         (bbox[:, 1] < regionY[i, 1]) &
                         (bbox[:, 3] < obsSzFilter_model['minmaxH'][i, 1] + area_thresh * (i+1)) &
                         (bbox[:, 3] > obsSzFilter_model['minmaxH'][i, 0] - area_thresh * (i+1)) &
                         (bbox[:, 2] < obsSzFilter_model['minmaxW'][i, 1] + area_thresh * (i+1)) &
                         (bbox[:, 2] > obsSzFilter_model['minmaxW'][i, 0] - area_thresh * (i+1)) &
                         (area_bboxes < obsSzFilter_model['minmaxSZ'][i, 1] + area_thresh * (i+1)) &
                         (area_bboxes > obsSzFilter_model['minmaxSZ'][i, 0])) == True)
        if i==0:
            idx = idx_r
        else:
            idx = np.concatenate((idx,idx_r),axis=1)
    return np.squeeze(box[idx, :]), idx


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
