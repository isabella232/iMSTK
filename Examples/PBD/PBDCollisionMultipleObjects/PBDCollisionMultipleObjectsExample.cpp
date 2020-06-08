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
#include "imstkCollisionGraph.h"
#include "imstkLight.h"
#include "imstkMeshIO.h"
#include "imstkObjectInteractionFactory.h"
#include "imstkOneToOneMap.h"
#include "imstkPbdModel.h"
#include "imstkScene.h"
#include "imstkSimulationManager.h"
#include "imstkSurfaceMesh.h"
#include "imstkTetrahedralMesh.h"

// Enable this macro to generate many dragons
#define BIG_SCENE

using namespace imstk;

///
/// \brief Generate a random color
///
Color
getRandomColor()
{
    Color color(0, 0, 0, 1);
    while (true)
    {
        for (unsigned int i = 0; i < 3; ++i)
        {
            color.rgba[i] = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
        }
        if (color.rgba[0] > 0.95
            || color.rgba[1] > 0.95
            || color.rgba[2] > 0.95)
        {
            break;
        }
    }
    return color;
}

void
generateDragon(const std::shared_ptr<imstk::Scene>& scene,
               const Vec3d&                         translation,
               std::shared_ptr<SurfaceMesh>&        surfMesh,
               std::shared_ptr<PbdObject>&          deformableObj,
               std::shared_ptr<PbdSolver>&          pbdSolver)
{
    // Load a sample mesh
    auto tetMesh = MeshIO::read(iMSTK_DATA_ROOT "/asianDragon/asianDragon.veg");
    LOG_IF(FATAL, (!tetMesh)) << "Could not read mesh from file.";

    auto volTetMesh = std::dynamic_pointer_cast<TetrahedralMesh>(tetMesh);
    LOG_IF(FATAL, (!volTetMesh)) << "Dynamic pointer cast from PointSet to TetrahedralMesh failed!";

    // Rotate a rando angle
    volTetMesh->rotate(Vec3d(0, 1, 0), static_cast<double>(rand()), Geometry::TransformType::ApplyToData);

    // Translate
    volTetMesh->translate(translation, Geometry::TransformType::ApplyToData);

    // Trick to force update geometry postUpdateTransform
    const auto positions = volTetMesh->getVertexPositions();
    (void)positions;

    static int count = -1;
    ++count;
    surfMesh = std::make_shared<SurfaceMesh>("Dragon-" + std::to_string(count));
    volTetMesh->extractSurfaceMesh(surfMesh, true);

    auto material = std::make_shared<RenderMaterial>();

#if 0
    // Wireframe color
    material->setDisplayMode(RenderMaterial::DisplayMode::WIREFRAME_SURFACE);
    material->setDebugColor(getRandomColor()); // Wireframe color
    material->setLineWidth(2);
#else
    material->setDisplayMode(RenderMaterial::DisplayMode::Surface);
    material->setColor(getRandomColor());
#endif

    auto surfMeshModel = std::make_shared<VisualModel>(surfMesh);
    surfMeshModel->setRenderMaterial(material);

    auto deformMapP2C = std::make_shared<OneToOneMap>(tetMesh, surfMesh);

    deformableObj = std::make_shared<PbdObject>("Dragon-" + std::to_string(count));
    deformableObj->addVisualModel(surfMeshModel);
    deformableObj->setCollidingGeometry(surfMesh);
    deformableObj->setPhysicsGeometry(volTetMesh);
    deformableObj->setPhysicsToCollidingMap(deformMapP2C);

    // Create model and object
    auto pbdModel = std::make_shared<PbdModel>();
    pbdModel->setModelGeometry(volTetMesh);

    // configure model
    auto pbdParams = std::make_shared<PBDModelConfig>();

    // FEM constraint
    pbdParams->femParams->m_YoungModulus = 1000.0;
    pbdParams->femParams->m_PoissonRatio = 0.3;
    pbdParams->enableFEMConstraint(PbdConstraint::Type::FEMTet, PbdFEMConstraint::MaterialType::StVK);

    // Other parameters
    pbdParams->m_uniformMassValue = 5.0;
    pbdParams->m_gravity    = Vec3d(0, -1.0, 0);
    pbdParams->m_defaultDt  = 0.01;
    pbdParams->m_iterations = 20;
    pbdParams->collisionParams->m_proximity = 0.5;

    pbdModel->configure(pbdParams);
    deformableObj->setDynamicalModel(pbdModel);

    scene->addSceneObject(deformableObj);
}

