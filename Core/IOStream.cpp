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

// iMSTK includes
#include "Core/IOStream.h"

namespace imstk {

ConsoleStream::ConsoleStream()
{
}
IOStream& ConsoleStream::operator<<(std::string p_string)
{
    std::cout << p_string;
    return *this;
}
IOStream& ConsoleStream::operator>>(std::string &p_string)
{
    std::getline(std::cin, inputBuffer);
    p_string = inputBuffer;
    return *this;
}
WindowString::WindowString()
{
    x = 0;
    y = 0;
    string = "";
    string.reserve(IMSTK_WINDOW_MAXSTRINGSIZE);
}
WindowString::WindowString(std::string p_string)
{
    string = p_string;
}
WindowString::WindowString(std::string p_string, float p_x, float p_y)
{
    string = p_string;
    x = p_x;
    y = p_y;
}
WindowString& WindowString::operator<<(std::string p_string)
{
    string = p_string;
    return *this;
}
void WindowString::operator=(WindowString& p_windowString)
{
    string.clear();
    string = p_windowString.string;
    x = p_windowString.x;
    y = p_windowString.y;
}
IOStream& WindowStream::operator<<(std::string /*p_string*/)
{
    return *this;
}
IOStream& WindowStream::operator>>(std::string& /*p_string*/)
{
    return *this;
}
void OpenGLWindowStream::init(int p_totalTexts)
{
    textColor.setValue(1.0, 1.0, 1.0, 1.0);
    totalTexts = p_totalTexts;
    windowTexts = new WindowData[totalTexts];
    this->setDrawOrder(ClassDrawOrder::AfterObjects);

    for (int i = 0; i < totalTexts; i++)
    {
        windowTexts[i].enabled = false;
    }

    enabled = true;
    currentIndex = 0;
    initialTextPositionX = 0.0;
    //initialTextPositionY = font.pointSize(); //+font.pointSize()/2.0;
    initialTextPositionY = 0.0;
    lastTextPosition = 0;
}
OpenGLWindowStream::OpenGLWindowStream(int p_totalTexts)
{
    //font.setPointSize(10.0);
    init(p_totalTexts);
}
int OpenGLWindowStream::addText(const std::string& p_tag, const std::string& p_string)
{
    WindowString string;
    string.string = p_string;
    string.x = 0;
    string.y = lastTextPosition;
    //lastTextPosition += font.pointSize() + font.pointSize() / 2.0;
    tagMap[p_tag] = currentIndex;
    windowTexts[currentIndex].enabled = true;
    windowTexts[currentIndex].windowString = string;
    currentIndex = (currentIndex + 1) % totalTexts;
    return currentIndex;
}
bool OpenGLWindowStream::addText(std::string p_tag, WindowString& p_string)
{
    if (p_string.string.size() > IMSTK_WINDOW_MAXSTRINGSIZE)
    {
        return false;
    }

    currentIndex = (currentIndex + 1) % totalTexts;
    tagMap[p_tag] = currentIndex;
    windowTexts[currentIndex].windowString = p_string;
    windowTexts[currentIndex].enabled = true;
    return true;
}
bool OpenGLWindowStream::updateText(std::string p_tag, std::string p_string)
{
    int index = -1;

    if (p_string.size() > IMSTK_WINDOW_MAXSTRINGSIZE)
    {
        return false;
    }

    index = tagMap[p_tag];

    if (index >= 0)
        windowTexts[index].windowString.string = p_string;
    else
    {
        return false;
    }

    return true;
}
bool OpenGLWindowStream::updateText(int p_textHandle, std::string p_string)
{
    int index = p_textHandle;

    if (p_string.size() > IMSTK_WINDOW_MAXSTRINGSIZE)
    {
        return false;
    }

    if (index >= 0)
        windowTexts[index].windowString.string = p_string;
    else
    {
        return false;
    }

    return true;
}
bool OpenGLWindowStream::removeText(std::string p_tag)
{
    int index = tagMap[p_tag];
    windowTexts[index].enabled = false;
    return true;
}

WindowConsole::WindowConsole(int p_totalTexts)
{
    init(p_totalTexts);
    backGroundColor.setValue(1.0, 1.0, 1.0, 0.15);
    this->eventHanlder->attachEvent(EventType::Keyboard,shared_from_this());
    left = 0.0;
    bottom = 0.0;
    right = 1.0;
    top = 0.15;
}
std::string WindowConsole::getLastEntry()
{
    return windowTexts[currentIndex].windowString.string;
}
int WindowConsole::addText(const std::string &p_tag, const std::string& p_string)
{
    int traverseIndex;
    WindowString string;
    string.string = p_string;
    windowTexts[currentIndex].enabled = true;
    windowTexts[currentIndex].windowString = string;
    tagMap[p_tag] = currentIndex;

    for (int i = currentIndex, counter = 0; counter < totalTexts; i--, counter++)
    {
        if (i < 0)
        {
            i += totalTexts;
        }

        traverseIndex = i % totalTexts;
        windowTexts[traverseIndex].windowString.x = 0.0;
        //windowTexts[traverseIndex].windowString.y = (font.pointSize() * (totalTexts - counter));
    }

    currentIndex = (currentIndex + 1) % totalTexts;
    return currentIndex;
}

}
