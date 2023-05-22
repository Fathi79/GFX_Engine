#pragma once

#include "../ecs/component.hpp"     
#include "../ecs/transform.hpp"      
#include <unordered_map>            
#include <string>                   
#include <type_traits>              
#include <glm/glm.hpp>               

namespace our {
    
    enum class LightType {
        DIRECTIONAL,    // Enumeration for directional light type
        SPOT,           // Enumeration for spotlight type
        POINT           // Enumeration for point light type
    };
    
    // LightComponent class derived from Component base class
    class LightComponent : public Component {
    public:
        int lightType;              // Type of light (represented as an integer)
        glm::vec3 diffuse;          // Diffuse color of the light
        glm::vec3 direction;        // Direction of the light (for spotlights and directional lights)
        glm::vec3 specular;         // Specular color of the light
        glm::vec4 color;            // Color of the light
        glm::vec3 attenuation;      // Attenuation parameters for the light
        glm::vec2 cone_angles;      // Cone angles for spotlights
        std::string lightTypeStr;   // String representation of the light type
        // Reads light data from json file
        void deserialize(const nlohmann::json& data) override;
        
        static std::string getID() { return "light"; }    // Get the unique identifier for the light component
    };

}
