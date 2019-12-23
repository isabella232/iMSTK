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

#pragma once

#include "imstkGeometry.h"

namespace imstk
{
///
/// \class ImageData
///
/// \brief Class to represent 2D and 3D image data (i.e. structured points)
///
class ImageData : public Geometry
{
public:
    ///
    /// \brief Protected constructor
    ///
    ImageData(const Type type = Geometry::Type::ImageData, const std::string name = std::string(""))
        : Geometry(type, name)
    {
    }

    ///
    /// \brief Print the image data info
    ///
    void print() const override;

    ///
    /// \brief Returns the volume
    ///
    double getVolume() const override;

protected:
    friend class VTKImageDataRenderDelegate;

    void applyTranslation(const Vec3d t) override
    {
    }

    void applyRotation(const Mat3d r) override
    {
    }

    void applyScaling(const double s) override
    {
    }

    void updatePostTransformData() override
    {
    }
};
} // imstk
