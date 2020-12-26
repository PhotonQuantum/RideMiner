//
// Created by lightquantum on 2020/12/26.
//

#ifndef PSPPROJ_MEANSHIFT_H
#define PSPPROJ_MEANSHIFT_H

#include <iostream>
#include <QPointF>
#include <QVector>
#define EPSILON 0.001
#define CLUSTER_EPSILON 10

using namespace std;

struct MeanShift
{
    QPointF pos;
    QPointF res;
    int label = -1;
};

class MeanShiftClass
{
public:
    MeanShiftClass(double bandwidth, const QVector<QPointF>& _dataset);;
    void DoMeanShiftCluster();
    vector<MeanShift> dataset;
private:
    bool stop;
    int point_num;
    double kernel_bandwidth;

    void LabelClusters();
    void ShiftOnce(MeanShift& p);
    static double GaussianKernel(double distance, double bandwidth);
    static double GetEuclideanDistance(QPointF p0, QPointF p1);
};
#endif //PSPPROJ_MEANSHIFT_H
