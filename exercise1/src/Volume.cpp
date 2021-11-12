// This source code is property of the Computer Graphics and Visualization 
// chair of the TU Dresden. Do not distribute! 
// Copyright (C) CGV TU Dresden - All Rights Reserved

#include "Volume.h"

#include <iostream>



float ComputeVolume(HEMesh &mesh) {
    float vol = 0;

    /* Mesh is already triangulated!*/

    for (HEMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it) {
        HEMesh::FaceVertexIter fv_it = mesh.fv_begin(*f_it);
        HEMesh::Point point1 = mesh.point(*fv_it);
        ++fv_it;
        HEMesh::Point point2 = mesh.point(*fv_it);
        ++fv_it;
        HEMesh::Point point3 = mesh.point(*fv_it);

        vol += point1.dot(point2.cross(point3)) / 6.0f;
    }

    /* Improvements: Check if object ist closed(but its not the task)*/

    /* e.g. triangulate and check that V + F - E = 2 (Euler)*/
    return vol;
}