// This source code is property of the Computer Graphics and Visualization 
// chair of the TU Dresden. Do not distribute! 
// Copyright (C) CGV TU Dresden - All Rights Reserved

#include "SurfaceArea.h"

#include <iostream>


/* still dont work //TODO*/
float ComputeSurfaceArea(HEMesh& mesh) {
    float area = 0.0;
    /* Task 1.2.2 */
    HEMesh::VertexIter v_it, v_end(mesh.vertices_end());
    HEMesh::VertexFaceIter     vf_it;

    /* Iterate over vertices */
    for (v_it = mesh.vertices_begin(); v_it != v_end; ++v_it) {


        /* Iterate over faces */
        for (vf_it = mesh.vf_iter(v_it); vf_it; ++vf_it) {
            HEMesh::PolyConnectivity::FaceVertexIter fv_it = mesh.fv_iter(vf_it);

            const OpenMesh::Vec3f P = mesh.point(fv_it);
            ++fv_it;
            const OpenMesh::Vec3f Q = mesh.point(fv_it);
            ++fv_it;
            const OpenMesh::Vec3f R = mesh.point(fv_it);
            std::cout << (((Q - P) % (R - P)).norm() * 0.5f * 0.3333f) << std::endl;
            area += ((Q - P) % (R - P)).norm() * 0.5f * 0.3333f;    // norm: compute euclidean norm, return Scalar
        }
        std::cout << "area ";
        std::cout << area << std::endl;
    }
    return area;
}