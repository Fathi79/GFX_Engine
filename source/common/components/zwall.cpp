#include "zwall.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"

namespace our {

    void zwall::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;
       
    }
}