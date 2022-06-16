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

#include "imstkCamera.h"
#include "imstkCDObjectFactory.h"
#include "imstkKeyboardSceneControl.h"
#include "imstkLineMesh.h"
#include "imstkLineMeshToLineMeshCCD.h"
#include "imstkMeshIO.h"
#include "imstkMouseSceneControl.h"
#include "imstkNew.h"
#include "imstkOrientedBox.h"
#include "imstkPbdModel.h"
#include "imstkPbdObject.h"
#include "imstkPbdObjectCollision.h"
#include "imstkRenderMaterial.h"
#include "imstkRigidBodyModel2.h"
#include "imstkRigidObjectController.h"
#include "imstkScene.h"
#include "imstkSceneManager.h"
#include "imstkSimulationManager.h"
#include "imstkSurfaceMesh.h"
#include "imstkTaskGraph.h"
#include "imstkVisualModel.h"
#include "imstkVTKViewer.h"
#include "../PBDStaticSuture/NeedleObject.h"

#ifdef iMSTK_USE_OPENHAPTICS
#include "imstkHapticDeviceManager.h"
#include "imstkHapticDeviceClient.h"
#else
#include "../PBDStaticSuture/imstkMouseDeviceClient3D.h"
#endif

using namespace imstk;

/// String geometry for interactive example.
const std::vector<Eigen::Vector3d> selfCCDStringMesh = {
    // move right
    { 0.00, 0.02, 0 },
    { 0.01, 0.02, 0 },
    { 0.02, 0.02, 0 },
    { 0.03, 0.02, 0 },
    { 0.04, 0.02, 0 },
    { 0.05, 0.02, 0 },
    { 0.06, 0.02, 0 },
    { 0.07, 0.02, 0 },
    { 0.08, 0.02, 0 },
    { 0.09, 0.02, 0 },

    // jump (back / down / left) and then move forward
    { 0.05, 0, -0.04 },
    { 0.05, 0, -0.03 },
    { 0.05, 0, -0.02 },
    { 0.05, 0, -0.01 },
    { 0.05, 0, 0.00 },
    { 0.05, 0, 0.01 },
    { 0.05, 0, 0.02 },
    { 0.05, 0, 0.03 },
    { 0.05, 0, 0.04 },
};

///
/// \brief Create self-ccd string geometry
///
static std::shared_ptr<LineMesh>
makeSelfCCDGeometry()
{
    // Create the geometry
    imstkNew<LineMesh>                stringGeometry;
    size_t                            numVerts = selfCCDStringMesh.size();
    imstkNew<VecDataArray<double, 3>> verticesPtr(static_cast<int>(numVerts));
    VecDataArray<double, 3>&          vertices = *verticesPtr.get();
    for (size_t i = 0; i < numVerts; i++)
    {
        vertices[i] = selfCCDStringMesh[i];
    }

    // Add connectivity data
    imstkNew<VecDataArray<int, 2>> segmentsPtr;
    VecDataArray<int, 2>&          segments = *segmentsPtr.get();
    for (int i = 0; i < numVerts - 1; i++)
    {
        segments.push_back(Vec2i(i, i + 1));
    }

    stringGeometry->initialize(verticesPtr, segmentsPtr);
    return stringGeometry;
}

///
/// \brief Create pbd string object
///
static std::shared_ptr<PbdObject>
makePbdString(const std::string& name, const std::string& filename)
{
    // Setup the Geometry
    std::shared_ptr<LineMesh> stringMesh;
    if (!filename.empty())
    {
        stringMesh = MeshIO::read<LineMesh>(filename);
    }
    else
    {
        stringMesh = makeSelfCCDGeometry();
    }

    const int numVerts = stringMesh->getNumVertices();

    // Setup the Parameters
    imstkNew<PbdModelConfig> pbdParams;
    if (name == "granny_knot")
    {
        pbdParams->m_fixedNodeIds = { 0, 1, size_t(stringMesh->getNumVertices() - 2), size_t(stringMesh->getNumVertices() - 1) };
        pbdParams->enableConstraint(PbdModelConfig::ConstraintGenType::Distance, 2.0);
        pbdParams->enableBendConstraint(1.0, 1);
        //pbdParams->enableBendConstraint(.5, 2);
    }
    else
    {
        pbdParams->m_fixedNodeIds = { 9, 10, selfCCDStringMesh.size() - 2, selfCCDStringMesh.size() - 1 };
        pbdParams->enableConstraint(PbdModelConfig::ConstraintGenType::Distance, 2.0);
        pbdParams->enableBendConstraint(0.1, 1);
        //pbdParams->enableBendConstraint(.5, 2);
    }

    pbdParams->m_uniformMassValue = 0.0001 / numVerts; // grams
    pbdParams->m_gravity = Vec3d(0.0, -9.8, 0.0);
    pbdParams->m_dt      = 0.001;
    // Very important parameter for stability of solver, keep lower than 1.0:
    pbdParams->m_contactStiffness    = 0.1;
    pbdParams->m_iterations          = 20;
    pbdParams->m_viscousDampingCoeff = 0.03;

    // Setup the Model
    imstkNew<PbdModel> pbdModel;
    pbdModel->setModelGeometry(stringMesh);
    pbdModel->configure(pbdParams);

    // Setup the VisualModel
    imstkNew<RenderMaterial> material;
    material->setBackFaceCulling(false);
    material->setColor(Color::Red);
    material->setLineWidth(2.0);
    material->setPointSize(6.0);
    material->setDisplayMode(RenderMaterial::DisplayMode::Wireframe);

    imstkNew<VisualModel> visualModel;
    visualModel->setGeometry(stringMesh);
    visualModel->setRenderMaterial(material);

    // Setup the Object
    imstkNew<PbdObject> stringObj(name);
    stringObj->addVisualModel(visualModel);
    stringObj->setPhysicsGeometry(stringMesh);
    stringObj->setCollidingGeometry(stringMesh);
    stringObj->setDynamicalModel(pbdModel);

    return stringObj;
}

