// This source code is property of the Computer Graphics and Visualization 
// chair of the TU Dresden. Do not distribute! 
// Copyright (C) CGV TU Dresden - All Rights Reserved

#include "Volume.h"

#include <iostream>



float ComputeVolume(HEMesh &mesh) {
    float vol = 0;

    /* Mesh is already triangulated!*/

    int number_of_faces = 0;
    int number_of_vertices = 0;
    int number_of_edges = 0;

    for (HEMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it) {
        number_of_faces++;
    }
    for (HEMesh::VertexIter v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it) {
        number_of_vertices++;
    }
    for (HEMesh::EdgeIter e_it = mesh.edges_begin(); e_it != mesh.edges_end(); ++e_it) {
        number_of_edges++;
    }

    int check_sum = number_of_vertices + number_of_faces - number_of_edges;
    if(check_sum != 2){
        std::cout << "No closed 3D Object! return 0" << std::endl;
        return vol;
    }


    for (HEMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it) {
        /* Now we have triangle surfaces! */

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