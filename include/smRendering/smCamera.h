#ifndef _GLM_CAMERA_H_
#define _GLM_CAMERA_H_

/// STL icludes
#include <memory>

// GLM includes
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

/// \brief A simple camera class that uses glm as a backend
///
/// \detail Everything is public because I am going to leave it up to the
/// programmer to not screw themselves or others
///
class smCamera
{
public:
    //Construction/Destruction
    smCamera();
    //Nothing to clean up

    //View settings
    void setCameraPos(float x, float y, float z);
    void setCameraPos(const glm::vec3& v);
    void setCameraFocus(float x, float y, float z);
    void setCameraFocus(const glm::vec3& v);
    void setCameraUpVec(float x, float y, float z);
    void setCameraUpVec(const glm::vec3& v);

    //Projection settings
    void setAspectRatio(const float ar);
    void setViewAngle(const float a);
    void setNearClipDist(const float d);
    void setFarClipDist(const float d);

    //Get matrix data
    glm::mat4 getViewMat();
    float* getViewMatRef();
    glm::mat4 getProjMat();
    float* getProjMatRef();

    //Create matrices
    void genViewMat();
    void genProjMat();

    static std::shared_ptr<smCamera> getDefaultCamera()
    {
        std::shared_ptr<smCamera> defaultCamera = std::make_shared<smCamera>();
        defaultCamera->setAspectRatio(800.0 / 640.0); //Doesn't have to match screen resolution
        defaultCamera->setFarClipDist(1000);
        defaultCamera->setNearClipDist(0.001);
        defaultCamera->setViewAngle(0.785398f); //45 degrees
        defaultCamera->setCameraPos(0, 0, 10);
        defaultCamera->setCameraFocus(0, 0, 0);
        defaultCamera->setCameraUpVec(0, 1, 0);
        defaultCamera->genProjMat();
        defaultCamera->genViewMat();
        return defaultCamera;
    }

    //View matrix variables
    glm::vec3 pos; ///< position of the camera
    glm::vec3 fp; ///< focal point of the camera
    glm::vec3 up; ///< the up direction for the camera

    //Projection matrix variables
    float ar; ///< aspect ratio
    float angle; ///< angle in radians
    float nearClip; ///< near clipping distance
    float farClip; ///< far clipping distance

    //functional matrices
    glm::mat4 view; ///< View matrix for OpenGL
    glm::mat4 proj; ///< Projection matrix for OpenGL

};

#endif
