#include "entity.hpp"
#include "../deserialize-utils.hpp"

#include <glm/gtx/euler_angles.hpp>

namespace our {

    // This function computes and returns a matrix that represents this transform
    // Remember that the order of transformations is: Scaling, Rotation then Translation
    // HINT: to convert euler angles to a rotation matrix, you can use glm::yawPitchRoll
    glm::mat4 Transform::toMat4() const {
        //TODO: (Req 3) Write this function
        
        // transMatrix is the translation matrix that is used to translate a specific object from one postion to another 
        glm::mat4 transMatrix=glm::translate(glm::mat4(1.0f),position);
        // scaleMatrix is the scaling matrix that is used to scale an object with a specific ratio 
        glm::mat4 scaleMatrix=glm::scale(glm::mat4(1.0f),scale);
        // rotMatrix is the rotation matrix that is used to rotates an object with a specific angle 
        glm::mat4 rotMatrix=glm::yawPitchRoll(rotation.y,rotation.x,rotation.z);
        // transformMatrix is a matrix that applies the whole transformation in one matrix (we get it by multiplying the 3 previous matricies)
        glm::mat4 transformMatrix=transMatrix*rotMatrix*scaleMatrix;
        return transformMatrix; 
    }

     // Deserializes the entity data and components from a json object
    void Transform::deserialize(const nlohmann::json& data){
        position = data.value("position", position);
        rotation = glm::radians(data.value("rotation", glm::degrees(rotation)));
        scale    = data.value("scale", scale);
    }

}