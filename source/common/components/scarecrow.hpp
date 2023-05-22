#pragma once

#include "../ecs/component.hpp"

#include <glm/glm.hpp>

namespace our {

    // This is a scarecrow component
    class scarecrow : public Component {
    public:

        // The ID of this component type is "Movement"
        static std::string getID() { return "scarecrow"; }

        // Reads linearVelocity & angularVelocity from the given json object
        void deserialize(const nlohmann::json& data) override;
    };

}