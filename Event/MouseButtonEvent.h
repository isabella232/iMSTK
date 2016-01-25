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

#ifndef EVENT_MOUSEBUTTONEVENT_H
#define EVENT_MOUSEBUTTONEVENT_H

// iMSTK includes
#include "Core/Event.h"
#include "Core/Vector.h"

namespace imstk {

enum class MouseButton
{
    Unknown = -1,
    Left = 0,
    Right,
    Middle,
    Button0,
    Button1,
    Reset
};

class MouseButtonEvent : public Event
{
public:
    static EventType EventName;

public:
    MouseButtonEvent(const MouseButton &button);

    const MouseButton &getMouseButton();

    void setPresed(const bool &press);

    const bool &getPressed();

    const bool &togglePressed();

    void setWindowCoord(const Vec2d &coordinates);

    const Vec2d &getWindowCoord();

private:
    bool pressed; // If the button was pressed or released in this event
    MouseButton mouseButton; // Which mouse button was pressed
    Vec2d coord; // X,Y coorindate relative to left edge
};

} // event namespace

#endif // EVENT_MOUSEBUTTONEVENT_H
