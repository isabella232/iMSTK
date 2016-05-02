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

#include "imstkMeshReader.h"

#include <sys/stat.h>

#include "imstkVTKMeshReader.h"
#include "imstkVegaMeshReader.h"

#include "g3log/g3log.hpp"

namespace imstk {
std::shared_ptr<Mesh>
MeshReader::read(const std::string& filePath)
{
    if (!MeshReader::fileExists(filePath))
    {
        LOG(WARNING) << "MeshReader::read error: file not found: " << filePath;
        return nullptr;
    }

    FileType meshType = MeshReader::getFileType(filePath);
    switch (meshType)
    {
    case FileType::VTK :
    case FileType::VTU :
    case FileType::VTP :
    case FileType::STL :
    case FileType::PLY :
    case FileType::OBJ :
        return VTKMeshReader::read(filePath, meshType);
        break;
    case FileType::VEG :
        return VegaMeshReader::read(filePath, meshType);
        break;
    }

    LOG(WARNING) << "MeshReader::read error: file type not supported";
    return nullptr;
}

bool
MeshReader::fileExists(const std::string& file)
{
    struct stat buf;
    return (stat(file.c_str(), &buf) == 0);
}

const MeshReader::FileType
MeshReader::getFileType(const std::string& filePath)
{
    FileType meshType = FileType::UNKNOWN;

    std::string extString = filePath.substr(filePath.find_last_of(".") + 1);
    if (extString.empty())
    {
        LOG(WARNING) << "MeshReader::getFileType error: invalid file name";
        return meshType;
    }

    if (extString == "vtk" || extString == "VTK")
    {
        meshType = FileType::VTK;
    }
    else if (extString == "vtp" || extString == "VTP")
    {
        meshType = FileType::VTP;
    }
    else if (extString == "vtu" || extString == "VTU")
    {
        meshType = FileType::VTU;
    }
    else if (extString == "obj" || extString == "OBJ")
    {
        meshType = FileType::OBJ;
    }
    else if (extString == "stl" || extString == "STL")
    {
        meshType = FileType::STL;
    }
    else if (extString == "ply" || extString == "PLY")
    {
        meshType = FileType::PLY;
    }
    else if (extString == "veg" || extString == "VEG")
    {
        meshType = FileType::VEG;
    }
    else
    {
        LOG(WARNING) << "MeshReader::getFileType error: unknown file extension";
    }

    return meshType;
}

}
