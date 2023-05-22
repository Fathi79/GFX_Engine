#pragma once

#include "../ecs/component.hpp"

#include <glm/glm.hpp> 

namespace our {

    class ScareCrowControllerComponent : public Component {
    public:
        // The ID of this component type is "Free Camera Controller"
        static std::string getID() { return "Scare Crow Controller"; }

        // Reads sensitivities & speedupFactor from the given json object
        void deserialize(const nlohmann::json& data) override;
    };

}