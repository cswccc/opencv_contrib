#include <zxing/common/KMeans.h>
#include <zxing/common/util/INIReader.h>

#include<iostream>
#include<cmath>
#include<vector>

typedef unsigned int uint;

namespace zxing
{

double cal_distance(std::vector<double> a, std::vector<double> b)
{
    float KMEANS_COUNT_FACTOR = GetIniParser()->GetReal("FP_SELECT", "KMEANS_COUNT_FACTOR", 0.0);
    float KMEANS_MS_FACTOR = GetIniParser()->GetReal("FP_SELECT", "KMEANS_MS_FACTOR", 1.0);
    
    uint da = a.size();
    uint db = b.size();
    if (da != db) std::cerr << "Dimensions of two vectors must be same!!\n";
    double val = 0.0;
    for (uint i = 0; i < da; i++)
    {
        if (i == 1)
            val += KMEANS_MS_FACTOR*pow((a[i] - b[i]), 2);
        else if (i == 0)
            val += KMEANS_COUNT_FACTOR*pow((a[i] - b[i]), 2);
        else
            val += pow((a[i] - b[i]), 2);
    }
    return pow(val, 0.5);
}

/*
 * maxepoches ����������
 * minchanged ��С���ĵ�䶯��
 */
std::vector<Cluster> k_means(std::vector<std::vector<double> > trainX, uint k, uint maxepoches, uint minchanged)
{
    const uint row_num = trainX.size();
    const uint col_num = trainX[0].size();
    
    /*��ʼ����������*/
    std::vector<Cluster> clusters(k);
    int step = trainX.size() / k;
    
    for (uint i = 0; i < k; i++)
    {
        clusters[i].centroid = trainX[i*step];
    }
    
    /*��ε���ֱ�������������������100��*/
    for (uint it = 0; it < maxepoches; it++)
    {
        /*ÿһ�����¼����������������֮ǰ�����ԭ����������Ϣ*/
        for (uint i = 0; i < k; i++)
        {
            clusters[i].samples.clear();
        }
        /*���ÿ���������Ӧ��������һ������*/
        for (uint j = 0; j < row_num; j++)
        {
            /*����ʼ�����ڵ�0������*/
            uint c = 0;
            double min_distance = cal_distance(trainX[j], clusters[c].centroid);
            for (uint i = 1; i < k; i++)
            {
                double distance = cal_distance(trainX[j], clusters[i].centroid);
                if (distance < min_distance)
                {
                    min_distance = distance;
                    c = i;
                }
            }
            clusters[c].samples.push_back(j);
        }
        
        uint changed = 0;
       
        for (uint i = 0; i < k; i++)
        {
            std::vector<double> val(col_num, 0.0);
            for (uint j = 0; j < clusters[i].samples.size(); j++)
            {
                uint sample = clusters[i].samples[j];
                for (uint d = 0; d < col_num; d++)
                {
                    val[d] += trainX[sample][d];
                    if (j == clusters[i].samples.size() - 1)
                    {
                        double  value = val[d] / clusters[i].samples.size();
                        if (clusters[i].centroid[d] != value)
                        {
                            clusters[i].centroid[d] = value;
                            changed++;
                        }
                    }
                }
            }
        }
        
        if (changed <= minchanged) return clusters;
    }
    return clusters;
}

}  // namespace zxing
