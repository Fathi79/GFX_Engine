#pragma once

#include "../ecs/world.hpp"
#include "../components/camera.hpp"
#include "../components/free-camera-controller.hpp"

#include "../application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include "../components/wall.hpp"
#include "../components/metal.hpp"
#include "../components/zwall.hpp"
#include "../components/scarecrow.hpp"


namespace our
{

    // The free camera controller system is responsible for moving every entity which contains a FreeCameraControllerComponent.
    // This system is added as a slightly complex example for how use the ECS framework to implement logic. 
    // For more information, see "common/components/free-camera-controller.hpp"
    class FreeCameraControllerSystem {
        Application* app; // The application in which the state runs
        bool mouse_locked = false; // Is the mouse locked  

    public:
        bool iscolide;
        bool f=false;


        // When a state enters, it should call this function and give it the pointer to the application
        void enter(Application* app){
            this->app = app;

        }

        // This should be called every frame to update all entities containing a FreeCameraControllerComponent 
        void update(World* world, float deltaTime) {
            // First of all, we search for an entity containing both a CameraComponent and a FreeCameraControllerComponent
            // As soon as we find one, we break
            CameraComponent* camera = nullptr;
            FreeCameraControllerComponent *controller = nullptr;
            for(auto entity : world->getEntities()){
                camera = entity->getComponent<CameraComponent>();
                controller = entity->getComponent<FreeCameraControllerComponent>();
                if(camera && controller) break;
            }
            // If there is no entity with both a CameraComponent and a FreeCameraControllerComponent, we can do nothing so we return
            if(!(camera && controller)) return;
            // Get the entity that we found via getOwner of camera (we could use controller->getOwner())
            Entity* entity = camera->getOwner();


            // We get a reference to the entity's position and rotation
            glm::vec3& position = entity->localTransform.position;
            glm::vec3& rotation = entity->localTransform.rotation;

            // If the left mouse button is pressed, we get the change in the mouse location
            // and use it to update the camera rotation
            if(app->getMouse().isPressed(GLFW_MOUSE_BUTTON_1)){
                glm::vec2 delta = app->getMouse().getMouseDelta();
                //rotation.x -= delta.y * controller->rotationSensitivity; // The y-axis controls the pitch
                //rotation.y -= delta.x * controller->rotationSensitivity; // The x-axis controls the yaw
            }

            // We prevent the pitch from exceeding a certain angle from the XZ plane to prevent gimbal locks
            if(rotation.x < -glm::half_pi<float>() * 0.99f) rotation.x = -glm::half_pi<float>() * 0.99f;
            if(rotation.x >  glm::half_pi<float>() * 0.99f) rotation.x  = glm::half_pi<float>() * 0.99f;
            // This is not necessary, but whenever the rotation goes outside the 0 to 2*PI range, we wrap it back inside.
            // This could prevent floating point error if the player rotates in single direction for an extremely long time. 
            rotation.y = glm::wrapAngle(rotation.y);

            // We update the camera fov based on the mouse wheel scrolling amount
            // float fov = camera->fovY + app->getKeyboard().isPressed(GLFW_KEY_LEFT_SHIFT) * controller->fovSensitivity;
            // fov = glm::clamp(fov, glm::pi<float>() * 0.01f, glm::pi<float>() * 0.99f); // We keep the fov in the range 0.01*PI to 0.99*PI
            // camera->fovY = fov;
            float fov;
            if (app->getKeyboard().justPressed(GLFW_KEY_LEFT_SHIFT)&&app->getKeyboard().isPressed(GLFW_KEY_W))
            {   
                f=true;
                // fov = camera->fovY + 0.99*1.2;
                // fov = glm::clamp(fov, glm::pi<float>() * 0.01f, glm::pi<float>() * 0.99f); // We keep the fov in the range 0.01*PI to 0.99*PI
                // camera->fovY = fov;
            }
            if(app->getKeyboard().justReleased(GLFW_KEY_LEFT_SHIFT)&&app->getKeyboard().isPressed(GLFW_KEY_W)&&f){
                f=false;
                // fov = camera->fovY- 0.99*1.2;
                // fov = glm::clamp(fov, glm::pi<float>() * 0.01f, glm::pi<float>() * 0.99f); // We keep the fov in the range 0.01*PI to 0.99*PI
                // camera->fovY = fov;
            }
         
            
            // We get the camera model matrix (relative to its parent) to compute the front, up and right directions
            glm::mat4 matrix = entity->localTransform.toMat4();

            glm::vec3 front = glm::vec3(matrix * glm::vec4(0, 0, -1, 0)),
                      up = glm::vec3(matrix * glm::vec4(0, 1, 0, 0)), 
                      right = glm::vec3(matrix * glm::vec4(1, 0, 0, 0));

            glm::vec3 current_sensitivity = controller->positionSensitivity;
            // If the LEFT SHIFT key is pressed, we multiply the position sensitivity by the speed up factor
            if(app->getKeyboard().isPressed(GLFW_KEY_LEFT_SHIFT)) current_sensitivity *= controller->speedupFactor*1.2;

            // We change the camera position based on the keys WS
            // S & W moves the player back and forth
            if(app->getKeyboard().isPressed(GLFW_KEY_W)) position += glm::vec3(0.2,0.2,0.2)*front * (deltaTime * (current_sensitivity.z));
            iscolide = iscollide(world,position);
            if(iscolide) position -= glm::vec3(0.2,0.2,0.2)*front * (deltaTime * current_sensitivity.z);

            if(app->getKeyboard().isPressed(GLFW_KEY_S)&&!iscolide) position -= glm::vec3(0.2,0.2,0.2)*front * (deltaTime * current_sensitivity.z);
            iscolide = iscollide(world,position);
            if(iscolide) position += glm::vec3(0.2,0.2,0.2)*front * (deltaTime * current_sensitivity.z);


            // A & D moves the player left or right 
            if(app->getKeyboard().isPressed(GLFW_KEY_D)) rotation.y -= deltaTime* 200 * controller->rotationSensitivity;
            if(app->getKeyboard().isPressed(GLFW_KEY_A)) rotation.y += deltaTime* 200 * controller->rotationSensitivity;

            // Q & E moves the player left or right (for building the maze)
            if(app->getKeyboard().isPressed(GLFW_KEY_Q)) position += up * (deltaTime * current_sensitivity.y);
            if(app->getKeyboard().isPressed(GLFW_KEY_E)) position -= up * (deltaTime * current_sensitivity.y);

            // iscolide = iscollide(world,position);
            // if(iscolide) position -= front * (deltaTime * current_sensitivity.z);

            // Change the condition to reaching the exit
            if(position.z < -9.5 && position.x > -5.5 && position.x < -4.8) app->changeState("menu");
        }

