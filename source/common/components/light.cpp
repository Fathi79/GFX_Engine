#include "light.hpp"                      
#include "../deserialize-utils.hpp"        
#include "./component-deserializer.hpp"     
#include <glm/gtx/euler_angles.hpp>       
#include <iostream>                        

namespace our
{    
    // Reads light data from json file
    void LightComponent::deserialize(const nlohmann::json &data)
    {
        if (!data.is_object())
            return;

        lightTypeStr = data.value("typeOfLight", "DIRECTIONAL");   // Get the string value for the light type (default: DIRECTIONAL)
        direction = glm::vec3(data.value("direction", glm::vec3(1, 1, 1)));   // Get the direction vector of the light (default: (1, 1, 1))

        if (lightTypeStr == "DIRECTIONAL")
            lightType = 0;    // Set the light type to DIRECTIONAL (0)

        else if (lightTypeStr == "POINT")
            lightType = 1;    // Set the light type to POINT (1)

        else if (lightTypeStr == "SPOT")
            lightType = 2;    // Set the light type to SPOT (2)

        diffuse = glm::vec3(data.value("diffuse", glm::vec3(1, 1, 1)));    // Get the diffuse color of the light (default: (1, 1, 1))
        specular = glm::vec3(data.value("specular", glm::vec3(1, 1, 1)));  // Get the specular color of the light (default: (1, 1, 1))

        if (lightType != 0)   // If the light type is not DIRECTIONAL
        {
            attenuation = glm::vec3(data.value("attenuation", glm::vec3(1, 0, 0)));   // Get the attenuation parameters of the light (default: (1, 0, 0))
        }

        if (lightType == 2)   // If the light type is SPOT
        {  
            cone_angles.x = glm::radians((float)data.value("cone_angles.in", 10));   // Get the inner cone angle of the spotlight in radians (default: 10)
            cone_angles.y = glm::radians((float)data.value("cone_angles.out", 80));  // Get the outer cone angle of the spotlight in radians (default: 80)
        }
    }
    
}
