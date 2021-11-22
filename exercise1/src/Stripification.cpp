// This source code is property of the Computer Graphics and Visualization 
// chair of the TU Dresden. Do not distribute! 
// Copyright (C) CGV TU Dresden - All Rights Reserved

#include "Stripification.h"

#include <random>
#include "sample_set.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include "Primitives.h"

// traverses halfedges forwards
HEMesh::HalfedgeHandle forward(HEMesh mesh, HEMesh::HalfedgeHandle currentHalfedge, unsigned int parity) {

	HEMesh::HalfedgeHandle nextHalfedge = mesh.opposite_halfedge_handle(currentHalfedge);

	if (parity == 0) {
		nextHalfedge = mesh.prev_halfedge_handle(nextHalfedge);
	}
	else {
		nextHalfedge = mesh.next_halfedge_handle(nextHalfedge);
	}

	return nextHalfedge;

}

// traverses halfedges backwards
HEMesh::HalfedgeHandle backward(HEMesh mesh, HEMesh::HalfedgeHandle currentHalfedge, unsigned int parity) {

	HEMesh::HalfedgeHandle nextHalfedge;

	if (parity == 0) {
		nextHalfedge = mesh.prev_halfedge_handle(currentHalfedge);
	}
	else {
		nextHalfedge = mesh.next_halfedge_handle(currentHalfedge);
	}

	nextHalfedge = mesh.opposite_halfedge_handle(nextHalfedge);

	return nextHalfedge;

}

// converts a mesh into a sample_set, helper function
sample_set<HEMesh::FaceHandle> convertToSampleSet(HEMesh mesh) {

	sample_set<HEMesh::FaceHandle> meshSet;
	HEMesh::FaceIter f_it = mesh.faces_begin();


	for (f_it; f_it != mesh.faces_end(); f_it++) {
		HEMesh::FaceHandle face_i = mesh.face_handle(f_it->idx());
		meshSet.insert(face_i);
	}

	return meshSet;

}

// gets a triangle strip starting with the given face
std::vector<HEMesh::FaceHandle> getTriangleStrip(HEMesh mesh, OpenMesh::FPropHandleT<int> perFaceStripIdProperty, HEMesh::FaceHandle startingFace) {
	int parity = 0;
	std::vector<HEMesh::FaceHandle> currentStrip{};
	HEMesh::HalfedgeHandle currentHalfedge = mesh.halfedge_handle(startingFace);
	HEMesh::FaceHandle currentFace = startingFace;

	if (!startingFace.is_valid() || mesh.is_boundary(startingFace)) {
		return currentStrip;
	}

	// fügt faces vorwärts hinzu
	while (mesh.property(perFaceStripIdProperty, currentFace) == -1 && std::find(currentStrip.begin(), currentStrip.end(), currentFace) == currentStrip.end()) {
		currentStrip.push_back(currentFace);
		if (parity == 0) {
			parity = 1;
		}
		else {
			parity = 0;
		}

		currentHalfedge = forward(mesh, currentHalfedge, parity);
		if (!currentHalfedge.is_valid()) {
			break;
		}
		else if(mesh.is_boundary(currentHalfedge)) {
			break;
		}
		currentFace = mesh.face_handle(currentHalfedge);
		
	}

	return currentStrip;
}


unsigned int ExtractTriStrips(HEMesh& mesh, OpenMesh::FPropHandleT<int> perFaceStripIdProperty, unsigned int nTrials)
{
	//prepare random engine
	std::mt19937 eng;
	

	//initialize strip index to -1 for each face
	for (auto f : mesh.faces())
		mesh.property(perFaceStripIdProperty, f) = -1;

	int nStrips = 0;

	/*Task 1.2.5*/

	// iterators
	HEMesh::HalfedgeIter he_it = mesh.halfedges_begin();
	HEMesh::FaceIter f_it = mesh.faces_begin();
	HEMesh::FaceHandle fh;
	std::vector< std::vector<HEMesh::FaceHandle> > triangleStrips;
	int index = 0;
	int totalSize = 0;
	signed int oldMeshSetSize = 0;
	int counter = 0;

	sample_set<HEMesh::FaceHandle> meshSet = convertToSampleSet(mesh);

	// loops until there are no untagged faces left
	while (meshSet.size() > 0 && totalSize < mesh.n_faces()) {
		std::vector<HEMesh::FaceHandle> largestTriangleStrip;
		std::vector<HEMesh::FaceHandle> currentTriangleStrip;
		// tries nTrials different seeds
		for (int i = 0; i < nTrials; i++) {
			HEMesh::FaceHandle randomFace = meshSet.sample(eng);
			if (randomFace.is_valid() && !mesh.is_boundary(randomFace)) {
				currentTriangleStrip = getTriangleStrip(mesh, perFaceStripIdProperty, randomFace);
			}
			else {
				break;
			}
			if (currentTriangleStrip.size() > largestTriangleStrip.size()) {
				largestTriangleStrip = currentTriangleStrip;
			}
		}
		totalSize += largestTriangleStrip.size();

		// for debugging
		/*if (!largestTriangleStrip.empty()) {
			std::cout << std::endl;
			std::cout << "Seed Face is ";
			std::cout << largestTriangleStrip.front() << std::endl;
			std::cout << "Final Face is ";
			std::cout << largestTriangleStrip.back() << std::endl;
			std::cout << "Size: ";
			std::cout << largestTriangleStrip.size();
			std::cout << " out of ";
			std::cout << mesh.n_faces();
			std::cout << " faces" << std::endl;
			std::cout << "Total size: ";
			std::cout << totalSize;
			std::cout << "/";
			std::cout << mesh.n_faces() << std::endl;
			std::cout << "Triangle strips found so far: ";
			std::cout << triangleStrips.size() << std::endl;
			std::cout << meshSet.size() << std::endl;
			std::cout << std::endl;
		}*/

		oldMeshSetSize = meshSet.size();

		triangleStrips.push_back(largestTriangleStrip);
		// setting IDs
		for (HEMesh::FaceHandle face : largestTriangleStrip) {
			meshSet.remove(face);
			mesh.property(perFaceStripIdProperty, face) = index;
		}
		index++;
		// resetting vectors
		largestTriangleStrip.clear();
		currentTriangleStrip.clear();

	}

	nStrips = triangleStrips.size();

	return nStrips;
}