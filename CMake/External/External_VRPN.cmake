#-----------------------------------------------------------------------------
# Dependencies
#-----------------------------------------------------------------------------
set(VRPN_DEPENDENCIES "LibNiFalcon")
if(WIN32)
  list(APPEND VRPN_DEPENDENCIES "Libusb")
  list(APPEND VRPN_DEPENDENCIES "FTD2XX")
endif(WIN32)

#-----------------------------------------------------------------------------
# Phantom Omni
#-----------------------------------------------------------------------------
option(${PROJECT_NAME}_USE_OpenHaptics "Build OpenHaptics to support the Phantom Omni in VRPN." OFF)
if(${${PROJECT_NAME}_USE_OpenHaptics})
  message(STATUS "Superbuild -   VRPN => ENABLING Phantom Omni support")
  if(NOT DEFINED OPENHAPTICS_ROOT_DIR OR NOT EXISTS ${OPENHAPTICS_ROOT_DIR})
    set(OPENHAPTICS_ROOT_DIR "$ENV{OH_SDK_BASE}" CACHE PATH "Path to OpenHaptics install directory." FORCE)
  endif()
  if(NOT EXISTS ${OPENHAPTICS_ROOT_DIR})
    message(FATAL_ERROR "\nCan not support Phantom Omni without OpenHaptics.\nSet OPENHAPTICS_ROOT_DIR to OpenHaptics installation directory.\n\n")
  endif()
else()
  message(STATUS "Superbuild -   VRPN => Phantom Omni support DISABLED")
  if(DEFINED OPENHAPTICS_ROOT_DIR)
    unset(OPENHAPTICS_ROOT_DIR CACHE)
  endif()
endif()

#-----------------------------------------------------------------------------
# Add External Project
#-----------------------------------------------------------------------------
include(imstkAddExternalProject)
imstk_add_external_project( VRPN
  GIT_REPOSITORY https://gitlab.kitware.com/iMSTK/vrpn.git
  GIT_TAG 021396b8770e6f14cb14427345d0b74479a32d82
  # This is the improve-openhaptics-rebase branch
  CMAKE_CACHE_ARGS
    -DBUILD_TESTING:BOOL=OFF
    -DVRPN_SUBPROJECT_BUILD:BOOL=ON
    -DVRPN_BUILD_CLIENTS:BOOL=OFF
    -DVRPN_BUILD_CLIENT_LIBRARY:BOOL=ON
    -DVRPN_BUILD_SERVER_LIBRARY:BOOL=ON
    -DVRPN_INSTALL:BOOL=ON
    -DVRPN_BUILD_PYTHON:BOOL=OFF
    -DVRPN_USE_GPM_MOUSE:BOOL=OFF
    -DVRPN_USE_LIBUSB_1_0:BOOL=ON
    -DVRPN_USE_HID:BOOL=ON
    -DVRPN_USE_LIBNIFALCON:BOOL=ON
    -DVRPN_BUILD_SERVERS:BOOL=${${PROJECT_NAME}_USE_OpenHaptics}
    -DVRPN_USE_PHANTOM_SERVER:BOOL=${${PROJECT_NAME}_USE_OpenHaptics}
    -DVRPN_USE_HDAPI:BOOL=${${PROJECT_NAME}_USE_OpenHaptics}
    -DOPENHAPTICS_ROOT_DIR:PATH=${OPENHAPTICS_ROOT_DIR}
  DEPENDENCIES ${VRPN_DEPENDENCIES}
  RELATIVE_INCLUDE_PATH ""
  #VERBOSE
  )
