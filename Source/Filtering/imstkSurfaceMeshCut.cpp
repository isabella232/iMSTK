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

#include "imstkSurfaceMeshCut.h"

#include "imstkGeometryUtilities.h"
#include "imstkLogger.h"
#include "imstkPlane.h"

#include <map>
#include <set>

namespace imstk
{
SurfaceMeshCut::SurfaceMeshCut()
{
    setNumberOfInputPorts(1);
    setNumberOfOutputPorts(1);
    setOutput(std::make_shared<SurfaceMesh>());
}

std::shared_ptr<SurfaceMesh>
SurfaceMeshCut::getOutputMesh()
{
    return std::dynamic_pointer_cast<SurfaceMesh>(getOutput(0));
}

void
SurfaceMeshCut::setInputMesh(std::shared_ptr<SurfaceMesh> inputMesh)
{
    setInput(inputMesh, 0);
}

void
SurfaceMeshCut::requestUpdate()
{
    // input and output SurfaceMesh
    std::shared_ptr<SurfaceMesh> inputSurf = std::dynamic_pointer_cast<SurfaceMesh>(getInput(0));
    if (inputSurf == nullptr)
    {
        LOG(WARNING) << "Missing required SurfaceMesh input";
        return;
    }
    std::shared_ptr<SurfaceMesh> outputSurf = std::dynamic_pointer_cast<SurfaceMesh>(getOutput(0));
    outputSurf->deepCopy(inputSurf);

    // vertices on the cutting path and whether they will be split
    std::map<int, bool> cutVerts;

    generateCutData(m_Plane, outputSurf);

    // refinement
    refinement(outputSurf, cutVerts);

    // split cutting vertices
    splitVerts(outputSurf, cutVerts);

    // vtkPolyData to output SurfaceMesh
    setOutput(outputSurf);
}

void
SurfaceMeshCut::refinement(std::shared_ptr<SurfaceMesh> outputSurf, std::map<int, bool>& cutVerts)
{
    // map between one exsiting edge to the new vert generated from the cutting
    std::map<std::pair<int, int>, int> edgeVertMap;

    auto triangles = outputSurf->getTriangleIndices();
    auto vertices  = outputSurf->getVertexPositions();

    for (size_t i = 0; i < m_CutData->size(); i++)
    {
        CutData curCutData = m_CutData->at(i);
        auto    curCutType = curCutData.cutType;
        int     triId      = curCutData.triId;
        int     ptId0      = curCutData.ptIds[0];
        int     ptId1      = curCutData.ptIds[1];
        Vec3d   cood0      = curCutData.cutCoords[0];
        Vec3d   cood1      = curCutData.cutCoords[1];

        if (curCutType == CutType::EDGE || curCutType == CutType::EDGE_VERT)
        {
            int  newPtId = -1;
            auto edge0   = std::make_pair(ptId0, ptId1);
            auto edge1   = std::make_pair(ptId1, ptId0);

            // add new point
            if (edgeVertMap.find(edge1) != edgeVertMap.end())
            {
                newPtId = edgeVertMap[edge1];
            }
            else
            {
                newPtId = vertices->size();
                vertices->push_back(cood0);
                edgeVertMap[edge0] = newPtId;
            }

            // update triangle indices
            Vec3i ptIds = (*triangles)[triId];
            int   ptId2 = -1;
            if (ptIds[0] != ptId0 && ptIds[0] != ptId1)
            {
                ptId2 = ptIds[0];
            }
            else if (ptIds[1] != ptId0 && ptIds[1] != ptId1)
            {
                ptId2 = ptIds[1];
            }
            else
            {
                ptId2 = ptIds[2];
            }
            (*triangles)[triId] = Vec3i(ptId2, ptId0, newPtId);
            triangles->push_back(Vec3i(ptId2, newPtId, ptId1));

            // add vertices to cutting path
            if (curCutType == CutType::EDGE_VERT)
            {
                cutVerts[ptId2]   = (cutVerts.find(ptId2) != cutVerts.end()) ? true : false;
                cutVerts[newPtId] = (cutVerts.find(newPtId) != cutVerts.end()) ? true : false;
            }
        }
        else if (curCutType == CutType::EDGE_EDGE)
        {
            int newPtId0 = -1;
            int newPtId1 = -1;

            Vec3i ptIds = (*triangles)[triId];
            int   ptId2 = -1;
            if (ptIds[0] != ptId0 && ptIds[0] != ptId1)
            {
                ptId2 = ptIds[0];
            }
            else if (ptIds[1] != ptId0 && ptIds[1] != ptId1)
            {
                ptId2 = ptIds[1];
            }
            else
            {
                ptId2 = ptIds[2];
            }

            auto edge00 = std::make_pair(ptId2, ptId0);
            auto edge01 = std::make_pair(ptId0, ptId2);
            auto edge10 = std::make_pair(ptId1, ptId2);
            auto edge11 = std::make_pair(ptId2, ptId1);

            // add new points
            if (edgeVertMap.find(edge01) != edgeVertMap.end())
            {
                newPtId0 = edgeVertMap[edge01];
            }
            else
            {
                newPtId0 = vertices->size();
                vertices->push_back(cood0);
                edgeVertMap[edge00] = newPtId0;
            }
            if (edgeVertMap.find(edge11) != edgeVertMap.end())
            {
                newPtId1 = edgeVertMap[edge11];
            }
            else
            {
                newPtId1 = vertices->size();
                vertices->push_back(cood1);
                edgeVertMap[edge10] = newPtId1;
            }

            // update triangle indices
            (*triangles)[triId] = Vec3i(ptId2, newPtId0, newPtId1);
            triangles->push_back(Vec3i(newPtId0, ptId0, ptId1));
            triangles->push_back(Vec3i(newPtId0, ptId1, newPtId1));

            // add vertices to cutting path
            cutVerts[newPtId0] = (cutVerts.find(newPtId0) != cutVerts.end()) ? true : false;
            cutVerts[newPtId1] = (cutVerts.find(newPtId1) != cutVerts.end()) ? true : false;
        }
        else if (curCutType == CutType::VERT_VERT)
        {
            // add vertices to cutting path
            cutVerts[ptId0] = (cutVerts.find(ptId0) != cutVerts.end()) ? true : false;
            cutVerts[ptId1] = (cutVerts.find(ptId1) != cutVerts.end()) ? true : false;
        }
        else
        {
            //throw error
        }
    }
    outputSurf->setInitialVertexPositions(std::make_shared<VecDataArray<double, 3>>(*vertices));
    outputSurf->setVertexPositions(std::make_shared<VecDataArray<double, 3>>(*vertices));
    outputSurf->setTriangleIndices(std::make_shared<VecDataArray<int, 3>>(*triangles));
    outputSurf->modified();
}

void
SurfaceMeshCut::splitVerts(std::shared_ptr<SurfaceMesh> outputSurf, std::map<int, bool>& cutVerts)
{
    auto triangles = outputSurf->getTriangleIndices();
    auto vertices  = outputSurf->getVertexPositions();

    // build vertexToTriangleListMap
    std::vector<std::set<int>> vertexNeighborTriangles;
    vertexNeighborTriangles.clear();
    vertexNeighborTriangles.resize(vertices->size());

    int triangleId = 0;

    for (const auto& t : *triangles)
    {
        vertexNeighborTriangles.at(t[0]).insert(triangleId);
        vertexNeighborTriangles.at(t[1]).insert(triangleId);
        vertexNeighborTriangles.at(t[2]).insert(triangleId);
        triangleId++;
    }

    // split cutting vertices
    for (const auto& cutVert : cutVerts)
    {
        if (cutVert.second == false && !vertexOnBoundary(triangles, vertexNeighborTriangles[cutVert.first]))
        {
            // do not split vertex since it's the cutting end in surface
        }
        else
        {
            // split vertex
            int newPtId = vertices->size();
            vertices->push_back((*vertices)[cutVert.first]);

            for (const auto& t : vertexNeighborTriangles[cutVert.first])
            {
                //if triangle on the negative side
                Vec3d pt0 = (*vertices)[(*triangles)[t][0]];
                Vec3d pt1 = (*vertices)[(*triangles)[t][1]];
                Vec3d pt2 = (*vertices)[(*triangles)[t][2]];

                if (pointOnPlaneSide(pt0) < 0 || pointOnPlaneSide(pt1) < 0 || pointOnPlaneSide(pt2) < 0)
                {
                    for (int i = 0; i < 3; i++)
                    {
                        if ((*triangles)[t][i] == cutVert.first)
                        {
                            (*triangles)[t][i] = newPtId;
                        }
                    }
                }
            }
        }
    }

    outputSurf->setInitialVertexPositions(std::make_shared<VecDataArray<double, 3>>(*vertices));
    outputSurf->setVertexPositions(std::make_shared<VecDataArray<double, 3>>(*vertices));
    outputSurf->setTriangleIndices(std::make_shared<VecDataArray<int, 3>>(*triangles));
    outputSurf->modified();
}

int
SurfaceMeshCut::pointOnPlaneSide(Vec3d pt)
{
    double normalProjection = m_Plane->getNormal().dot(pt - m_Plane->getPosition());
    if (normalProjection > m_Epsilon)
    {
        return 1;
    }
    else if (normalProjection < -m_Epsilon)
    {
        return -1;
    }
    else
    {
        return 0;
    }
    return 0;
}

bool
SurfaceMeshCut::vertexOnBoundary(std::shared_ptr<VecDataArray<int, 3>> triangleIndices, std::set<int>& triSet)
{
    std::set<int> nonRepeatNeighborVerts;
    for (const auto& t : triSet)
    {
        for (int i = 0; i < 3; i++)
        {
            int ptId = (*triangleIndices)[t][i];
            if (nonRepeatNeighborVerts.find(ptId) != nonRepeatNeighborVerts.end())
            {
                nonRepeatNeighborVerts.erase(ptId);
            }
            else
            {
                nonRepeatNeighborVerts.insert(ptId);
            }
        }
    }
    if (nonRepeatNeighborVerts.size() >= 2)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void
SurfaceMeshCut::generateCutData(std::shared_ptr<Plane> plane, std::shared_ptr<SurfaceMesh> inputSurf)
{
    auto triangles = inputSurf->getTriangleIndices();
    auto vertices  = inputSurf->getVertexPositions();

    m_CutData->clear();
    std::set<std::pair<int, int>> repeatEdges;  // make sure not boundary edge in vert-vert case

    for (int i = 0; i < triangles->size(); i++)
    {
        auto&   tri = (*triangles)[i];
        CutData newCutData;

        auto ptSide = Vec3i(pointOnPlaneSide((*vertices)[tri[0]]),
                            pointOnPlaneSide((*vertices)[tri[1]]),
                            pointOnPlaneSide((*vertices)[tri[2]]));

        switch (ptSide.squaredNorm())
        {
        case 1:
            for (int j = 0; j < 3; j++)
            {
                if (ptSide[j] != 0)
                {
                    int idx0  = (j + 1) % 3;
                    int idx1  = (j + 2) % 3;
                    int ptId0 = tri[idx0];
                    int ptId1 = tri[idx1];

                    std::pair<int, int> cutEdge = std::make_pair(ptId1, ptId0);
                    // if find cut triangle from the other side of the edge, add newCutData
                    if (repeatEdges.find(cutEdge) != repeatEdges.end())
                    {
                        newCutData.cutType      = CutType::VERT_VERT;
                        newCutData.triId        = i;
                        newCutData.ptIds[0]     = ptId0;
                        newCutData.ptIds[1]     = ptId1;
                        newCutData.cutCoords[0] = (*vertices)[ptId0];
                        newCutData.cutCoords[1] = (*vertices)[ptId1];
                        m_CutData->push_back(newCutData);
                    }
                    else
                    {
                        repeatEdges.insert(std::make_pair(ptId0, ptId1));
                    }
                }
            }
            break;
        case 2:
            if (ptSide.sum() == 0)
            {
                for (int j = 0; j < 3; j++)
                {
                    if (ptSide[j] == 0)
                    {
                        int   idx0  = (j + 1) % 3;
                        int   idx1  = (j + 2) % 3;
                        int   ptId0 = tri[idx0];
                        int   ptId1 = tri[idx1];
                        Vec3d pos0  = (*vertices)[ptId0];
                        Vec3d pos1  = (*vertices)[ptId1];
                        Vec3d p     = m_Plane->getPosition();
                        Vec3d n     = m_Plane->getNormal();

                        newCutData.cutType      = CutType::EDGE_VERT;
                        newCutData.triId        = i;
                        newCutData.ptIds[0]     = ptId0;
                        newCutData.ptIds[1]     = ptId1;
                        newCutData.cutCoords[0] = (p - pos0).dot(n) / (pos1 - pos0).dot(n) * (pos1 - pos0) + pos0;
                        newCutData.cutCoords[1] = (*vertices)[tri[j]];
                        m_CutData->push_back(newCutData);
                    }
                }
            }
            else
            {
                // newCutData.cutType = CutType::VERT;
            }
            break;
        case 3:
            if (ptSide.sum() == 1)
            {
                for (int j = 0; j < 3; j++)
                {
                    if (ptSide[j] < 0)
                    {
                        int   idx0  = (j + 1) % 3;
                        int   idx1  = (j + 2) % 3;
                        int   ptId0 = tri[idx0];
                        int   ptId1 = tri[idx1];
                        int   ptId2 = tri[j];
                        Vec3d pos0  = (*vertices)[ptId0];
                        Vec3d pos1  = (*vertices)[ptId1];
                        Vec3d pos2  = (*vertices)[ptId2];
                        Vec3d p     = m_Plane->getPosition();
                        Vec3d n     = m_Plane->getNormal();

                        newCutData.cutType      = CutType::EDGE_EDGE;
                        newCutData.triId        = i;
                        newCutData.ptIds[0]     = ptId0;
                        newCutData.ptIds[1]     = ptId1;
                        newCutData.cutCoords[0] = (p - pos0).dot(n) / (pos2 - pos0).dot(n) * (pos2 - pos0) + pos0;
                        newCutData.cutCoords[1] = (p - pos1).dot(n) / (pos2 - pos1).dot(n) * (pos2 - pos1) + pos1;
                        m_CutData->push_back(newCutData);
                    }
                }
            }
            else if (ptSide.sum() == -1)
            {
                for (int j = 0; j < 3; j++)
                {
                    if (ptSide[j] > 0)
                    {
                        int   idx0  = (j + 1) % 3;
                        int   idx1  = (j + 2) % 3;
                        int   ptId0 = tri[idx0];
                        int   ptId1 = tri[idx1];
                        int   ptId2 = tri[j];
                        Vec3d pos0  = (*vertices)[ptId0];
                        Vec3d pos1  = (*vertices)[ptId1];
                        Vec3d pos2  = (*vertices)[ptId2];
                        Vec3d p     = m_Plane->getPosition();
                        Vec3d n     = m_Plane->getNormal();

                        newCutData.cutType      = CutType::EDGE_EDGE;
                        newCutData.triId        = i;
                        newCutData.ptIds[0]     = ptId0;
                        newCutData.ptIds[1]     = ptId1;
                        newCutData.cutCoords[0] = (p - pos0).dot(n) / (pos2 - pos0).dot(n) * (pos2 - pos0) + pos0;
                        newCutData.cutCoords[1] = (p - pos1).dot(n) / (pos2 - pos1).dot(n) * (pos2 - pos1) + pos1;
                        m_CutData->push_back(newCutData);
                    }
                }
            }
            else
            {
                // no intersection
            }
            break;
        default:
            break;
        }
    }
}
}