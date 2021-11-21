// This source code is property of the Computer Graphics and Visualization 
// chair of the TU Dresden. Do not distribute! 
// Copyright (C) CGV TU Dresden - All Rights Reserved

#include "Smoothing.h"

#include <random>
#include <iostream>
#include <OpenMesh/Tools/Smoother/LaplaceSmootherT.hh>

void SmoothUniformLaplacian(HEMesh &m, float lambda, unsigned int iterations) {
    /*Task 1.2.4*/
    if (iterations == 0) {
        return;
    }

    for (int i = 0; i < iterations; i++) {
        //Iterate through all vertices
        for (HEMesh::VertexIter v_it = m.vertices_begin(); v_it != m.vertices_end(); ++v_it) {
            HEMesh::Point &point = m.point(*v_it);
            /* init Vs */
            HEMesh::Point vs = OpenMesh::VectorT<float, 3>(0, 0, 0);
            /* Get Neighbours*/
            int neighbours = 0;
            for (auto h_it = v_it->outgoing_halfedges().begin(); h_it != v_it->outgoing_halfedges().end(); ++h_it) {
                HEMesh::Point to_point = m.point(m.to_vertex_handle(*h_it));
                vs += to_point;
                neighbours++;
            }

            vs = vs / neighbours;
            point += lambda * (vs - point);
        }
    }

}

void AddNoise(HEMesh &m, OpenMesh::MPropHandleT<Viewer::BBoxType> bbox_prop) {
    std::mt19937 rnd;
    std::normal_distribution<float> dist;

    for (auto v: m.vertices()) {
        OpenMesh::Vec3f n;
        m.calc_vertex_normal_correct(v, n);
        const auto diag = m.property(bbox_prop).diagonal();
        const float base_diag = std::min(diag.x(), std::min(diag.y(), diag.z())) / 20.f;
        float base_nb = 0, nb_num = 0;
        for (auto vnb: m.vv_range(v)) {
            base_nb += (m.point(v) - m.point(vnb)).norm();
            nb_num++;
        }
        base_nb /= 4.f * nb_num;

        m.point(v) += std::min(base_diag, base_nb) * dist(rnd) * n.normalized();
    }
}
