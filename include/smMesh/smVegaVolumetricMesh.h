// This file is part of the SimMedTK project.
// Copyright (c) Center for Modeling, Simulation, and Imaging in Medicine,
//                        Rensselaer Polytechnic Institute
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//---------------------------------------------------------------------------
//
// Authors:
//
// Contact:
//---------------------------------------------------------------------------

#ifndef SMVEGAMESH_H
#define SMVEGAMESH_H

// STL includes
#include <iostream>
#include <memory>
#include <map>

// SimMedTK includes
#include "smMesh/smSurfaceMesh.h"

// VEGA includes
#include "volumetricMesh.h"
#include "generateMeshGraph.h"
#include "cubicMesh.h"
#include "tetMesh.h"
#include "graph.h"

//
// Interface to VegaFEM's volumetric mesh class
//
class smVegaVolumetricMesh
{
public:
    ///
    /// @brief Constructor
    ///
    smVegaVolumetricMesh(bool generateMeshGraph = true);

    ///
    /// @brief Destructor
    ///
    ~smVegaVolumetricMesh();

    ///
    /// @brief Loads vega volume mesh and stores it locally
    ///
    void loadMesh(const std::string &fileName, const int &verbose);

    ///
    /// @brief Returns graph
    /// Note that this does not check if the graph is valid
    ///
    std::shared_ptr<Graph> getMeshGraph();

    ///
    /// @brief Returns the total number of vertices in the mesh
    ///
    size_t getNumberOfVertices() const;

    ///
    /// @brief Returns the total number of elements in the mesh
    ///
    size_t getNumberOfElements() const;

    ///
    /// @brief Attach surface mesh to the volume mesh and stores interpolation weights
    ///
    void attachSurfaceMesh(std::shared_ptr<smSurfaceMesh> surfaceMesh, const double &radius = -1.0);

    ///
    /// @brief Returns weigths associated with surface ith mesh
    ///
    const std::vector<double> &getAttachedWeights(const size_t &i) const;

    ///
    /// @brief Return vertice for interpolation weights
    ///
    const std::vector<int> &getAttachedVertices(const size_t &i) const;

private:
    // Vega mesh base object
    std::shared_ptr<VolumetricMesh> mesh;

    // Vega mesh graph
    std::shared_ptr<Graph> meshGraph;

    // Generate graph for this mesh
    bool generateGraph;

    // Store pointer to the surface meshes
    std::vector<std::shared_ptr<smSurfaceMesh>> attachedMeshes;

    // Store map of vertices
    std::map<std::shared_ptr<smSurfaceMesh>,std::vector<int>> attachedVertices;

    // Store map of  weigths
    std::map<std::shared_ptr<smSurfaceMesh>,std::vector<double>> attachedWeights;

};

#endif // SMVEGAMESH_H
