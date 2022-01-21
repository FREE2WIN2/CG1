// This source code is property of the Computer Graphics and Visualization 
// chair of the TU Dresden. Do not distribute! 
// Copyright (C) CGV TU Dresden - All Rights Reserved

#include <iostream>
#include "GridTraverser.h"
#include "GridUtils.h"


GridTraverser::GridTraverser() {}

GridTraverser::GridTraverser(const Eigen::Vector3f &o, const Eigen::Vector3f &d, const Eigen::Vector3f cell_extents)
        : orig(o), dir(d), cellExtents(cell_extents) {
    dir.normalize();
    Init();
}

Eigen::Vector3f &GridTraverser::Origin() {
    return orig;
}

const Eigen::Vector3f &GridTraverser::Origin() const {
    return orig;
}

Eigen::Vector3f &GridTraverser::Direction() {
    return dir;
}

const Eigen::Vector3f &GridTraverser::Direction() const {
    return dir;
}

void GridTraverser::SetCellExtents(const Eigen::Vector3f &cellExtent) {
    this->cellExtents = cellExtent;
    Init();
}

void GridTraverser::Init() {
    current = PositionToCellIndex(orig, cellExtents);
    /* Task 4.2.2 */
    //you can add some precalculation code here
    currentPosition = orig;
}

void GridTraverser::operator++(int) {
    /* Task 4.2.2 */
    //traverse one step along the ray
    //update the cell index stored in attribute "current"
    CalculateNextGridDimensions();
    if (tx < ty && tx < tz) {
        currentPosition += dir * tx;
        if (dir.x() * tx > 0) {
            current.x()++;
        } else {
            current.x()--;
        }
    } else if (ty < tx && ty < tz) {
        currentPosition += dir * ty;
        if (dir.y() * ty > 0) {
            current.y()++;
        } else {
            current.y()--;
        }
    } else if (tz < tx && tz < ty) {
        currentPosition += dir * tz;
        if (dir.z() * tz > 0) {
            current.z()++;
        } else {
            current.z()--;
        }
    }

}

Eigen::Vector3i GridTraverser::operator*() {
    return current;
}

void GridTraverser::CalculateNextGridDimensions() {

    float nextGridPositionx = ( current.x() + dir.x()/abs(dir.x()) ) * cellExtents.x();
    float nextGridPositiony = ( current.y() + dir.y()/abs(dir.y()) ) * cellExtents.y();
    float nextGridPositionz = ( current.z() + dir.z()/abs(dir.z()) ) * cellExtents.z();
    tx = (nextGridPositionx - currentPosition.x()) / dir.x() ;
    ty =  (nextGridPositiony - currentPosition.y()) / dir.y();
    tz =  (nextGridPositionz - currentPosition.z()) / dir.z();

    if (tx == 0) {
        tx = cellExtents.x() / dir.x();
    }

    if (ty == 0) {
        ty = cellExtents.y() / dir.y();
    }

    if (tz == 0) {
        tz = cellExtents.z() / dir.z();
    }

}

	
