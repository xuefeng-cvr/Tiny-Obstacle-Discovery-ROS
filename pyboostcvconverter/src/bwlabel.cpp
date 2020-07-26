#include "bwlabel.h"
#include "MathUtils.hpp"

int bwlabel(int *map, int h, int w, int *&connectMap)
{
    int label = 0;
    int x, y, ind;
    connectMap = new int[h * w];
    memset(connectMap, 0, h * w * sizeof(int));
    int pStack[50];
    int pCount = 0;
    for (ind = 0; ind < h * w; ind++)
    {
        if (map[ind] == 0)
            continue;
        if (map[ind] == 1 && connectMap[ind] != 0)
            continue;
        label++;
        pStack[pCount++] = ind;
        connectMap[ind] = label;
        while (pCount > 0)
        {
            int p = pStack[--pCount];
            if ((!(IS_LEFT_ADJ(p, w))) && (map[p - 1] == 1) && (connectMap[p - 1] == 0))
            {
                pStack[pCount++] = p - 1;
                connectMap[p - 1] = label;
            }
            if ((!(IS_BOTTOM_ADJ(p, h, w))) && (map[p + w] == 1) && (connectMap[p + w] == 0))
            {
                pStack[pCount++] = p + w;
                connectMap[p + w] = label;
            }
            if ((!(IS_RIGHT_ADJ(p, w))) && (map[p + 1] == 1) && (connectMap[p + 1] == 0))
            {
                pStack[pCount++] = p + 1;
                connectMap[p + 1] = label;
            }
            if ((!(IS_TOP_ADJ(p, w))) && (map[p - w] == 1) && (connectMap[p - w] == 0))
            {
                pStack[pCount++] = p - w;
                connectMap[p - w] = label;
            }
        }
    }
    return label;
}

void first_pass(const int sc[], const int ec[], const int r[], int labels[], const int num_runs, const int mode)
{
    int cur_row = 0;
    int next_label = 1;
    int first_run_on_prev_row = -1;
    int last_run_on_prev_row = -1;
    int first_run_on_this_row = 0;
    int offset = 0;
    int *equal_i = new int[num_runs];
    int *equal_j = new int[num_runs];
    int equal_idx = 0;
    if (mode == 8)
        offset = 1;
    for (int k = 0; k < num_runs; k++)
    {
        if (r[k] == cur_row + 1)
        {
            cur_row += 1;
            first_run_on_prev_row = first_run_on_this_row;
            first_run_on_this_row = k;
            last_run_on_prev_row = k - 1;
        }
        else if (r[k] > cur_row + 1)
        {
            first_run_on_prev_row = -1;
            last_run_on_prev_row = -1;
            first_run_on_this_row = k;
            cur_row = r[k];
        }
        if (first_run_on_prev_row >= 0)
        {
            int p = first_run_on_prev_row;
            while (p <= last_run_on_prev_row && sc[p] <= (ec[k] + offset))
            {
                if (sc[k] <= ec[p] + offset)
                {
                    if (labels[k] == 0)
                        labels[k] = labels[p];
                    else if (labels[k] != labels[p])
                    {
                        equal_i[equal_idx] = labels[k];
                        equal_j[equal_idx] = labels[p];
                        equal_idx += 1;
                    }
                }
                p += 1;
            }
        }
        if (labels[k] == 0)
        {
            labels[k] = next_label++;
        }
    }
    /////////////////////// process labels
    for (int i = 0; i < equal_idx; i++)
    {
        int max_label = equal_i[i] > equal_j[i] ? equal_i[i] : equal_j[i];
        int min_label = equal_i[i] < equal_j[i] ? equal_i[i] : equal_j[i];
        for (int j = 0; j < num_runs; j++)
        {
            if (labels[j] == max_label)
                labels[j] = min_label;
        }
    }
    delete[] equal_i;
    delete[] equal_j;
    /////////////////////process ignore labels
    int *hist = new int[next_label];
    int *non_labels = new int[next_label];
    memset(hist, 0, sizeof(int) * next_label);
    int non_num = 0;
    for (int i = 0; i < num_runs; i++)
    {
        hist[labels[i]]++;
    }
    for (int i = 1; i < next_label; i++)
    {
        if (hist[i] == 0)
            non_labels[non_num++] = i;
    }
    for (int j = 0; j < num_runs; j++)
    {
        int k = labels[j];
        for (int i = non_num - 1; i >= 0; i--)
        {
            if (k > non_labels[i])
            {
                labels[j] -= (i + 1);
                break;
            }
        }
    }
    delete[] hist;
    delete[] non_labels;
}
void fill_run_vectors(const cv::Mat in, int sc[], int ec[], int r[])
{
    const int rows = in.rows;
    const int cols = in.cols;
    int idx = 0;
    for (int row = 0; row < rows; row++)
    {
        const uchar *p_row = in.ptr<uchar>(row);
        int prev = 0;
        for (int col = 0; col < cols; col++)
        {
            if (p_row[col] != prev)
            {
                if (prev == 0)
                {
                    sc[idx] = col;
                    r[idx] = row;
                    prev = 1;
                }
                else
                {
                    ec[idx++] = col - 1;
                    prev = 0;
                }
            }
            if (col == cols - 1 && prev == 1)
            {
                ec[idx++] = col;
            }
        }
    }
}
int number_of_runs(const cv::Mat in)
{
    const int rows = in.rows;
    const int cols = in.cols;
    int result = 0;
    for (int row = 0; row < rows; row++)
    {
        const uchar *p_row = in.ptr<uchar>(row);
        if (p_row[0] != 0)
            result++;
        for (int col = 1; col < cols; col++)
        {
            if (p_row[col] != 0 && p_row[col - 1] == 0)
                result++;
        }
    }
    return result;
}
Mat bwlabel(const Mat in, int *num, const int mode)
{
    const int num_runs = number_of_runs(in);
    int *sc = new int[num_runs];
    int *ec = new int[num_runs];
    int *r = new int[num_runs];
    int *labels = new int[num_runs];
    memset(labels, 0, sizeof(int) * num_runs);
    fill_run_vectors(in, sc, ec, r);
    first_pass(sc, ec, r, labels, num_runs, mode);
    Mat result = Mat::zeros(in.size(), CV_8UC1);
    
    int number = 0;
    for (int i = 0; i < num_runs; i++)
    {
        uchar *p_row = result.ptr<uchar>(r[i]);
        for (int j = sc[i]; j <= ec[i]; j++)
            p_row[j] = labels[i];
        if (number < labels[i])
            number = labels[i];
    }
    if (num != NULL)
        *num = number;
    delete[] sc;
    delete[] ec;
    delete[] r;
    delete[] labels;
    return result;
}

