//
//  Camera.cpp
//  Lab5
//
//  Created by CGIS on 28/10/2016.
//  Copyright © 2016 CGIS. All rights reserved.
//

#include "Camera.hpp"

namespace gps {
    
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, float boundsL, float boundsR, float boundsFB)
    {
        /*
        BEGIN: Basketball - Spiel : Initialisierung der neuen Kamera Variablen
        Author : Fabian Vogt(s0570800)
        */
        this->lastX = 0;
        this->lastY = 0;
        this->firstMouse = true;
        this->pitch = 0;
        this->yaw = 90;
        this->boundsL = boundsL;
        this->boundsR = boundsR;
        this->boundsFB = boundsFB;
        /*
        END: Basketball - Spiel : Erweiterung der Kamera
        */
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
    }
    
    glm::mat4 Camera::getViewMatrix()
    {
        return glm::lookAt(cameraPosition, cameraPosition + cameraDirection , glm::vec3(0.0f, 1.0f, 0.0f));
    }
    /*
    BEGIN: Basketball - Spiel : Zusätzliche Hilfsfunktionen der Kamera
    Author : Fabian Vogt(s0570800)
    */
	glm::vec3 Camera::getCameraTarget()
	{
		return cameraTarget;
	}

    glm::vec3 Camera::getPosition()
    {
        return cameraPosition;
    }

    glm::vec3 Camera::getDirection()
    {
        return cameraDirection;
    }
    void Camera::setHeight(double height)
    {
        this->cameraPosition.y = height;
    }
    /*
    END: Basketball - Spiel : Zusätzliche Hilfsfunktionen der Kamera
    */
    /*
    BEGIN: Basketball - Spiel : Move überarbeitet, sodass nur Bewegungen innerhalb der Grenzen möglich sind
    Author : Fabian Vogt(s0570800)
    */
    void Camera::move(MOVE_DIRECTION direction, float speed)
    {
        glm::vec3 cameraPosTemp = cameraPosition;

        double heightTemp = cameraPosition.y;
   
        switch (direction) {
            case MOVE_FORWARD:
                cameraPosTemp += cameraDirection * speed;
                break;
                
            case MOVE_BACKWARD:

                cameraPosTemp -= cameraDirection * speed;
                break;
                
            case MOVE_RIGHT:

                cameraPosTemp += cameraRightDirection * speed;
                break;
                
            case MOVE_LEFT:

                cameraPosTemp -= cameraRightDirection * speed;
                break;
        }
        cameraPosTemp.y = heightTemp;

        // check if this move is still within the boundaries
        if (cameraPosTemp.z > boundsFB ||
            cameraPosTemp.z < -boundsFB ||
            cameraPosTemp.x > boundsL ||
            cameraPosTemp.x < -boundsR) {
            return;
        }
        cameraPosition = cameraPosTemp;
    }
    /*
    END: Basketball - Spiel : Move überarbeitet, sodass nur Bewegungen innerhalb der Grenzen möglich sind
    */
    
    void Camera::rotate(float pitch, float yaw)
    {
        cameraDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraDirection.y = sin(glm::radians(pitch));
        cameraDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f)));

    }

    /*
    BEGIN: Basketball - Spiel : Maussteuerung zum Umschauen
    Author : Fabian Vogt(s0570800)
    */
    void Camera::mouseRotate(GLFWwindow* window, double xpos, double ypos) {

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;

        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraDirection = glm::normalize(direction);
        cameraRightDirection = glm::normalize(glm::cross(this->cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
    }
    /*
    END: Basketball - Spiel : Maussteuerung zum Umschauen
    */
}
