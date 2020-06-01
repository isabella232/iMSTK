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

#include "imstkAbstractDynamicalModel.h"
#include "imstkComputeGraph.h"
#include "imstkLogger.h"

namespace imstk
{
AbstractDynamicalModel::AbstractDynamicalModel(DynamicalModelType type) :
    m_type(type), m_numDOF(0), m_computeGraph(std::make_shared<ComputeGraph>("AbstractDynamicalModel_Source", "AbstractDynamicalModel_Sink"))
{
}

bool
AbstractDynamicalModel::isGeometryValid(const std::shared_ptr<Geometry> geometry)
{
    if (geometry)
    {
        // If no valid geometries specified all geometries work
        if (m_validGeometryTypes.size() == 0)
        {
            return true;
        }

        // If it exists in the set then it is valid geometry
        if (m_validGeometryTypes.count(geometry->getType()))
        {
            return true;
        }
        else
        {
            LOG(WARNING) << "The geometry is not supported!!";
        }
    }
    else
    {
        LOG(WARNING) << "The geometry is not a valid pointer";
    }

    return false;
}

void
AbstractDynamicalModel::setModelGeometry(std::shared_ptr<Geometry> geometry)
{
    if (isGeometryValid(geometry))
    {
        m_geometry = geometry;
    }
    else
    {
        LOG(WARNING) << "Invalid geometry for Model";
    }
}

void
AbstractDynamicalModel::initGraphEdges()
{
    m_computeGraph->clearEdges();
    initGraphEdges(m_computeGraph->getSource(), m_computeGraph->getSink());
}

void
AbstractDynamicalModel::initGraphEdges(std::shared_ptr<ComputeNode> source, std::shared_ptr<ComputeNode> sink)
{
    m_computeGraph->addEdge(source, sink);
}

std::shared_ptr<ComputeNode>
AbstractDynamicalModel::addFunction(std::string name, std::function<void()> func)
{
    return m_computeGraph->addFunction(name, func);
}

void
AbstractDynamicalModel::addEdge(std::shared_ptr<ComputeNode> srcNode, std::shared_ptr<ComputeNode> destNode)
{
    m_computeGraph->addEdge(srcNode, destNode);
}
} // imstk