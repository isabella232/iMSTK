###########################################################################
#
# Copyright (c) Center for Modeling, Simulation, and Imaging in Medicine,
#                        Rensselaer Polytechnic Institute
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0.txt
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
###########################################################################
#
# Oculus
#

set(Oculus_TAG "d4327f8e773dc9ce008717d4960629aabac97f8f")
set(Oculus_REPOSITORY ${git_protocol}://github.com/ricortiz/OculusSDK.git)

# Make sure this file is included only once
get_filename_component(CMAKE_CURRENT_LIST_FILENAME ${CMAKE_CURRENT_LIST_FILE} NAME_WE)
if(${CMAKE_CURRENT_LIST_FILENAME}_FILE_INCLUDED)
  return()
endif()
set(${CMAKE_CURRENT_LIST_FILENAME}_FILE_INCLUDED 1)

# Sanity checks
if(DEFINED Oculus_DIR AND NOT EXISTS ${Oculus_DIR})
  message(FATAL_ERROR "Oculus_DIR variable is defined but corresponds to non-existing directory")
endif()

set(Oculus_DEPENDENCIES "")

# Include dependent projects if any
SimMedTKCheckDependencies(Oculus)

set(proj Oculus)

if(NOT DEFINED ${proj}_DIR)

  # Set CMake OSX variable to pass down the external project
  set(CMAKE_OSX_EXTERNAL_PROJECT_ARGS)
  if(APPLE)
    list(APPEND CMAKE_OSX_EXTERNAL_PROJECT_ARGS
      -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
      -DCMAKE_OSX_SYSROOT=${CMAKE_OSX_SYSROOT}
      -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET})
  endif()

#     message(STATUS "Adding project:${proj}")
  ExternalProject_Add(${proj}
    SOURCE_DIR ${CMAKE_BINARY_DIR}/SuperBuild/${proj}
    BINARY_DIR ${CMAKE_BINARY_DIR}/SuperBuild/${proj}-build
    PREFIX ${CMAKE_BINARY_DIR}/SuperBuild/${proj}${ep_suffix}
    GIT_REPOSITORY ${${proj}_REPOSITORY}
    GIT_TAG ${${proj}_TAG}
    GIT_SUBMODULES cmake
    UPDATE_COMMAND ""
    INSTALL_COMMAND ""
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS
      -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
      -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
      -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/SimMedTK-build
      -DBUILD_SHARED_LIBS:BOOL=${SimMedTK_BUILD_SHARED_LIBS}
      -DOCULUS_BUILD_SAMPLES:BOOL=OFF
      ${CMAKE_OSX_EXTERNAL_PROJECT_ARGS}
      ${OUTPUT_DIRECTORIES}
    DEPENDS
      ${${proj}_DEPENDENCIES}
	LOG_DOWNLOAD 1            # Wrap download in script to log output
	LOG_UPDATE 1              # Wrap update in script to log output
# 	LOG_CONFIGURE 1           # Wrap configure in script to log output
# 	LOG_BUILD 1               # Wrap build in script to log output
	LOG_TEST 1                # Wrap test in script to log output
	LOG_INSTALL 1             # Wrap install in script to log output
    )
  set(${proj}_DIR ${CMAKE_BINARY_DIR}/SuperBuild/${proj}-build)

else()
  SimMedTKEmptyExternalProject(${proj} "${${proj}_DEPENDENCIES}")
endif()

set(SimMedTK_CMAKE_INCLUDE_PATH ${CMAKE_BINARY_DIR}/SuperBuild/${proj}/LibOVR/Include/${sep}${SimMedTK_CMAKE_INCLUDE_PATH})
set(SimMedTK_CMAKE_INCLUDE_PATH ${CMAKE_BINARY_DIR}/SuperBuild/${proj}/LibOVR/Src/${sep}${SimMedTK_CMAKE_INCLUDE_PATH})