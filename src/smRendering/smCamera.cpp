// This file is part of the SimMedTK project.
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
//
//---------------------------------------------------------------------------
//
// Authors:
//
// Contact:
//---------------------------------------------------------------------------

#include "smUtilities/smMath.h"
#include "smUtilities/smQuaternion.h"
#include "smUtilities/smVector.h"
#include "smRendering/smCamera.h"

smCamera::smCamera()
    : ar(4.0 / 3.0),
      angle(SM_DEGREES2RADIANS(45.0)),
      nearClip(0.1),
      farClip(100.0),
      pos(0, 0, 0),
      fp(0, 0, -1),
      orientation(),
      viewDirty(true),
      projDirty(true),
      orientDirty(false)
{
}

smVec3f smCamera::getPos()
{
    std::lock_guard<std::mutex> lock(posLock);
    return this->pos;
}

void smCamera::setPos(const float x, const float y, const float z)
{
    this->setPos(smVec3f(x, y, z));
}

void smCamera::setPos(const smVec3f& v)
{
    {//scoped for mutex release
    std::lock_guard<std::mutex> lock(posLock);
    this->pos = v;
    }
    this->viewDirty.store(true);
    this->orientDirty.store(true);
}

smVec3f smCamera::getFocus()
{
    std::lock_guard<std::mutex> lock(fpLock);
    return this->fp;
}

void smCamera::setFocus(const float x, const float y, const float z)
{
    this->setFocus(smVec3f(x, y, z));
}

void smCamera::setFocus(const smVec3f& v)
{
    { //scoped for mutex release
    std::lock_guard<std::mutex> lock(fpLock);
    this->fp = v;
    }
    this->viewDirty.store(true);
    this->orientDirty.store(true);
}

smVec3f smCamera::getUpVec()
{
    return getOrientation() * smVec3f::UnitY();
}

smVec3f smCamera::getDirection()
{
    return -(getOrientation() * smVec3f::UnitZ());
}

float smCamera::getAspectRatio()
{
    return this->ar.load();
}

void smCamera::setAspectRatio(const float ar)
{
    this->ar.store(ar);
    this->projDirty.store(true);
}

float smCamera::getViewAngle()
{
    return this->angle.load();
}

void smCamera::setViewAngle(const float a)
{
    this->angle.store(a);
    this->projDirty.store(true);
}

float smCamera::getViewAngleDeg()
{
    return SM_RADIANS2DEGREES(getViewAngle());
}

void smCamera::setViewAngleDeg(const float a)
{
    setViewAngle(SM_DEGREES2RADIANS(a));
}

float smCamera::getNearClipDist()
{
    return this->nearClip.load();
}

void smCamera::setNearClipDist(const float d)
{
    this->nearClip.store(d);
    this->projDirty.store(true);
}

float smCamera::getFarClipDist()
{
    return this->farClip.load();
}

void smCamera::setFarClipDist(const float d)
{
    this->farClip.store(d);
    this->projDirty.store(true);
}

void smCamera::setOrientation(const smQuaternionf q)
{
    { //scoped for mutex release
    std::lock_guard<std::mutex> lock(orientationLock);
    this->orientation = q;
    }
    this->orientDirty.store(false);
}

void smCamera::setOrientFromDir(const smVec3f d)
{
    smMatrix33f camAxes;
    smVec3f tempUp;
    { //scoped for mutex release
    std::lock_guard<std::mutex> lock(orientationLock);
    tempUp = this->orientation * smVec3f::UnitY();
    }

    camAxes.col(2) = (-d).normalized();
    camAxes.col(0) = tempUp.cross( camAxes.col(2) ).normalized();
    camAxes.col(1) = camAxes.col(2).cross( camAxes.col(0) ).normalized();
    setOrientation(smQuaternionf(camAxes));
}

smQuaternionf smCamera::getOrientation()
{
    if (true == this->orientDirty.load())
    {
        setOrientFromDir((getFocus() - getPos()).normalized());
    }
    std::lock_guard<std::mutex> lock(orientationLock);
    return this->orientation;
}

smMatrix44f smCamera::getViewMat()
{
    if (true == this->viewDirty.load())
    {
        genViewMat();
    }
    std::lock_guard<std::mutex> lock(viewLock);
    return this->view;
}

