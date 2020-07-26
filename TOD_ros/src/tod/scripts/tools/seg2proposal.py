import numpy as np
def seg2proposal(gt):
    if np.max(gt)>999:
        C=np.unique(gt[gt>1000])
        x,y=np.where(gt>1000)
    else:
        C=np.unique(gt[gt>1])
        x,y=np.where(gt>1)

    instance_num = np.shape(C)[0]
    instance_box=np.zeros((instance_num,4))


    for k in range(instance_num):
        maxX=0
        maxY=0
        minX=np.shape(gt)[0]-1
        minY=np.shape(gt)[1]-1
        for i in range(np.shape(x)[0]):
            if C[k]==gt[x[i],y[i]]:
                maxX=max(maxX,x[i])
                minX=min(minX,x[i])
                maxY=max(maxY,y[i])
                minY=min(minY,y[i])
        instance_box[k,:]=np.array([minY, minX, maxY-minY, maxX-minX])
    instance_box=np.int32(instance_box)
    return instance_box


