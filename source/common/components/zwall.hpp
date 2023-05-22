#pragma once

#include "../ecs/component.hpp"

#include <glm/glm.hpp>

namespace our {

    // This component denotes the walls in the z-direction
    class zwall : public Component {
    public:

        // The ID of this component type is "zwall"
        static std::string getID() { return "zwall"; }

        // Reads linearVelocity & angularVelocity from the given json object
        void deserialize(const nlohmann::json& data) override;
    };

}