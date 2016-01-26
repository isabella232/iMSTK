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

#ifndef CORE_COLLSIONDETECTION_H
#define CORE_COLLSIONDETECTION_H

// STL includes
#include <memory>

// iMSTK includes
#include "Core/CoreClass.h"

class Mesh;
class CollisionManager;

/// \brief Base class to calculate contact information between two meshes
/// It determines if two meshes are in close proximity and calculates contacts
/// if they are.
class CollisionDetection: public CoreClass
{
public:
    CollisionDetection() {}

    virtual ~CollisionDetection() {}

    void computeCollision(std::shared_ptr<CollisionManager> pairs);

private:
    virtual void doComputeCollision(std::shared_ptr<CollisionManager> pairs) = 0;
};

#endif