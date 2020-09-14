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

#include "imstkTrackingDeviceControl.h"
#include "imstkDeviceClient.h"
#include "imstkLogger.h"
#include "imstkMath.h"

namespace imstk
{
TrackingDeviceControl::TrackingDeviceControl() :
    m_translationOffset(WORLD_ORIGIN),
    m_rotationOffset(Quatd::Identity())
{
}

TrackingDeviceControl::TrackingDeviceControl(std::shared_ptr<DeviceClient> device) :
    DeviceControl(device),
    m_translationOffset(WORLD_ORIGIN),
    m_rotationOffset(Quatd::Identity())
{
}

bool
TrackingDeviceControl::updateTrackingData()
{
    if (m_deviceClient == nullptr)
    {
        LOG(WARNING) << "DeviceTracker::getTrackingData warning: no controlling device set.";
        return false;
    }

    // Retrieve device info
    m_currentPos = m_deviceClient->getPosition();
    m_currentRot = m_deviceClient->getOrientation();

    // Apply inverse if needed
    if (m_invertFlags & InvertFlag::transX)
    {
        m_currentPos[0] = -m_currentPos[0];
    }
    if (m_invertFlags & InvertFlag::transY)
    {
        m_currentPos[1] = -m_currentPos[1];
    }
    if (m_invertFlags & InvertFlag::transZ)
    {
        m_currentPos[2] = -m_currentPos[2];
    }
    if (m_invertFlags & InvertFlag::rotX)
    {
        m_currentRot.x() = -m_currentRot.x();
    }
    if (m_invertFlags & InvertFlag::rotY)
    {
        m_currentRot.y() = -m_currentRot.y();
    }
    if (m_invertFlags & InvertFlag::rotZ)
    {
        m_currentRot.z() = -m_currentRot.z();
    }

    // Apply Offsets
    m_currentPos = m_rotationOffset * m_currentPos * m_scaling + m_translationOffset;
    m_currentRot = m_rotationOffset * m_currentRot;

    m_trackingDataUptoDate = true;

    return true;
}

double
TrackingDeviceControl::getTranslationScaling() const
{
    return m_scaling;
}

void
TrackingDeviceControl::setTranslationScaling(const double scaling)
{
    m_scaling = scaling;
}

const Vec3d&
TrackingDeviceControl::getTranslationOffset() const
{
    return m_translationOffset;
}

void
TrackingDeviceControl::setTranslationOffset(const Vec3d& t)
{
    m_translationOffset = t;
}

const Quatd&
TrackingDeviceControl::getRotationOffset()
{
    return m_rotationOffset;
}

void
TrackingDeviceControl::setRotationOffset(const Quatd& r)
{
    m_rotationOffset = r;
}

unsigned char
TrackingDeviceControl::getInversionFlags()
{
    return m_invertFlags;
}

void
TrackingDeviceControl::setInversionFlags(const unsigned char f)
{
    m_invertFlags = f;
}
} // imstk