//
// Created by lightquantum on 2020/12/26.
//

#include <cmath>
#include "MeanShift.h"

void MeanShiftClass::DoMeanShiftCluster() {
    for (int i = 0; i < point_num; i++) {
        stop = false;
        while (!stop) {
            ShiftOnce(dataset[i]);
        }
    }
    LabelClusters();
}

double MeanShiftClass::GetEuclideanDistance(QPointF p0, QPointF p1) {
    return sqrt((p0.x() - p1.x()) * (p0.x() - p1.x()) + (p0.y() - p1.y()) * (p0.y() - p1.y()));
}

double MeanShiftClass::GaussianKernel(double distance, double bandwidth) {
    return exp(-0.5 * (distance * distance) / (bandwidth * bandwidth));
}

void MeanShiftClass::ShiftOnce(MeanShift &p) {
    double x_sum = 0;
    double y_sum = 0;
    double weight_sum = 0;
    for (int i = 0; i < point_num; i++) {
        double tmp_distance = GetEuclideanDistance(p.res, dataset[i].pos);
        double weight = GaussianKernel(tmp_distance, kernel_bandwidth);
        x_sum += dataset[i].pos.x() * weight;
        y_sum += dataset[i].pos.y() * weight;
        weight_sum += weight;
    }
    QPointF shift_vector(x_sum / weight_sum, y_sum / weight_sum);
    double shift_distance = GetEuclideanDistance(p.res, shift_vector);

    if (shift_distance < EPSILON)
        stop = true;

    p.res = shift_vector;
}

void MeanShiftClass::LabelClusters() {
    int current_label = -1;
    for (int i = 0; i < point_num; i++) {
        if (dataset[i].label != -1)
            continue;
        current_label++;
        for (int j = 0; j < point_num; j++) {
            if (GetEuclideanDistance(dataset[i].res, dataset[j].res) < CLUSTER_EPSILON) {
                dataset[j].label = current_label;
            }
        }
    }
}

MeanShiftClass::MeanShiftClass(double bandwidth, const QVector<QPointF> &_dataset) : stop(false),
                                                                                  kernel_bandwidth(bandwidth),
                                                                                  point_num(_dataset.length()) {
    for (auto p: _dataset) {
        dataset.push_back(MeanShift{p, p});
    }
}
