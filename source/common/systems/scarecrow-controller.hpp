#pragma once

#include "../ecs/world.hpp"
#include "../components/camera.hpp"
#include "../components/free-camera-controller.hpp"
#include "../components/scarecrow-controller.hpp"

#include "../application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include "../components/wall.hpp"
#include "../components/metal.hpp"
#include "../components/zwall.hpp"
#include "../components/scarecrow.hpp"
#include "../components/movement.hpp"

#define COLLIDED_WITH_XWALL 1
#define COLLIDED_WITH_ZWALL -1
#define NO_COLLISION 0

namespace our
{

    // The free camera controller system is responsible for moving every entity which contains a FreeCameraControllerComponent.
    // This system is added as a slightly complex example for how use the ECS framework to implement logic. 
    // For more information, see "common/components/free-camera-controller.hpp"
    class ScareCrowControllerSystem {
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
            scarecrow* sc = nullptr;
            ScareCrowControllerComponent *controller = nullptr;
            // Loop over all the scarecrows to update them
            for(auto entity : world->getEntities()){
                sc = entity->getComponent<scarecrow>();
                controller = entity->getComponent<ScareCrowControllerComponent>();

                // If there is no entity with both a CameraComponent and a FreeCameraControllerComponent, we can do nothing so we return
                if(!(sc && controller)) continue;

                // We get a reference to the entity's position and rotation
                glm::vec3& position = entity->localTransform.position;
                glm::vec3& rotation = entity->localTransform.rotation;

                // When the scarecrow collides with a wall, it changes its motion direction
                if(iscollide(world, position) == COLLIDED_WITH_ZWALL)
                {
                    entity->getComponent<MovementComponent>()->linearVelocity.x *= -1;
                }
                if(iscollide(world, position) == COLLIDED_WITH_XWALL
                    || position.z < -9 && position.x > -5.5 && position.x < -4.8)
                {
                    entity->getComponent<MovementComponent>()->linearVelocity.z *= -1;
                }
            }
            
        }

        // When the state exits, it should call this function to ensure the mouse is unlocked
        void exit(){
            if(mouse_locked) {
                mouse_locked = false;
                app->getMouse().unlockMouse(app->getWindow());
            }
        }



        // Collision detection handling
        int iscollide(World*World, glm::vec3& position){

            glm::vec3 wallPosition;
            glm::vec3 zwallPosition;
            
            auto entities = World->getEntities();

            //Loop over all walls to check if the scarecrow collided with any of them
            for(auto entity : entities)
            {
                if(entity->getComponent<wall>())
                {
                    wallPosition =glm::vec3(entity->getLocalToWorldMatrix() *glm::vec4(entity->localTransform.position, 1.0));


                    if(abs(position.x-wallPosition.x)<0.45 && abs(position.z-wallPosition.z)<0.1)
                    {
                        return COLLIDED_WITH_XWALL;
                    }
                }
                if(entity->getComponent<zwall>())
                {
                    zwallPosition =glm::vec3(entity->getLocalToWorldMatrix() *glm::vec4(entity->localTransform.position, 1.0));


                    if(abs(position.x-zwallPosition.x)<0.1 && abs(position.z-zwallPosition.z)<0.45)
                    {
                        return COLLIDED_WITH_ZWALL;
                    }
                }

            }
            return NO_COLLISION;
    
        }




    };

}
