// This file is part of the iMSTK project.
//
// Copyright (c) Kitware, Inc.
//
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

#include "Assembler/Assembler.h"

// iMSTK includes
#include "CollisionContext/CollisionContext.h"
#include "Core/ContactHandling.h"
#include "Core/Model.h"
#include "TimeIntegrators/OdeSystem.h"
#include "Solvers/SystemOfEquations.h"

Assembler::Assembler(std::shared_ptr <CollisionContext> collContext)
{
    this->collisionContext = collContext;
    this->initSystem();
}

//---------------------------------------------------------------------------
void Assembler::
setCollisionContext(std::shared_ptr< CollisionContext > newCollisionContext)
{
    this->collisionContext = newCollisionContext;
}

//---------------------------------------------------------------------------
std::shared_ptr<CollisionContext > Assembler::getCollisionContext() const
{
    return this->collisionContext;
}

//---------------------------------------------------------------------------
void Assembler::
setSystemOfEquations(
    std::vector<std::shared_ptr<SparseLinearSystem>> newSystemOfEquations)
{
    this->equationList = newSystemOfEquations;
}

//---------------------------------------------------------------------------
std::vector<std::shared_ptr<Assembler::SparseLinearSystem>> Assembler::
getSystemOfEquations() const
{
    return this->equationList;
}

//---------------------------------------------------------------------------
void Assembler::type1Interactions()
{
    if(!this->collisionContext)
    {
        // TODO: Log this
        return;
    }

    auto ch = this->collisionContext->getContactHandlers();

    for(auto & ch : this->collisionContext->getContactHandlers())
    {
        const auto &forces = ch->getContactForces();
        auto sceneModel = ch->getSecondSceneObject();
        sceneModel->updateExternalForces(forces);
    }
}

//---------------------------------------------------------------------------
void Assembler::initSystem()
{
    for(auto & rows : this->collisionContext->getIslands())
    {
        size_t dofSize = 0;
        size_t nnz = 0;
        std::vector<const core::SparseMatrixd *> systemMatrices;
        std::vector<const core::Vectord *> rhsVector;

        for(auto & col : rows)
        {
            // For the moment only DeformableSceneObject are SystemsOfEquations
            auto sceneModel = this->collisionContext->getSceneModel(col);
            auto linearSystem = std::dynamic_pointer_cast<SparseLinearSystem>(sceneModel);
            if(sceneModel && linearSystem)
            {
                this->equationList.push_back(linearSystem);
                nnz += linearSystem->getMatrix().nonZeros();
                dofSize += linearSystem->getRHSVector().size();
            }
        }

        if(dofSize > 0)
        {
            this->A.emplace_back(dofSize, dofSize);
            this->A.back().reserve(nnz);
            this->b.emplace_back(dofSize);

            size_t size = 0;
            for(auto &equation : this->equationList)
            {
                auto &M = equation->getMatrix();
                auto &rhs = equation->getRHSVector();
                this->concatenateMatrix(M, this->A.back(), size, size);
                this->b.back().segment(size, rhs.size());

                size += rhs.size();
            }
        }
    }
}

//---------------------------------------------------------------------------
void Assembler::concatenateMatrix(const core::SparseMatrixd &Q,
                                  core::SparseMatrixd &R,
                                  std::size_t i,
                                  std::size_t j)
{
    for(size_t k = i; k < i + Q.outerSize(); ++k)
    {
        for(core::SparseMatrixd::InnerIterator it(Q, k); it; ++it)
        {
            R.insert(k, it.col() + j) = it.value();
        }
    }
}