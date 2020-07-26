import numpy as np

def compute_obsScore(vars, feat, bbox, th=0, w=0):
    if th == 0 & w == 0:
        vars.th = th
        vars.w = w
    if vars.modelType == 1:
        score, prediction_per_tree = regRF_predict(feat, vars.rf)
        idx = -1
    elif vars.modelType == 2:
        class_scores_mod1, prediction_per_tree_mod1 = regRF_predict(feat, vars.rf)
        class_scores_mod2, prediction_per_tree_mod2 = regRF_predict(feat, vars.rf2)

        ids = np.argsort(class_scores_mod1, 'descend')[::-1]
        ids_p_m1 = ids[range(np.size(ids, axis=1) * vars.th_mod1)]
        score = class_scores_mod1[ids_p_m1, :] + vars.w_mod2 * class_scores_mod2[ids_p_m1, :]

        idx = ids_p_m1
        prediction_per_tree = np.concatenate(
            (prediction_per_tree_mod1[ids_p_m1, :], prediction_per_tree_mod2[ids_p_m1, :]), axis=1)
    else:
        print('Wrong model type')
    return score, prediction_per_tree, idx
