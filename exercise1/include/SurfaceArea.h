// This source code is property of the Computer Graphics and Visualization 
// chair of the TU Dresden. Do not distribute! 
// Copyright (C) CGV TU Dresden - All Rights Reserved

#pragma once

#include "util/OpenMeshUtils.h"

//computes the surface area of the halfedge mesh mesh by computing and summing up the
//areas of all polygonal faces
//method is not restricted to triangle meshes
float ComputeSurfaceArea(HEMesh mesh);