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

#ifndef imstkSceneObjectControllerBase_h
#define imstkSceneObjectControllerBase_h

namespace imstk
{

///
/// \class SceneObjectControllerBase
///
/// \brief Base class for all the scene object controllers
///
class SceneObjectControllerBase
{
public:
    ///
    /// \brief Constructor/Destructor
    ///
    SceneObjectControllerBase() = default;
    ~SceneObjectControllerBase() = default;

    ///
    /// \brief Initialize offset based on object geometry
    ///
    virtual void initOffsets() = 0;

    ///
    /// \brief Update controlled scene objects using latest tracking information
    ///
    virtual void updateControlledObjects() = 0;

    ///
    /// \brief Apply forces to the haptic device
    ///
    virtual void applyForces() = 0;

    ///
    /// \brief
    ///
    virtual void setTrackerToOutOfDate() = 0;
};

} // imstk

#endif // ifndef imstkSceneObjectControllerBase_h