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

#include "imstkMath.h"
#include "imstkTimer.h"
#include "imstkSimulationManager.h"

// Objects
#include "imstkForceModelConfig.h"
#include "imstkFEMDeformableBodyModel.h"
#include "imstkVirtualCouplingPBDObject.h"
#include "imstkDynamicObject.h"
#include "imstkDeformableObject.h"
#include "imstkPbdObject.h"
#include "imstkSceneObject.h"
#include "imstkLight.h"
#include "imstkCamera.h"
#include "imstkRigidObject.h"


#include "imstkGraph.h"

// Time Integrators
#include "imstkBackwardEuler.h"

// Solvers
#include "imstkNonlinearSystem.h"
#include "imstkNewtonSolver.h"
#include "imstkConjugateGradient.h"
#include "imstkPbdSolver.h"
#include "imstkGaussSeidel.h"
#include "imstkJacobi.h"
#include "imstkSOR.h"

// Geometry
#include "imstkPlane.h"
#include "imstkSphere.h"
#include "imstkCube.h"
#include "imstkCylinder.h"
#include "imstkTetrahedralMesh.h"
#include "imstkHexahedralMesh.h"
#include "imstkSurfaceMesh.h"
#include "imstkMeshIO.h"
#include "imstkLineMesh.h"
#include "imstkDecalPool.h"

// Maps
#include "imstkTetraTriangleMap.h"
#include "imstkIsometricMap.h"
#include "imstkOneToOneMap.h"

// Devices and controllers
#include "imstkHDAPIDeviceClient.h"
#include "imstkHDAPIDeviceServer.h"
#include "imstkVRPNDeviceClient.h"
#include "imstkVRPNDeviceServer.h"
#include "imstkCameraController.h"
#include "imstkSceneObjectController.h"
#include "imstkLaparoscopicToolController.h"

// Collisions
#include "imstkInteractionPair.h"
#include "imstkPointSetToPlaneCD.h"
#include "imstkPointSetToSphereCD.h"
#include "imstkVirtualCouplingCH.h"
#include "imstkPointSetToSpherePickingCD.h"
#include "imstkPickingCH.h"
#include "imstkBoneDrillingCH.h"

// logger
#include "g3log/g3log.hpp"
#include "imstkLogger.h"

// imstk utilities
#include "imstkPlotterUtils.h"
#include "imstkAPIUtilities.h"

// testVTKTexture
#include <vtkOBJReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <string>
#include <vtkJPEGReader.h>

// global variables
const std::string phantomOmni1Name = "Phantom1";
const std::string phantomOmni2Name = "Phantom2";
const std::string novintFalcon1Name = "device0";
const std::string novintFalcon2Name = "device1";

using namespace imstk;

void testLapToolController()
{
#ifdef iMSTK_USE_OPENHAPTICS
    // SDK and Scene
    auto sdk = std::make_shared<SimulationManager>();
    auto scene = sdk->createNewScene("TestLapToolController");

    // Device clients
    auto client0 = std::make_shared<HDAPIDeviceClient>(phantomOmni1Name);

    // Device Server
    auto server = std::make_shared<HDAPIDeviceServer>();
    server->addDeviceClient(client0);
    sdk->addModule(server);

    // Plane
    auto planeObj = apiutils::createVisualAnalyticalSceneObject(
        Geometry::Type::Plane, scene, "VisualPlane", 100., Vec3d(0., -20., 0.));

    // laparoscopic tool
    auto pivot = apiutils::createAndAddVisualSceneObject(scene, iMSTK_DATA_ROOT "/laptool/pivot.obj", "pivot");
    auto upperJaw = apiutils::createAndAddVisualSceneObject(scene, iMSTK_DATA_ROOT "/laptool/upper.obj", "upperJaw");
    auto lowerJaw = apiutils::createAndAddVisualSceneObject(scene, iMSTK_DATA_ROOT "/laptool/lower.obj", "lowerJaw");

    auto trackingCtrl = std::make_shared<DeviceTracker>(client0);
    trackingCtrl->setTranslationScaling(0.5);
    auto lapToolController = std::make_shared<LaparoscopicToolController>(pivot, upperJaw, lowerJaw, trackingCtrl);
    lapToolController->setJawRotationAxis(Vec3d(1.0, 0, 0));
    lapToolController->setJawAngleChange(0.1);
    scene->addObjectController(lapToolController);

    // Set Camera
    auto cam = scene->getCamera();
    cam->setPosition(Vec3d(0, 30, 60));
    cam->setFocalPoint(Vec3d(0, 0, 0));

    // Light
    auto light = std::make_shared<DirectionalLight>("light");
    light->setFocalPoint(Vec3d(5, -8, -5));
    light->setIntensity(1);
    scene->addLight(light);

    // Run
    sdk->setActiveScene(scene);
    sdk->startSimulation(SimulationStatus::PAUSED);

#endif
}

int main()
{
    
    testLapToolController();
   
    return 0;
}