        // When the state exits, it should call this function to ensure the mouse is unlocked
        void exit(){
            if(mouse_locked) {
                mouse_locked = false;
                app->getMouse().unlockMouse(app->getWindow());
            }
        }



        // Collision detection handling
        bool iscollide(World*World, glm::vec3& position){

            glm::vec3 wallPosition;
            glm::vec3 zwallPosition;
            glm::vec3 crowPosition;
            
            auto entities = World->getEntities();

            for(auto entity : entities)
            {
                if(entity->getComponent<scarecrow>())
                {
                    crowPosition = glm::vec3(entity->getLocalToWorldMatrix() *glm::vec4(entity->localTransform.position, 1.0));
                    if(abs(position.x-crowPosition.x)<0.4 && abs(position.z-crowPosition.z)<0.1)
                    {
                        app->changeState("menu");
                    }
                }
                if(entity->getComponent<wall>())
                {
                    wallPosition =glm::vec3(entity->getLocalToWorldMatrix() *glm::vec4(entity->localTransform.position, 1.0));


                    if(abs(position.x-wallPosition.x)<0.45&&abs(position.z-wallPosition.z)<0.1)
                    {
                        return true;
                    }
                }
                if(entity->getComponent<zwall>())
                {
                    zwallPosition =glm::vec3(entity->getLocalToWorldMatrix() *glm::vec4(entity->localTransform.position, 1.0));


                    if(abs(position.x-zwallPosition.x)<0.1&&abs(position.z-zwallPosition.z)<0.45)
                    {
                        return true;
                    }
                }

            }
            return false;
    
        }




    };

}
