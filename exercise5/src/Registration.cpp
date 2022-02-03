// This source code is property of the Computer Graphics and Visualization 
// chair of the TU Dresden. Do not distribute! 
// Copyright (C) CGV TU Dresden - All Rights Reserved

#include "Registration.h"

Eigen::Affine3f CalculateRigidRegistration(std::vector<correspondence> &correspondences) {
    //transform to compute
    Eigen::Affine3f T = Eigen::Affine3f(Eigen::Matrix3f::Identity());

    if (correspondences.size() < 3)
        return T;

    /* Task 5.2.2 */
    //Rotation matrix R
    Eigen::Vector3f inCentroid = Eigen::Vector3f::Zero();
    Eigen::Vector3f outCentroid = Eigen::Vector3f::Zero();
    for (auto corr = correspondences.begin(); corr != correspondences.end(); corr++) {
        inCentroid += corr->first;
        outCentroid += corr->second;
    }
    inCentroid /= (float) correspondences.size();
    outCentroid /= (float) correspondences.size();

    Eigen::Matrix3f Cov = Eigen::Matrix3f::Zero();
    for (auto corr = correspondences.begin(); corr != correspondences.end(); corr++) {
        Cov += (corr->second - outCentroid) * (corr->first - inCentroid).transpose();
    }
    // SVD
    Eigen::JacobiSVD<Eigen::Matrix3f> svd(Cov, Eigen::ComputeFullU | Eigen::ComputeFullV);

    // Find the rotation
    double d = (svd.matrixV() * svd.matrixU().transpose()).determinant();
    Eigen::Matrix3f I = Eigen::Matrix3f::Identity(3, 3);
    I(2, 2) = d;
    Eigen::Matrix3f R = svd.matrixV() * I * svd.matrixU().transpose();

    // The final transform
    T.linear() = R;
    T.translation() = inCentroid - R * outCentroid;

    return T;
}
