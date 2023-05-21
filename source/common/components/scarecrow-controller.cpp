#include "scarecrow-controller.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"

namespace our {
    void ScareCrowControllerComponent::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;

    }
}