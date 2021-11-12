// This source code is property of the Computer Graphics and Visualization 
// chair of the TU Dresden. Do not distribute! 
// Copyright (C) CGV TU Dresden - All Rights Reserved

#include <iostream>
#include "SurfaceArea.h"


float ComputeSurfaceArea(HEMesh &mesh) {
    float area = 0.0;
    /* Task 1.2.2 */

    /* Iterate over faces */
    for (HEMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it) {
        HEMesh::Point face_area(0, 0, 0);

        /* had to remove const from parameter mesh because mesh.fh_begin() don't work on const variable */
        for (HEMesh::FaceHalfedgeIter fh_it = mesh.fh_begin(*f_it); fh_it != mesh.fh_end(*f_it); ++fh_it) {
            //would do with *fh_it.to() but it returns smart vertex handle!
            HEMesh::Point from_point = mesh.point(mesh.from_vertex_handle(*fh_it));
            HEMesh::Point to_point = mesh.point(mesh.to_vertex_handle(*fh_it));
            face_area += from_point.cross(to_point);
        }
        area += 0.5f * abs(face_area.length());
    }
    return area;
}