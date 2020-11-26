/*=========================================================================

   Library: iMSTK

   Copyright (c) Kitware, Inc. & Center for Modeling, Simulation,
   & Imaging in Medicine, Rensselaer Polytechnic Institute.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

=========================================================================*/

#pragma once

#include "imstkTextureManager.h"
#include "imstkVTKPolyDataRenderDelegate.h"
#include "imstkVTKTextureDelegate.h"

class vtkCellArray;
class vtkDataArray;
class vtkDoubleArray;
class vtkFloatArray;
class vtkPolyData;

namespace imstk
{
class SurfaceMesh;
template<typename T, int N> class VecDataArray;

///
/// \class VTKSurfaceMeshRenderDelegate
///
/// \brief Surface mesh render delegate with VTK backend
///
class VTKSurfaceMeshRenderDelegate : public VTKPolyDataRenderDelegate
{
public:
    ///
    /// \brief Constructor
    ///
    VTKSurfaceMeshRenderDelegate(std::shared_ptr<VisualModel> visualModel);

    ///
    /// \brief Destructor
    ///
    virtual ~VTKSurfaceMeshRenderDelegate() override = default;

    ///
    /// \brief Event handler
    ///
    void processEvents() override;

    ///
    /// \brief Initialize textures
    ///
    void initializeTextures(TextureManager<VTKTextureDelegate>& textureManager);

protected:
    ///
    /// \brief Callback for when vertex values are modified
    ///
    void vertexDataModified(Event* e);
    //void indexDataModified(Event* e);

    ///
    /// \brief Callback for when geometry is modified
    ///
    void geometryModified(Event* e);

protected:
    std::shared_ptr<VecDataArray<double, 3>> m_vertices;
    std::shared_ptr<VecDataArray<double, 3>> m_normals;
    std::shared_ptr<VecDataArray<int, 3>>    m_indices;

    vtkSmartPointer<vtkPolyData> m_polydata;

    vtkSmartPointer<vtkDoubleArray> m_mappedVertexArray;       ///> Mapped array of vertices
    vtkSmartPointer<vtkDoubleArray> m_mappedNormalArray;       ///> Mapped array of normals
    vtkSmartPointer<vtkDoubleArray> m_mappedTangentArray;      ///> Mapped array of tangents
    vtkSmartPointer<vtkFloatArray>  m_mappedTCoordsArray;      ///> Mapped array of tcoords
    vtkSmartPointer<vtkDataArray>   m_mappedVertexScalarArray; ///> Mapped array of scalars
    vtkSmartPointer<vtkDataArray>   m_mappedCellScalarArray;   ///> Mapped array of scalars
    vtkSmartPointer<vtkCellArray>   m_cellArray;               ///> Array of cells
};
}