void smCamera::setViewMat(const smMatrix44f &m)
{
    { //scoped for mutex release
    std::lock_guard<std::mutex> lock(viewLock);
    this->view = m;
    }
    this->viewDirty.store(false);
}

smMatrix44f smCamera::getProjMat()
{
    if (true == this->projDirty.load())
    {
        genProjMat();
    }
    std::lock_guard<std::mutex> lock(projLock);
    return this->proj;
}

void smCamera::setProjMat(const smMatrix44f &m)
{
    { //scoped for mutex release
    std::lock_guard<std::mutex> lock(projLock);
    this->proj = m;
    }
    this->projDirty.store(false);
}

void smCamera::pan(smVec3f v)
{
    v = getOrientation() * v;
    setPos(getPos() + v);
    setFocus(getFocus() + v);
}

void smCamera::zoom(const float d)
{
    float dist = (getPos() - getFocus()).norm();
    if (dist > d)
    {
        setPos(getPos() + getDirection() * d);
    }
}

void smCamera::rotateLocal(const float angle, const smVec3f axis)
{
    float dist = (getPos() - getFocus()).norm();
    smQuaternionf q;
    q = Eigen::AngleAxisf(angle, axis.normalized());

    setOrientation(getOrientation() * q);
    setFocus(getPos() + dist * getDirection());
}

void smCamera::rotateFocus(const float angle, const smVec3f axis)
{
    float dist = (getFocus() - getPos()).norm();
    smQuaternionf q;
    q = Eigen::AngleAxisf(angle, axis.normalized());

    setOrientation(getOrientation() * q);
    setPos(getFocus() + dist * getDirection());
}

void smCamera::rotateLocalX(const float angle)
{
    rotateLocal(angle, smVec3f::UnitX());
}

void smCamera::rotateLocalY(const float angle)
{
    rotateLocal(angle, smVec3f::UnitY());
}

void smCamera::rotateLocalZ(const float angle)
{
    rotateLocal(angle, smVec3f::UnitZ());
}

void smCamera::rotateFocusX(const float angle)
{
    rotateFocus(angle, smVec3f::UnitX());
}

void smCamera::rotateFocusY(const float angle)
{
    rotateFocus(angle, smVec3f::UnitY());
}

void smCamera::rotateFocusZ(const float angle)
{
    rotateFocus(angle, smVec3f::UnitZ());
}


// Implementation adapted from Sylvain Pointeau's Blog:
// http://spointeau.blogspot.com/2013/12/hello-i-am-looking-at-opengl-3.html
smMatrix44f smCamera::lookAt(const smVec3f pos,
                             const smVec3f fp,
                             const smVec3f up)
{
    smVec3f f = (fp - pos).normalized();
    smVec3f u = up.normalized();
    smVec3f s = f.cross(u).normalized();
    u = s.cross(f);

    smMatrix44f res;
    res <<  s.x(),s.y(),s.z(),-s.dot(pos),
            u.x(),u.y(),u.z(),-u.dot(pos),
            -f.x(),-f.y(),-f.z(),f.dot(pos),
            0,0,0,1;

    return res;
}

void smCamera::genViewMat()
{
    setViewMat(smCamera::lookAt(getPos(), getFocus(), getUpVec()));
}

// Implementation adapted from Sylvain Pointeau's Blog:
// http://spointeau.blogspot.com/2013/12/hello-i-am-looking-at-opengl-3.html
smMatrix44f smCamera::perspective(const float fovy, const float ar,
                                  const float zNear, const float zFar)
{
    assert(ar > 0);
    assert(zFar > zNear);

    double tanHalfFovy = tan(fovy / 2.0);
    smMatrix44f res = smMatrix44f::Zero();

    res(0,0) = 1.0 / (ar * tanHalfFovy);
    res(1,1) = 1.0 / (tanHalfFovy);
    res(2,2) = - (zFar + zNear) / (zFar - zNear);
    res(3,2) = - 1.0;
    res(2,3) = - (2.0 * zFar * zNear) / (zFar - zNear);
    return res;
}

void smCamera::genProjMat()
{
    setProjMat(smCamera::perspective(getViewAngle(), getAspectRatio(),
                                     getNearClipDist(), getFarClipDist()));
}
