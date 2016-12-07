//////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016, Andrew Dornbush
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the copyright holder nor the names of its
//       contributors may be used to endorse or promote products derived from
//       this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//////////////////////////////////////////////////////////////////////////////

#ifndef SBPL_GEOMETRY_MESH_UTILS_H
#define SBPL_GEOMETRY_MESH_UTILS_H

// standard includes
#include <vector>

// system includes
#include <Eigen/Dense>

// project includes
#include <smpl/geometry/voxel_grid.h>

namespace sbpl {
namespace geometry {

void CreateIndexedBoxMesh(
    double length,
    double width,
    double height,
    std::vector<Eigen::Vector3d>& vertices,
    std::vector<int>& indices);

void CreateIndexedSphereMesh(
    double radius,
    int lng_count,
    int lat_count,
    std::vector<Eigen::Vector3d>& vertices,
    std::vector<int>& triangles);

void CreateIndexedCylinderMesh(
    double radius,
    double height,
    std::vector<Eigen::Vector3d>& vertices,
    std::vector<int>& indices);

void CreateIndexedConeMesh(
    double radius,
    double height,
    std::vector<Eigen::Vector3d>& vertices,
    std::vector<int>& indices);

void CreateIndexedPlaneMesh(
    double a, double b, double c, double d,
    const Eigen::Vector3d& min,
    const Eigen::Vector3d& max,
    std::vector<Eigen::Vector3d>& vertices,
    std::vector<int>& indices);

/// \brief Create a mesh representation of a grid
template <typename Discretizer>
void CreateIndexedGridMesh(
    const VoxelGrid<Discretizer>& vg,
    std::vector<Eigen::Vector3d>& vertices,
    std::vector<int>& indices);

void CreateBoxMesh(
    double length,
    double width,
    double height,
    std::vector<Eigen::Vector3d>& vertices);

/// \brief Create a non-indexed mesh representation of a grid
template <typename Discretizer>
void CreateGridMesh(
    const VoxelGrid<Discretizer>& vg,
    std::vector<Eigen::Vector3d>& vertices);

} // namespace geometry
} // namespace sbpl

#include "detail/mesh_utils.h"

#endif

