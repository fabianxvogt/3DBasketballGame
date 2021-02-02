//
//  Camera.hpp
//
//  Created by CGIS on 28/10/2016.
//  Copyright © 2016 CGIS. All rights reserved.
//

#ifndef Camera_hpp
#define Camera_hpp

#include <stdio.h>
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "GLFW/glfw3.h"

namespace gps {
    
    enum MOVE_DIRECTION {MOVE_FORWARD, MOVE_BACKWARD, MOVE_RIGHT, MOVE_LEFT};
    
    class Camera
    {
    public:
        Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, float boundsL, float boundsR, float boundsFB);
        glm::mat4 getViewMatrix();
		glm::vec3 getCameraTarget();
		void move(MOVE_DIRECTION direction, float speed);
        void rotate(float pitch, float yaw);	

        /*
	    BEGIN: Basketball-Spiel: Erweiterung der Kamera
	    Author: Fabian Vogt (s0570800)
        */
        void mouseRotate(GLFWwindow* window, double xpos, double ypos);
        void setHeight(double height);
        glm::vec3 getPosition();
        glm::vec3 getDirection();
        /*
        END: Basketball-Spiel: Erweiterung der Kamera
        */
        
    private:
        glm::vec3 cameraPosition;
        glm::vec3 cameraTarget;
        glm::vec3 cameraDirection;
        glm::vec3 cameraRightDirection;

        /*
        BEGIN: Basketball - Spiel : Erweiterung der Kamera
        Author : Fabian Vogt(s0570800)
        */
        float pitch;
        float yaw;
        bool firstMouse;
        double lastX;
        double lastY;
        float boundsL, boundsR, boundsFB;
        /*
        END: Basketball-Spiel: Erweiterung der Kamera
        */
    };
    
}

#endif /* Camera_hpp */
