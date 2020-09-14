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

#include "imstkDeviceClient.h"

#include <HD/hd.h>

namespace imstk
{
struct HDstate
{
    // \todo pos are redundant?
    HDdouble pos[3];
    HDdouble vel[3];
    HDdouble trans[16];
    HDint buttons;
};

///
/// \class HDAPIDeviceClient
/// \brief Subclass of DeviceClient for phantom omni
/// Holds and updates the data sync or on its own thread
/// Holder of data
///
class HapticDeviceClient : public DeviceClient
{
friend class HapticDeviceManager;

public:
    virtual ~HapticDeviceClient() {}

protected:
    ///
    /// \brief Constructor/Destructor
    ///
    explicit HapticDeviceClient(const std::string& name) : DeviceClient(name, "localhost") {}

    ///
    /// \brief Initialize the phantom omni device
    ///
    void initialize();

    ///
    /// \brief Use callback to get tracking data from phantom omni
    ///
    void updateData();

    ///
    /// \brief Disables the phantom omni device
    ///
    void disable();

private:
    ///
    /// \brief Phantom omni device api callback
    ///
    static HDCallbackCode HDCALLBACK hapticCallback(void* pData);

    HHD     m_handle; ///< device handle
    HDstate m_state;  ///< device reading state
};
}