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

using namespace imstk;

void testRendering()
{
    // SDK and Scene
    auto sdk = std::make_shared<SimulationManager>();
    auto scene = sdk->createNewScene("RenderingTest");

    // Head mesh
    auto head = MeshIO::read(iMSTK_DATA_ROOT "/head/head_revised.obj");
    auto headMesh = std::dynamic_pointer_cast<SurfaceMesh>(head);
    auto headObject = std::make_shared<VisualObject>("Head");
    headObject->setVisualGeometry(headMesh);
    scene->addSceneObject(headObject);

    // Head material
    auto headMaterial = std::make_shared<RenderMaterial>();
    auto headDiffuseTexture = std::make_shared<Texture>(iMSTK_DATA_ROOT "/head/diffuse.jpg", Texture::DIFFUSE);
    auto headNormalTexture = std::make_shared<Texture>(iMSTK_DATA_ROOT "/head/normal.png", Texture::NORMAL);
    auto headRoughnessTexture = std::make_shared<Texture>(iMSTK_DATA_ROOT "/head/roughness.jpg", Texture::ROUGHNESS);
    auto headSSSTexture = std::make_shared<Texture>(iMSTK_DATA_ROOT "/head/sss.jpg", Texture::SUBSURFACE_SCATTERING);
    headMaterial->addTexture(headDiffuseTexture);
    headMaterial->addTexture(headNormalTexture);
    headMaterial->addTexture(headRoughnessTexture);
    headMaterial->addTexture(headSSSTexture);
    headMaterial->setReceivesShadows(true);
    headMaterial->setCastsShadows(true);
    headMesh->setRenderMaterial(headMaterial);

    // Position camera
    auto cam = scene->getCamera();
    cam->setPosition(0, 0.25, 2);
    cam->setFocalPoint(0, 0.25, 0);

    // Lights
    auto directionalLight = std::make_shared<DirectionalLight>("DirectionalLight");
    directionalLight->setIntensity(7);
    directionalLight->setColor(Color(1.0, 0.95, 0.8));
    directionalLight->setCastsShadow(true);
    directionalLight->setShadowRange(1.5);
    scene->addLight(directionalLight);

    auto pointLight = std::make_shared<PointLight>("PointLight");
    pointLight->setIntensity(0.1);
    pointLight->setPosition(0.1, 0.2, 0.5);
    scene->addLight(pointLight);

    // Sphere
    auto sphereObj = apiutils::createVisualAnalyticalSceneObject(Geometry::Type::Sphere, scene, "VisualSphere", 0.025);
    auto sphereMaterial = std::make_shared<RenderMaterial>();
    auto sphereMesh = sphereObj->getVisualGeometry();
    sphereMesh->translate(0.1, 0.2, 0.5);
    sphereMaterial->setEmissivity(10);
    sphereMaterial->setCastsShadows(false);
    sphereObj->getVisualGeometry()->setRenderMaterial(sphereMaterial);

    // Plane
    auto planeObj = apiutils::createVisualAnalyticalSceneObject(Geometry::Type::Plane, scene, "VisualPlane", 10);
    auto planeMaterial = std::make_shared<RenderMaterial>();
    planeMaterial->setDiffuseColor(Color::DarkGray);
    planeObj->getVisualGeometry()->setRenderMaterial(planeMaterial);

    // Run
    sdk->setActiveScene(scene);
    sdk->getViewer()->setBackgroundColors(Vec3d(0, 0, 0));

#ifdef iMSTK_USE_Vulkan
    auto viewer = std::dynamic_pointer_cast<VulkanViewer>(sdk->getViewer());
    viewer->setResolution(1920, 1080);
    viewer->disableVSync();
    viewer->enableFullscreen();
#endif
    sdk->startSimulation(SimulationStatus::PAUSED);
}

int main()
{
    testRendering();
     return 0;
}