tuple<Mat,int> bwlabel(Mat map, int n)
{
    int label = 0;
    int x, y;
    int h = map.rows,w = map.cols;

    Mat contourMap;
    map.copyTo(contourMap);
    Mat connectMap(map.size(),CV_32SC1,Scalar(0));
    
    vector<Point> pStack;
    int pCount = 0;
    for (y = 0; y < h; y++)
    {
        for (x=0; x < w; x++)
        {
            if (contourMap.at<double>(y,x) == 0)
                continue;
            if (contourMap.at<double>(y,x) == 1 && connectMap.at<int>(y,x) != 0)
                continue;
            label++;
            Point p(x,y);
            pCount++;
            pStack.push_back(p);
            connectMap.at<int>(y,x) = label;
            while (pStack.size() > 0)
            {
                Point pn = pStack[pStack.size()-1];
                pStack.pop_back();
                int px = pn.x,py = pn.y;
                if ((px>0) && (contourMap.at<double>(py,px-1) == 1) && (connectMap.at<int>(py,px-1) == 0))
                {
                    Point pSave(px-1,py);
                    pStack.push_back(pSave);
                    pCount++;
                    connectMap.at<int>(pSave) = label;
                }
                if ((py<h-1) && (contourMap.at<double>(py+1,px) == 1) && (connectMap.at<int>(py+1,px) == 0))
                {
                    Point pSave(px,py+1);
                    pStack.push_back(pSave);
                    pCount++;
                    connectMap.at<int>(pSave) = label;
                }
                if ((px<w-1) && (contourMap.at<double>(py,px+1) == 1) && (connectMap.at<int>(py,px+1) == 0))
                {
                    Point pSave(px+1,py);
                    pStack.push_back(pSave);
                    pCount++;
                    connectMap.at<int>(pSave) = label;
                }
                if ((py>0) && (contourMap.at<double>(py-1,px) == 1) && (connectMap.at<int>(py-1,px) == 0))
                {
                    Point pSave(px,py-1);
                    pStack.push_back(pSave);
                    pCount++;
                    connectMap.at<int>(pSave) = label;
                }
                if(n == 8)
                {
                    if ((px>0&&py<h-1) && (contourMap.at<double>(py+1,px-1) == 1) && (connectMap.at<int>(py+1,px-1) == 0))
                    {
                        Point pSave(px-1,py+1);
                        pStack.push_back(pSave);
                        pCount++;
                        connectMap.at<int>(pSave) = label;
                    }
                    if ((px<w-1&&py<h-1) && (contourMap.at<double>(py+1,px+1) == 1) && (connectMap.at<int>(py+1,px+1) == 0))
                    {
                        Point pSave(px+1,py+1);
                        pStack.push_back(pSave);
                        pCount++;
                        connectMap.at<int>(pSave) = label;
                    }
                    if ((px<w-1&&py>0) && (contourMap.at<double>(py-1,px+1) == 1) && (connectMap.at<int>(py-1,px+1) == 0))
                    {
                        Point pSave(px+1,py-1);
                        pStack.push_back(pSave);
                        pCount++;
                        connectMap.at<int>(pSave) = label;
                    }
                    if ((px>0&&py>0) && (contourMap.at<double>(py-1,px-1) == 1) && (connectMap.at<int>(py-1,px-1) == 0))
                    {
                        Point pSave(px-1,py-1);
                        pStack.push_back(pSave);
                        pCount++;
                        connectMap.at<int>(pSave) = label;
                    }
                }
            }
        }
    }
    
    tuple<Mat,int> output = make_tuple(connectMap,label);
    contourMap.release();
    return output;
}
