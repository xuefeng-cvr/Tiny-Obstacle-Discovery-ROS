import numpy as np
import tools.pbcvt as pbcvt

def get_proposal(E, layer, obsSzFilter_model, rect):
    
    alpha = 0.65
    kappa = 1.5
    minScore = 0.01
    maxNum = 10000
    maxRatio = 6
    minArea = 60

    E = E/E.max()
    O = calO(E)

    regionY = obsSzFilter_model['regionY']
    minmaxH = obsSzFilter_model['minmaxH']
    minmaxW = obsSzFilter_model['minmaxW']
    minmaxSZ = obsSzFilter_model['minmaxSZ']
    reg_x = int(rect[0])
    reg_y = int(rect[1])

    bbox = pbcvt.scorebox(E, O, alpha, kappa, minScore, maxNum, maxRatio, minArea, regionY, minmaxH, minmaxW, minmaxSZ, reg_x, reg_y)
    return bbox, O

def calO(E):
    Oy, Ox = np.gradient(convTri(E, 4))
    i, Oxx = np.gradient(Ox)
    Oyy, Oxy = np.gradient(Oy)
    O = np.arctan(Oyy * np.sign(-Oxy) / (Oxx + (1e-5))) % np.pi
    return O

def convTri(I, r):
    J = pbcvt.convConst(I, r)
    return J
