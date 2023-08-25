#include <glm/ext/matrix_clip_space.hpp>
#include "Camera.h"

Camera::Camera()
{
}

Camera::~Camera()
{
}

void Camera::update(float deltaTime)
{
    if (moving())
    {
        if (mode == Camera::FIRST_PERSON)
        {
            glm::vec3 camFront;
            camFront.x = -cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
            camFront.y = sin(glm::radians(rotation.x));
            camFront.z = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
            camFront = glm::normalize(camFront);

            float moveDistance = deltaTime * moveSpeed;

            if (keys.up)
            {
                position += camFront * moveDistance;
            }
            if (keys.down)
            {
                position -= camFront * moveDistance;
            }
            if (keys.left)
                position -= glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) * moveDistance;
            if (keys.right)
                position += glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) * moveDistance;
            updateViewMatrix();
        }
    }
}

bool Camera::moving() const
{
    return keys.up || keys.down || keys.left || keys.right;
}

void Camera::updateViewMatrix()
{
    //    if (dirty) {
    glm::mat4 rotM = glm::mat4(1.0f);
    glm::mat4 transM{1};

    rotM = glm::rotate(rotM, glm::radians(rotation.x * (flipY ? -1.0f : 1.0f)), glm::vec3(1.0f, 0.0f, 0.0f));
    rotM = glm::rotate(rotM, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotM = glm::rotate(rotM, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    transM = glm::translate(transM, position);

    matrices.view = rotM * transM;

    //    }
}

void Camera::setPerspective(float fov, float aspect, float znear, float zfar)
{
    this->fov = fov;
    this->zNear = znear;
    this->zFar = zfar;
    matrices.perspective = glm::perspective(glm::radians(fov), aspect, znear, zfar);
    if (flipY)
    {
        matrices.perspective[1][1] *= -1.0f;
    }
}

void Camera::setRotation(glm::vec3 rotation)
{
    this->rotation = rotation;
    updateViewMatrix();
}

void Camera::setTranslation(glm::vec3 translation)
{
    this->position = translation;
    updateViewMatrix();
}

void Camera::setRotationByCamFront(glm::vec3 camFront)
{
    ////    camFront.x = -cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
    ////    camFront.y = sin(glm::radians(rotation.x));
    ////    camFront.z = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
    ////    rotation.x =
    //    rotation.x = glm::degrees(std::asin(camFront.y));
    //    rotation.y = glm::degrees(std::acos(camFront.z / cos(glm::radians(rotation.x))));
    ////    rotation.x = 0;
    ////    rotation.y = 0;
    //    rotation.z = 0;
    //    updateViewMatrix();
}