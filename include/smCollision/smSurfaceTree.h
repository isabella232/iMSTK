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

#ifndef SM_SMSURFACETREE_H
#define SM_SMSURFACETREE_H

// STD includes
#include <vector>
#include <array>

// SimMedTK includes
#include "smCollision/smCollisionModel.h"
#include "smCollision/smSurfaceTreeCell.h"
#include "smCollision/smCollisionMoller.h"
#include "smMesh/smSurfaceMesh.h"

/// \brief !!
template<typename CellType>
class smSurfaceTree : public smCoreClass
{
protected:
  typedef smMatrix44d MatrixType;

protected:
    std::shared_ptr<smSurfaceMesh> mesh; 							///< surface mesh
    int minTreeRenderLevel; 						///< !!
    bool renderSurface; 							///< !!
    bool enableShiftPos; 							///< !!
    bool enableTrianglePos; 						///< !!
    bool renderOnlySurface; 						///< true if rendering only the surface
    int totalCells; 								///< number of total cells
    std::vector<std::array<int,2>> levelStartIndex; ///< Stores levels start and end indices
    int currentLevel; ///<

public:
    /// \brief constructor
    smSurfaceTree(std::shared_ptr<smSurfaceMesh> surfaceMesh, int maxLevels = 6);

    /// \brief destructor
    ~smSurfaceTree();

    MatrixType transRot; ///< matrix for translation and rotation
    int maxLevel; ///< max level of the tree
    CellType root; ///< !!
    float shiftScale; ///< !!

    std::vector<CellType> initialTreeAllLevels; ///< !!
    std::vector<CellType> treeAllLevels; ///< !!

    /// \brief initialize the draw function related structures
    void initDraw(const smDrawParam &param) override;

protected:

    /// \brief creates the tree based on input triangles
    bool createTree(CellType &Node,
                    const std::vector<int> &triangles,
                    int siblingIndex);

public:

    /// \brief initialize the surface tree structure
    virtual void initStructure();

    virtual smCollisionModelIterator<CellType>  getLevelIterator(int level) ;

    virtual smCollisionModelIterator<CellType>  getLevelIterator() ;

    /// \brief !!
    inline std::shared_ptr<smUnifiedId> getAttachedMeshID()
    {
        return mesh->getUniqueId();
    }

    /// \brief rendering the surface tree
    virtual void draw(const smDrawParam &params) override;

    /// \brief !!
    void handleEvent(std::shared_ptr<smtk::Event::smEvent> p_event) override;

    /// \brief !! smSurfaceTree structure
    void updateStructure();

    /// \brief !!
    void translateRot();

    CellType &getRoot()
    {
        return root;
    }

    std::vector<std::pair<CellType,CellType>> getIntersectingNodes(std::shared_ptr<smSurfaceTree<CellType>> otherTree)
    {
        std::vector<std::pair<CellType,CellType>> intersectingNodes;
        getIntersectingNodes(root, otherTree->getRoot(),intersectingNodes);

        return intersectingNodes;
    }

    void getIntersectingNodes(const CellType &left,
                              const CellType &right,
                              std::vector<std::pair<CellType,CellType>> &result )
    {
        if(!smCollisionMoller::checkOverlapAABBAABB(left.getAabb(),right.getAabb()))
        {
            return;
        }

        if(left.getIsLeaf() && right.getIsLeaf())
        {
            result.emplace_back(left,right);
        }
        else if(left.getIsLeaf())
        {
            for(const auto &child : right.getChildNodes())
            {
                if(!child) continue;
                getIntersectingNodes(left,*child.get(),result);
            }
        }
        else if(right.getIsLeaf())
        {
            for(const auto &child : left.getChildNodes())
            {
                if(!child) continue;
                getIntersectingNodes(*child.get(),right,result);
            }
        }
        else
        {
            for(const auto &rightChild : right.getChildNodes())
            {
                if(!rightChild) continue;
                for(const auto &leftChild : left.getChildNodes())
                {
                    if(!leftChild) continue;
                    getIntersectingNodes(*leftChild.get(),*rightChild.get(),result);
                }
            }
        }

    }
};

#include "smCollision/smSurfaceTree.hpp"

#endif
