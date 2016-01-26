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

#ifndef RENDERING_VTKVIEWER_H
#define RENDERING_VTKVIEWER_H

#include "Core/ViewerBase.h"
#include "VirtualTools/LaparoscopicCameraController.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkContextScene.h"
#include "vtkContextActor.h"
#include "vtkRenderWindowInteractor.h"

class vtkCamera;
class vtkRenderWindowInteractor;
class vtkRenderer;

class VTKViewer : public ViewerBase
{

public:
    VTKViewer();
    ~VTKViewer();

    VTKViewer(const VTKViewer& other) = delete;
    VTKViewer& operator=(const VTKViewer& other) = delete;

public:
    ///
    /// \brief Execute when exit viewer
    ///
    void exitViewer() override {}

    ///
    /// \brief Add text for display
    ///
    void addText(std::string /*tag*/) override {}

    ///
    /// \brief Update text
    ///
    void updateText(std::string /*tag*/, std::string /*string*/) override {}

    ///
    /// \brief Update text
    ///
    void updateText(int /*handle*/, std::string /*string*/) override {}

    ///
    /// \brief Enable/disable VSync
    ///
    void setVSync(bool /*sync*/) override {}

    ///
    /// \brief Execute this module
    ///
    void exec() override;

    ///
    /// \brief Run when frame begins
    ///
    void beginFrame() override {}

    ///
    /// \brief Run when frame ends
    ///
    void endFrame() override {}

    ///
    /// \brief Returns the pointer to the current vtk camera
    ///
    vtkCamera* getVtkCamera();

	///
	/// \brief
	///
    vtkRenderer* getVtkRenderer();

	///
	/// \brief
	///
    vtkRenderWindowInteractor* getVtkRenderWindowInteractor();

    ///
    /// \brief Add chart actor to the renderer
    ///
    void addChartActor(vtkContextActor* chartActor, vtkContextScene* chartScene);

    vtkRenderWindow* getVtkRenderWindow() const;
    void setVtkRenderWindow(vtkSmartPointer<vtkRenderWindow> renWin);

protected:
    ///
    /// \brief Renders the render operation to screen
    ///
    virtual void renderToScreen(const RenderOperation &/*rop*/) override {}

    ///
    /// \brief Renders the render operation to an FBO
    ///
    void renderToFBO(const RenderOperation &/*rop*/) override {}

    ///
    /// \brief Initializes rendering system (e.g., OpenGL) capabilities and flags
    ///
    void initRenderingCapabilities() override {}

    ///
    /// \brief Initilizes the rendering system (e.g., OpenGL) context, and window containing it
    virtual void initRenderingContext() override;

    ///
    /// \brief Cleans up after initGLContext()
    ///
    void destroyRenderingContext() override {}

    ///
    /// \brief Initializes FBOs, textures, shaders and VAOs
    ///
    void initResources() override {}

    ///
    /// \brief Processes viewerRenderDetail options
    ///
    void processViewerOptions() override {}

    ///
    /// \brief Process window events and render as the major part of an event loop
    ///
    void processWindowEvents() override {}

    ///
    /// \brief Set the color and other viewer defaults
    ///
    void setToDefaults() override {}

    ///
    /// \brief render depth texture for debugging
    ///
    void renderTextureOnView() override {}

    ///
    /// \brief Render scene
    ///
    void render() override;
private:
    class VTKRenderer;
    std::unique_ptr<VTKRenderer> renderer;
};

#endif // VTKVIEWER_H