// This source code is property of the Computer Graphics and Visualization 
// chair of the TU Dresden. Do not distribute! 
// Copyright (C) CGV TU Dresden - All Rights Reserved

#include <iostream>
#include "SurfaceArea.h"


float ComputeSurfaceArea(HEMesh mesh) {
    float area = 0.0;
    /* Task 1.2.2 */

    /* Iterate over faces */
    for (HEMesh::FaceIter vf_it = mesh.faces_begin(); vf_it != mesh.faces_end(); ++vf_it) {
        HEMesh::FaceVertexIter fv_it = mesh.fv_iter(*vf_it);

        HEMesh::Point face_area(0, 0, 0);
        for (int i = 0; i < mesh.valence(*vf_it); ++i) {
            HEMesh::Point from_point = mesh.point(*fv_it);
            ++fv_it;
            HEMesh::Point to_point = mesh.point(*fv_it);
            face_area += from_point.cross(to_point);
        }
        area += 0.5 * abs(face_area.length());
    }


    return area;
}