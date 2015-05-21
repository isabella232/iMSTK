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

#ifndef SMGEOMETRY_H
#define SMGEOMETRY_H

// SimMedTK includes
#include "smCore/smConfig.h"
#include "smUtilities/smVector.h"

//forward declaration
struct smSphere;

/// \brief  Simple Plane definition with unit normal and spatial location
struct smPlane
{
    smVec3d unitNormal;
    smVec3d pos;
    double distance(smVec3d p_vector);
    smVec3d project(smVec3d p_vector);
};

/// \brief Axis Aligned bounding box declarions
class smAABB
{
public:
    /// \brief minimum x,y,z point
    smVec3d aabbMin;

    /// \brief maximum x,y,z point
    smVec3d aabbMax;

    const smVec3d &getMax() const
    {
        return aabbMax;
    }

    const smVec3d &getMin() const
    {
        return aabbMin;
    }

    /// \brief constrcutor. The default is set to origin for aabbMin and aabbMax
    smAABB();

    /// \brief center of the AABB
    smVec3d center() const;

    /// \brief check if two AABB overlaps
    static smBool checkOverlap(const smAABB &p_aabbA, const smAABB &p_aabbB);

    /// \brief set  p_aabb to the current one
    const smAABB &operator=(const smAABB &p_aabb);

    /// \brief scale the AABB
    smAABB &operator*(const double p_scale);

    /// \brief sub divides p_length will be used to create the slices
    void subDivide(const double p_length, const smInt p_divison, smAABB *p_aabb) const;

    /// \brief divides current AABB in x,y,z axes with specificed divisions. results are placed in p_aabb
    void subDivide(const smInt p_divisionX, const smInt p_divisionY, const smInt p_divisionZ, smAABB *p_aabb) const;

    /// \brief divides current AABB in all axes with specificed p_division. results are placed in p_aabb
    void subDivide(const smInt p_division, smAABB *p_aabb) const;

    /// \brief returns half of X edge of AABB
    double halfSizeX() const;

    /// \brief returns half of Y edge of AABB
    double halfSizeY() const;

    /// \brief returns half of Z edge of AABB
    double halfSizeZ() const;

    /// \brief expands aabb with p_factor
    void expand(const double &p_factor);

    void draw() const
    {
        glBegin(GL_LINES);
        {
            glVertex3d(aabbMin[0], aabbMin[1], aabbMin[2]);
            glVertex3d(aabbMin[0], aabbMin[1], aabbMax[2]);
            glVertex3d(aabbMin[0], aabbMax[1], aabbMin[2]);
            glVertex3d(aabbMin[0], aabbMax[1], aabbMax[2]);
            glVertex3d(aabbMax[0], aabbMin[1], aabbMin[2]);
            glVertex3d(aabbMax[0], aabbMin[1], aabbMax[2]);
            glVertex3d(aabbMax[0], aabbMax[1], aabbMin[2]);
            glVertex3d(aabbMax[0], aabbMax[1], aabbMax[2]);

            glVertex3d(aabbMin[0], aabbMin[1], aabbMin[2]);
            glVertex3d(aabbMin[0], aabbMax[1], aabbMin[2]);
            glVertex3d(aabbMin[0], aabbMin[1], aabbMax[2]);
            glVertex3d(aabbMin[0], aabbMax[1], aabbMax[2]);
            glVertex3d(aabbMax[0], aabbMin[1], aabbMin[2]);
            glVertex3d(aabbMax[0], aabbMax[1], aabbMin[2]);
            glVertex3d(aabbMax[0], aabbMin[1], aabbMax[2]);
            glVertex3d(aabbMax[0], aabbMax[1], aabbMax[2]);

            glVertex3d(aabbMin[0], aabbMin[1], aabbMin[2]);
            glVertex3d(aabbMax[0], aabbMin[1], aabbMin[2]);
            glVertex3d(aabbMin[0], aabbMax[1], aabbMin[2]);
            glVertex3d(aabbMax[0], aabbMax[1], aabbMin[2]);
            glVertex3d(aabbMin[0], aabbMin[1], aabbMax[2]);
            glVertex3d(aabbMax[0], aabbMin[1], aabbMax[2]);
            glVertex3d(aabbMin[0], aabbMax[1], aabbMax[2]);
            glVertex3d(aabbMax[0], aabbMax[1], aabbMax[2]);
        }
        glEnd();
    }

    void reset()
    {
        this->aabbMin << std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max();
        this->aabbMax << std::numeric_limits<float>::min(),
            std::numeric_limits<float>::min(),
            std::numeric_limits<float>::min();
    }

    void extend(const smAABB &other)
    {
        this->aabbMin = this->aabbMin.array().min(other.getMin().array());
        this->aabbMax = this->aabbMax.array().max(other.getMax().array());
    }
};

/// \brief sphere structure
struct smSphere
{

public:
    /// \brief center of sphere
    smVec3d center;

    /// \brief radius of sshere
    double radius;

    /// \brief constructor
    smSphere();

    /// \brief sphere constructor with center and radius
    smSphere(smVec3d p_center, double p_radius);
};

/// \brief cube
struct smCube
{
    /// \brief cube center
    smVec3d center;

    /// \brief cube length
    double sideLength;

    /// \brief constructor
    smCube();

    /// \brief subdivides the cube in mulitple cube with given number of cubes identified for each axis with p_divisionPerAxis
    void subDivide(smInt p_divisionPerAxis, smCube *p_cube);

    /// \brief expands the cube. increases the edge length with expansion*edge length
    void expand(double p_expansion);

    /// \brief returns the left most corner
    smVec3d leftMinCorner() const ;

    /// \brief returns right most corner
    smVec3d rightMaxCorner() const;

    /// \brief returns the smallest sphere encapsulates the cube
    smSphere getCircumscribedSphere();

    /// \brief returns the  sphere with half edge of the cube as a radius
    smSphere getInscribedSphere();

    /// \brief get tangent sphere
    smSphere getTangent2EdgeSphere();
};

#endif