///
/// \brief This example is an initial suturing example testbed. It provides the constraint
/// required for an arc shaped needle puncturing vs a static/immovable tissue. What it
/// does not do:
///  - The tissue is not deformable yet, so insertion is a bit stiff
///  - It only constrains the arc to the surface point it punctures not the volume
///  - The suture thread isn't constrained yet
///  - Ability to graps/release the needle, combining into one body
///
int
main()
{
    // Setup logger (write to file and stdout)
    Logger::startLogger();

    imstkNew<Scene> scene("PBDStaticSuture");

    std::shared_ptr<PbdObject> movingLine =
        // makePbdString("selfCCDLine", "");
        makePbdString("granny_knot", iMSTK_DATA_ROOT "/LineMesh/granny_knot.obj");

    scene->addSceneObject(movingLine);

    std::shared_ptr<PbdObjectCollision> interaction = std::make_shared<PbdObjectCollision>(movingLine, movingLine, "LineMeshToLineMeshCCD");
    interaction->setFriction(0.0);
    scene->addInteraction(interaction);

    // Create the arc needle
    imstkNew<NeedleObject> needleObj;
    needleObj->setForceThreshold(2.0);
    scene->addSceneObject(needleObj);

    // Adjust the camera
    scene->getActiveCamera()->setFocalPoint(0, 0, 0);
    scene->getActiveCamera()->setPosition(0, 0, 0.4);
    scene->getActiveCamera()->setViewUp(0, 1, 0);

    // Run the simulation
    {
        // Setup a viewer to render
        imstkNew<VTKViewer> viewer;
        viewer->setActiveScene(scene);
        viewer->setDebugAxesLength(0.01, 0.01, 0.01);

        // Setup a scene manager to advance the scene
        imstkNew<SceneManager> sceneManager;
        sceneManager->setActiveScene(scene);
        sceneManager->pause(); // Start simulation paused

        // Setup a simulation manager to manage renders & scene updates
        imstkNew<SimulationManager> driver;
        driver->addModule(viewer);
        driver->addModule(sceneManager);
        driver->setDesiredDt(0.001); // 1ms, 1000hz //timestep

        imstkNew<MouseDeviceClient3D> deviceClient(viewer->getMouseDevice());
        deviceClient->setOrientation(Quatd(Rotd(1.57, Vec3d(0.0, 1.0, 0.0))));
        const double translationScaling = 0.2;
        const Vec3d  offset = Vec3d(-0.1, -0.1, 0.0);

        connect<MouseEvent>(viewer->getMouseDevice(), &MouseDeviceClient::mouseScroll,
            [&](MouseEvent* e)
            {
                const Quatd delta = Quatd(Rotd(e->m_scrollDx * 0.1, Vec3d(0.0, 0.0, 1.0)));
                deviceClient->setOrientation(deviceClient->getOrientation() * delta);
            });

#define USE_NEEDLE
#ifdef USE_NEEDLE
        imstkNew<RigidObjectController> controller(needleObj, deviceClient);
        controller->setTranslationOffset(offset);
        controller->setTranslationScaling(translationScaling);
        controller->setLinearKs(1000.0);
        controller->setAngularKs(10000000.0);
        controller->setUseCritDamping(true);
        controller->setForceScaling(0.2);
        controller->setSmoothingKernelSize(5);
        controller->setUseForceSmoothening(true);
        scene->addController(controller);
#else
        imstkNew<SceneObjectController> controller(movingLine, deviceClient);
        controller->setTranslationOffset(offset);
        controller->setTranslationScaling(translationScaling);
        scene->addController(controller);
#endif

        std::shared_ptr<LineMesh> movingLineMesh[2] = { nullptr, nullptr };
        // Update the timesteps for real time
        connect<Event>(sceneManager, &SceneManager::preUpdate,
            [&](Event*)
            {
#ifdef USE_NEEDLE
                auto movingLineMesh = std::dynamic_pointer_cast<LineMesh>(movingLine->getPhysicsGeometry());
                auto needleLineMesh = std::dynamic_pointer_cast<LineMesh>(needleObj->getPhysicsGeometry());
                (*movingLineMesh->getVertexPositions())[1] = (*needleLineMesh->getVertexPositions())[0];
                (*movingLineMesh->getVertexPositions())[0] = (*needleLineMesh->getVertexPositions())[1];
#endif // USE_NEEDLE
            });
        // Constrain the first two vertices of the string to the needle
        connect<Event>(sceneManager, &SceneManager::postUpdate,
            [&](Event*)
            {
            });

        // Add mouse and keyboard controls to the viewer
        imstkNew<MouseSceneControl> mouseControl(viewer->getMouseDevice());
        mouseControl->setSceneManager(sceneManager);
        viewer->addControl(mouseControl);

        imstkNew<KeyboardSceneControl> keyControl(viewer->getKeyboardDevice());
        keyControl->setSceneManager(sceneManager);
        keyControl->setModuleDriver(driver);
        viewer->addControl(keyControl);

        driver->start();
    }

    return 0;
}