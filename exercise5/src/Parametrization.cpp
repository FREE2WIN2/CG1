﻿// This source code is property of the Computer Graphics and Visualization 
// chair of the TU Dresden. Do not distribute! 
// Copyright (C) CGV TU Dresden - All Rights Reserved

#include "Parametrization.h"
#include <queue>
#include <Eigen/Sparse>
#include <Eigen/SparseLU>
#include <iostream>

bool IsTopologicalDisk(const HEMesh &m, OpenMesh::HalfedgeHandle &outBoundary) {
    std::vector<bool> vertexVisited(m.n_vertices());
    std::queue<HEMesh::VertexHandle> vQueue;
    vQueue.push(m.vertex_handle(0));
    size_t visitedVertices = 0;
    size_t visitedBoundaries = 0;

    //check if we can reach all vertices from the first vertex by region growing
    while (!vQueue.empty()) {
        auto v = vQueue.front();
        vQueue.pop();

        if (vertexVisited[v.idx()])
            continue;
        vertexVisited[v.idx()] = true;
        ++visitedVertices;

        //visit all the vertices along the boundary
        if (m.is_boundary(v)) {
            ++visitedBoundaries;
            auto h = m.halfedge_handle(v); //this will always be a boundary halfedge
            outBoundary = h;
            auto nextV = m.to_vertex_handle(h);
            while (!vertexVisited[nextV.idx()]) {
                vertexVisited[nextV.idx()] = true;
                ++visitedVertices;
                vQueue.push(nextV);
                h = m.next_halfedge_handle(h);
                nextV = m.to_vertex_handle(h);
            }
        }

        for (auto vn: m.vv_range(v))
            vQueue.push(vn);
    }

    return visitedVertices == m.n_vertices() && visitedBoundaries == 1;
}

template<>
float Weight<CONSTANT_WEIGHT>(HEMesh &m, OpenMesh::HalfedgeHandle h) {
    return 1;
}

template<>
float Weight<EDGE_LENGTH_WEIGHT>(HEMesh &m, OpenMesh::HalfedgeHandle h) {
    return abs(m.calc_edge_length(h));
}

template<>
float Weight<INV_EDGE_LENGTH_WEIGHT>(HEMesh &m, OpenMesh::HalfedgeHandle h) {
    return 1 / abs(m.calc_edge_length(h));
}

template<>
float Weight<COTAN_WEIGHT>(HEMesh &m, OpenMesh::HalfedgeHandle h) {
    float alpha = m.calc_sector_angle(h);
    float beta = m.calc_sector_angle(m.prev_halfedge_handle(h));
    return 1 / (tan(alpha) * tan(beta));
}

template<WeightType wtype>
bool ComputeParametrizationOfTopologicalDisk(HEMesh &mesh) {
    OpenMesh::HalfedgeHandle boundary;
    if (!IsTopologicalDisk(mesh, boundary)) {
        std::cout << "This mesh is not a topological disk." << std::endl;
        return false;
    }

    /* Task 5.2.1 */
    Eigen::Vector2f middle(0.5,0.5);
    float radius = 0.5;
    float radiusSquared = radius*radius;
    float length = mesh.calc_edge_length(boundary);
    float angle = acos(sqrt((radiusSquared * 2 - length * length)/2*radius*length)); //Sqrt(a²+b²-c² / 2ab)
    float x = middle.x() + radius * cos(angle);
    float y = middle.y() + radius * sin(angle);

    Eigen::Vector2f circlePos(x,y);

    return true;
}

template bool ComputeParametrizationOfTopologicalDisk<CONSTANT_WEIGHT>(HEMesh &mesh);

template bool ComputeParametrizationOfTopologicalDisk<EDGE_LENGTH_WEIGHT>(HEMesh &mesh);

template bool ComputeParametrizationOfTopologicalDisk<INV_EDGE_LENGTH_WEIGHT>(HEMesh &mesh);

template bool ComputeParametrizationOfTopologicalDisk<COTAN_WEIGHT>(HEMesh &mesh);