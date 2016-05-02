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

#include "imstkSceneObject.h"

namespace imstk {
std::shared_ptr<Geometry>
SceneObject::getVisualGeometry() const
{
    return m_visualGeometry;
}

void
SceneObject::setVisualGeometry(std::shared_ptr<Geometry>geometry)
{
    m_visualGeometry = geometry;
}

const SceneObjectType&
SceneObject::getType() const
{
    return m_type;
}

const std::string&
SceneObject::getName() const
{
    return m_name;
}

void
SceneObject::setName(std::string name)
{
    m_name = name;
}
}
