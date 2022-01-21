// This source code is property of the Computer Graphics and Visualization 
// chair of the TU Dresden. Do not distribute! 
// Copyright (C) CGV TU Dresden - All Rights Reserved

#pragma once

#include <array>
#include <Eigen/Core>
#include <utility>
#include <vector>

//converts 3d floating point position pos into 3d integer grid cell index
inline Eigen::Vector3i PositionToCellIndex(const Eigen::Vector3f &pos, const Eigen::Vector3f &cellExtents) {
    Eigen::Vector3i idx;
    for (int d = 0; d < 3; ++d) {
        idx[d] = int(pos[d] / cellExtents[d]);
        if (pos[d] < 0)
            --idx[d];
    }
    return idx;
}

//returns true if the two Interval [lb1,ub2] and [lb2,ub2] overlap 
inline bool OverlapIntervals(float lb1, float ub1, float lb2, float ub2) {
    if (lb1 > ub1)
        std::swap(lb1, ub1);
    if (lb2 > ub2)
        std::swap(lb2, ub2);
    if (ub1 < lb2 || lb1 > ub2)
        return false;
    return true;
}
//returns true if the triangle and the box have a seperating line on the seperating axis
inline bool HasSeperatingLine(const Eigen::Vector3f& seperatingAxis, const std::vector<Eigen::Vector3f>& a, const std::vector<Eigen::Vector3f>& b) {
    //projected min and max points on the axis
    auto aMaxProj = -std::numeric_limits<float>::infinity();
    auto aMinProj = std::numeric_limits<float>::infinity();
    auto bMaxProj = -std::numeric_limits<float>::infinity();
    auto bMinProj = std::numeric_limits<float>::infinity();
    for (const Eigen::Vector3f& v: a) {
        auto proj = seperatingAxis.dot(v);
        if (proj < aMinProj) aMinProj = proj;
        if (proj > aMaxProj) aMaxProj = proj;
    }
    for (const Eigen::Vector3f& v: b) {
        auto proj = seperatingAxis.dot(v);
        if (proj < bMinProj) bMinProj = proj;
        if (proj > bMaxProj) bMaxProj = proj;
    }

    // check if the intervals are overlapping
    return !OverlapIntervals(aMinProj, aMaxProj, bMinProj, bMaxProj);
}
