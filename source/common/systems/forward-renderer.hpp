#pragma once

#include "../ecs/world.hpp"
#include "../components/camera.hpp"
#include "../components/light.hpp"
#include "../components/mesh-renderer.hpp"
#include "../asset-loader.hpp"
#include "../material/material.hpp"

#include <glad/gl.h>
#include <vector>
#include <algorithm>

namespace our
{
    
    // The render command stores command that tells the renderer that it should draw
    // the given mesh at the given localToWorld matrix using the given material
    // The renderer will fill this struct using the mesh renderer components
    struct RenderCommand {
        glm::mat4 localToWorld;
        glm::vec3 center;
        Mesh* mesh;
        Material* material;
    };

    // A forward renderer is a renderer that draw the object final color directly to the framebuffer
    // In other words, the fragment shader in the material should output the color that we should see on the screen
    // This is different from more complex renderers that could draw intermediate data to a framebuffer before computing the final color
    // In this project, we only need to implement a forward renderer
    class ForwardRenderer {
        // These window size will be used on multiple occasions (setting the viewport, computing the aspect ratio, etc.)
        glm::ivec2 windowSize;
        // These are two vectors in which we will store the opaque and the transparent commands.
        // We define them here (instead of being local to the "render" function) as an optimization to prevent reallocating them every frame
        std::vector<RenderCommand> opaqueCommands;
        std::vector<RenderCommand> transparentCommands;
        // Objects used for rendering a skybox
        Mesh* skySphere;
        TexturedMaterial* skyMaterial;
        // Objects used for Postprocessing
        GLuint postprocessFrameBuffer, postProcessVertexArray;
        Texture2D *colorTarget, *depthTarget;
        TexturedMaterial* postprocessMaterial;
        // Objects used for distortion
        Texture2D* Distorsion;
        //Dummy variable to switch between postprocessing effects
        bool dummy=false;
        // Objects used to support lighting
        std::vector<LightComponent*> lightSources;
        LitMaterial* lightMaterial;
        glm::vec3 skyTop;
        glm::vec3 skyMiddle;
        glm::vec3 skyBottom;

        

    public:
        //Inverts between the postprocessing effects
        void invertDummyVariable(){dummy=!dummy;}
        // Initialize the renderer including the sky and the Postprocessing objects.
        // windowSize is the width & height of the window (in pixels).
        void initialize(glm::ivec2 windowSize, const nlohmann::json& config);
        // Clean up the renderer
        void destroy();
        // This function should be called every frame to draw the given world
        void render(World* world);
        /// read material sky from json
        void deserialize(const nlohmann::json &data) 
        {
            if (data.contains("skyTop"))
            {
            
               skyTop= glm::vec3 (data["skyTop"][0],data["skyTop"][1],data["skyTop"][2]  );
            }
            if (data.contains("skyMiddle"))
            {
               skyMiddle= glm::vec3 (data["skyMiddle"][0],data["skyMiddle"][1],data["skyMiddle"][2]  );
            }
            if (data.contains("skyBottom"))
            {
               skyBottom= glm::vec3 (data["skyBottom"][0],data["skyBottom"][1],data["skyBottom"][2]  );
            }

        }


    };

}