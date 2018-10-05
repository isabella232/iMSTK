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

#ifndef imstkVulkanLineMeshRenderDelegate_h
#define imstkVulkanLineMeshRenderDelegate_h

#include <memory>

#include "imstkVulkanRenderDelegate.h"
#include "imstkLineMesh.h"
#include "imstkVulkanMemoryManager.h"

namespace imstk
{
class VulkanLineMeshRenderDelegate : public VulkanRenderDelegate
{
public:

    ///
    /// \brief Default destructor
    ///
    ~VulkanLineMeshRenderDelegate() = default;

    ///
    /// \brief Default constructor
    ///
    VulkanLineMeshRenderDelegate(std::shared_ptr<LineMesh> LineMesh,
                                 SceneObject::Type type,
                                 VulkanMemoryManager& memoryManager);

    ///
    /// \brief Update render geometry
    ///
    void update(const uint32_t frameIndex) override;

    ///
    /// \brief Get source geometry
    ///
    std::shared_ptr<Geometry> getGeometry() const override;


    ///
    /// \brief Fill vertex buffer
    ///
    void updateVertexBuffer(const uint32_t frameIndex);

protected:
    std::shared_ptr<LineMesh> m_geometry;

    VulkanLocalVertexUniforms m_localVertexUniforms;
};
}

#endif