///
/// \brief This example demonstrates the collision interaction
/// using Position based dynamics
///
int
main()
{
    auto simManager = std::make_shared<SimulationManager>();
    auto scene      = simManager->createNewScene("PbdCollision");

    // Get the VTKViewer
    auto viewer = std::dynamic_pointer_cast<VTKViewer>(simManager->getViewer());
    viewer->getVtkRenderWindow()->SetSize(1920, 1080);

    // Build floor geometry
    const double width  = 100.0;
    const double height = 100.0;
    const size_t nRows  = 2;
    const size_t nCols  = 2;
    const double dy     = width / static_cast<double>(nCols - 1);
    const double dx     = height / static_cast<double>(nRows - 1);

    StdVectorOfVec3d vertList;
    vertList.resize(nRows * nCols);
    for (size_t i = 0; i < nRows; ++i)
    {
        for (size_t j = 0; j < nCols; j++)
        {
            const double y = static_cast<double>(dy * j);
            const double x = static_cast<double>(dx * i);
            vertList[i * nCols + j] = Vec3d(x - 50, -10.0, y - 50);
        }
    }

    // c. Add connectivity data
    std::vector<SurfaceMesh::TriangleArray> triangles;
    for (std::size_t i = 0; i < nRows - 1; ++i)
    {
        for (std::size_t j = 0; j < nCols - 1; j++)
        {
            SurfaceMesh::TriangleArray tri[2];
            tri[0] = { { i* nCols + j, i* nCols + j + 1, (i + 1) * nCols + j } };
            tri[1] = { { (i + 1) * nCols + j + 1, (i + 1) * nCols + j, i* nCols + j + 1 } };
            triangles.push_back(tri[0]);
            triangles.push_back(tri[1]);
        }
    }

    auto floorObj = std::make_shared<PbdObject>("Floor");
    {
        auto floorMesh = std::make_shared<SurfaceMesh>();
        floorMesh->initialize(vertList, triangles);

        auto materialFloor = std::make_shared<RenderMaterial>();
        materialFloor->setDisplayMode(RenderMaterial::DisplayMode::WireframeSurface);
        auto floorMeshModel = std::make_shared<VisualModel>(floorMesh);
        floorMeshModel->setRenderMaterial(materialFloor);

        floorObj->setCollidingGeometry(floorMesh);
        floorObj->setVisualGeometry(floorMesh);
        floorObj->setPhysicsGeometry(floorMesh);

        auto pbdFloorModel = std::make_shared<PbdModel>();
        pbdFloorModel->setModelGeometry(floorMesh);

        // Configure floor
        auto pbdFloorConfig = std::make_shared<PBDModelConfig>();
        pbdFloorConfig->m_uniformMassValue = 0.0;
        pbdFloorConfig->collisionParams->m_proximity = -0.1;
        pbdFloorConfig->m_iterations = 0;

        // Set the parameters
        pbdFloorModel->configure(pbdFloorConfig);
        floorObj->setDynamicalModel(pbdFloorModel);
        scene->addSceneObject(floorObj);
    }

#ifdef BIG_SCENE
    const int expandsXZ = 1;
    const int expandsY  = 2;
#endif
    const double shiftX     = 5.0;
    const double distanceXZ = 10.0;
    const double distanceY  = 5.0;
    const double minHeight  = -5.0;

    std::vector<std::shared_ptr<PbdObject>> pbdObjs;

#ifdef BIG_SCENE
    for (int i = -expandsXZ; i < expandsXZ; ++i)
#else
    int i = 0;
#endif
    {
#ifdef BIG_SCENE
        for (int j = 0; j < expandsY; ++j)
#else
        int j = 0;
#endif
        {
#ifdef BIG_SCENE
            for (int k = -expandsXZ; k < expandsXZ; ++k)
#else
            int k = 0;
#endif
            {
                std::shared_ptr<SurfaceMesh> mesh;
                std::shared_ptr<PbdObject>   pbdObj;
                std::shared_ptr<PbdSolver>   solver;
                Vec3d                        translation(shiftX + i * distanceXZ, minHeight + j * distanceY, k * distanceXZ);
                generateDragon(scene, translation, mesh, pbdObj, solver);
                pbdObjs.push_back(pbdObj);

                scene->getCollisionGraph()->addInteraction(makeObjectInteractionPair(pbdObj, floorObj,
                    InteractionType::PbdObjToPbdObjCollision, CollisionDetection::Type::SurfaceMeshToSurfaceMesh));
            }
        }
    }

    for (size_t i = 0; i < pbdObjs.size(); ++i)
    {
        for (size_t j = i + 1; j < pbdObjs.size(); ++j)
        {
            scene->getCollisionGraph()->addInteraction(makeObjectInteractionPair(pbdObjs[i], pbdObjs[j],
                InteractionType::PbdObjToPbdObjCollision, CollisionDetection::Type::SurfaceMeshToSurfaceMesh));
        }
    }

    // Light
    auto light = std::make_shared<DirectionalLight>("light");
    light->setFocalPoint(Vec3d(5, -8, -5));
    light->setIntensity(1);
    scene->addLight(light);

    // Set Camera configuration
    auto cam = scene->getCamera();
    cam->setPosition(Vec3d(0, 15, 30));
    cam->setFocalPoint(Vec3d(0, 0, 0));

    simManager->setActiveScene(scene);
    simManager->start(SimulationStatus::Paused);

    return 0;
}
