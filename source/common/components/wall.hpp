#pragma once

#include "../ecs/component.hpp"

#include <glm/glm.hpp>

namespace our {

    // This component denotes the walls in x-direction
    class wall : public Component {
    public:

        // The ID of this component type is "Movement"
        static std::string getID() { return "wall"; }

        // Reads linearVelocity & angularVelocity from the given json object
        void deserialize(const nlohmann::json& data) override;
    };

}