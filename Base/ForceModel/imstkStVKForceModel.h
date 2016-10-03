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

#ifndef imstkStVKForceModel_h
#define imstkStVKForceModel_h

#include <memory>
#include "g3log/g3log.hpp"

//imstk
#include "imstkInternalForceModel.h"

//vega
#include "StVKInternalForces.h"
#include "StVKStiffnessMatrix.h"
#include "tetMesh.h"

namespace imstk
{

class StVKForceModel : public InternalForceModel
{
public:
    ///
    /// \brief Constructor
    ///
    StVKForceModel(std::shared_ptr<vega::VolumetricMesh> mesh,
        const bool withGravity = true,
        const double gravity = 10.0) : InternalForceModel()
    {
        auto tetMesh = std::dynamic_pointer_cast<vega::TetMesh>(mesh);
        m_stVKInternalForces = std::make_shared<vega::StVKInternalForces>(tetMesh.get(), nullptr, withGravity, gravity);
        m_vegaStVKStiffnessMatrix = std::make_shared<vega::StVKStiffnessMatrix>(m_stVKInternalForces.get());
    }

    ///
    /// \brief Destructor
    ///
    virtual ~StVKForceModel() = default;

    ///
    /// \brief Get the internal force
    ///
    void getInternalForce(const Vectord& u, Vectord& internalForce)
    {
        double *data = const_cast<double*>(u.data());
        m_stVKInternalForces->ComputeForces(data, internalForce.data());
    }

    ///
    /// \brief Get the tangent stiffness matrix topology
    ///
    virtual void getTangentStiffnessMatrixTopology(vega::SparseMatrix** tangentStiffnessMatrix)
    {
        m_vegaStVKStiffnessMatrix->GetStiffnessMatrixTopology(tangentStiffnessMatrix);
    }

    ///
    /// \brief Set the tangent stiffness matrix
    ///
    void getTangentStiffnessMatrix(const Vectord& u, SparseMatrixd& tangentStiffnessMatrix)
    {
        double *data = const_cast<double*>(u.data());
        m_vegaStVKStiffnessMatrix->ComputeStiffnessMatrix(data, m_vegaTangentStiffnessMatrix.get());
        InternalForceModel::updateValuesFromMatrix(m_vegaTangentStiffnessMatrix, tangentStiffnessMatrix.valuePtr());
    }

    ///
    /// \brief Set the tangent stiffness matrix and internal force
    ///
    void GetForceAndMatrix(const Vectord& u, Vectord& internalForce, SparseMatrixd& tangentStiffnessMatrix)
    {
        getInternalForce(u, internalForce);
        getTangentStiffnessMatrix(u, tangentStiffnessMatrix);
    }

    ///
    /// \brief
    ///
    virtual void setTangentStiffness(std::shared_ptr<vega::SparseMatrix> K) override
    {
    }

protected:

    std::shared_ptr<vega::StVKInternalForces> m_stVKInternalForces;
    std::shared_ptr<vega::SparseMatrix> m_vegaTangentStiffnessMatrix;
    std::shared_ptr<vega::StVKStiffnessMatrix> m_vegaStVKStiffnessMatrix;
    bool ownStiffnessMatrix;
};

} // imstk

#endif // imstkStVKForceModel_h