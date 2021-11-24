// This source code is property of the Computer Graphics and Visualization 
// chair of the TU Dresden. Do not distribute! 
// Copyright (C) CGV TU Dresden - All Rights Reserved

#include "ShellExtraction.h"
#include <iostream>
#include "util/UnionFind.h"

using namespace std;
using namespace nse;

#include <queue>

unsigned int ExtractShells(HEMesh& m, OpenMesh::FPropHandleT<int> perFaceShellIndex)
{
	//reset the shell indices to -1 for every face
	for (auto f : m.faces())
		m.property(perFaceShellIndex, f) = -1;


	/*Task 1.2.3*/

	nse::util::UnionFind UF;
	//init to component per face
	int nr_comp = m.n_faces();
	//cout << "nr_comp: " << nr_comp;
	//UF.init
	UF.AddItems(nr_comp);

	HEMesh::FaceIter fi;
	HEMesh::HalfedgeIter hi;

	//initial IDs
	int i = 0;
	for (fi = m.faces_begin(); fi != m.faces_end(); ++fi) {
		m.property(perFaceShellIndex, m.handle(m.face(*fi))) = i;
		i++;
	}

	//iterate faces
	for (fi = m.faces_begin(); fi != m.faces_end(); ++fi) {
		HEMesh::FaceHalfedgeIter fhi = m.fh_iter(m.handle(m.face(*fi)));
		for (; fhi.is_valid(); ++fhi) {
			//cout << m.property(perFaceShellIndex, m.opposite_face_handle(m.handle(m.halfedge(*fhi)))) << endl;
			int currentFaceID = m.property(perFaceShellIndex, m.handle(m.face(*fi)));
			int oppositeFaceID = m.property(perFaceShellIndex, m.opposite_face_handle(m.handle(m.halfedge(*fhi))));
			if (UF.GetRepresentative(currentFaceID) != UF.GetRepresentative(oppositeFaceID)) {
				UF.Merge(currentFaceID, oppositeFaceID);
				//cout << "merge: " << currentFaceID << ", " << oppositeFaceID << endl;
				nr_comp--;
			}

		}
	}

	//cout << "size: " << UF.size() << endl;
	//cout << "connected components: " << nr_comp << endl;



	int i1 = 0;
	for (auto f : m.faces()) {
		m.property(perFaceShellIndex, f) = UF.GetRepresentative(i1);
		i1++;
	}




	return nr_comp;
}