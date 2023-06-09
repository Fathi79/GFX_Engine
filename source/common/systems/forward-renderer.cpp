#include "forward-renderer.hpp"
#include "../mesh/mesh-utils.hpp"
#include "../texture/texture-utils.hpp"
#include "iostream"
#include <glm/gtx/euler_angles.hpp>
namespace our
{
    void ForwardRenderer::initialize(glm::ivec2 windowSize, const nlohmann::json &config)
    {
        // First, we store the window size for later use
        this->windowSize = windowSize;

        // Then we check if there is a sky texture in the configuration
        if (config.contains("sky"))
        {
            // First, we create a sphere which will be used to draw the sky
            this->skySphere = mesh_utils::sphere(glm::ivec2(16, 16));

            // We can draw the sky using the same shader used to draw textured objects
            ShaderProgram *skyShader = new ShaderProgram();
            skyShader->attach("assets/shaders/textured.vert", GL_VERTEX_SHADER);
            skyShader->attach("assets/shaders/textured.frag", GL_FRAGMENT_SHADER);
            skyShader->link();

            //TODO: (Req 10) Pick the correct pipeline state to draw the sky
            // Hints: the sky will be draw after the opaque objects so we would need depth testing but which depth funtion should we pick?
            // We will draw the sphere from the inside, so what options should we pick for the face culling.
            PipelineState skyPipelineState{};
            skyPipelineState.depthTesting.enabled = true;
            skyPipelineState.depthTesting.function = GL_LEQUAL;
            skyPipelineState.faceCulling.enabled = true;
            skyPipelineState.faceCulling.culledFace = GL_FRONT;
            // Load the sky texture (note that we don't need mipmaps since we want to avoid any unnecessary blurring while rendering the sky)
            std::string skyTextureFile = config.value<std::string>("sky", "");
            Texture2D *skyTexture = texture_utils::loadImage(skyTextureFile, false);

            // Setup a sampler for the sky
            Sampler *skySampler = new Sampler();
            skySampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_WRAP_S, GL_REPEAT);
            skySampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Combine all the aforementioned objects (except the mesh) into a material
            this->skyMaterial = new TexturedMaterial();
            this->skyMaterial->shader = skyShader;
            this->skyMaterial->texture = skyTexture;
            this->skyMaterial->sampler = skySampler;
            this->skyMaterial->pipelineState = skyPipelineState;
            this->skyMaterial->tint = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            this->skyMaterial->alphaThreshold = 1.0f;
            this->skyMaterial->transparent = false;
        }

        // Then we check if there is a postprocessing shader in the configuration
        if (config.contains("postprocess"))
        {
            // TODO: (Req 11) Create a framebuffer
            glGenFramebuffers(1, &postprocessFrameBuffer);

            // TODO: (Req 11) Create a color and a depth texture and attach them to the framebuffer
            //  Hints: The color format can be (Red, Green, Blue and Alpha components with 8 bits for each channel).
            //  The depth format can be (Depth component with 24 bits).
            colorTarget = new Texture2D();
            colorTarget->bind();
            int levels = (int)glm::floor(glm::log2((float)glm::max(windowSize.x, windowSize.y))) + 1;
            glTexStorage2D(GL_TEXTURE_2D, levels, GL_RGBA8, windowSize.x, windowSize.y);

            depthTarget = new Texture2D();
            depthTarget->bind();
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, windowSize.x, windowSize.y);

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postprocessFrameBuffer);

            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTarget->getOpenGLName(), 0);
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTarget->getOpenGLName(), 0);

            // TODO: (Req 11) Unbind the framebuffer just to be safe
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

            // Create a vertex array to use for drawing the texture
            glGenVertexArrays(1, &postProcessVertexArray);

            // Create a sampler to use for sampling the scene texture in the post processing shader
            Sampler *postprocessSampler = new Sampler();
            postprocessSampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            postprocessSampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Create the post processing shader
            ShaderProgram *postprocessShader = new ShaderProgram();
            postprocessShader->attach("assets/shaders/fullscreen.vert", GL_VERTEX_SHADER);
            postprocessShader->attach(config.value<std::string>("postprocess", ""), GL_FRAGMENT_SHADER);
            postprocessShader->link();



            Distorsion = texture_utils::loadImage(config.value<std::string>("PPtexture", "")); 


            // Create a post processing material
            postprocessMaterial = new TexturedMaterial();
            postprocessMaterial->shader = postprocessShader;
            postprocessMaterial->texture = colorTarget;
            postprocessMaterial->sampler = postprocessSampler;
            
            // The default options are fine but we don't need to interact with the depth buffer
            // so it is more performant to disable the depth mask
            postprocessMaterial->pipelineState.depthMask = false;
        }
    }

    void ForwardRenderer::destroy()
    {
        // Delete all objects related to the sky
        if (skyMaterial)
        {
            delete skySphere;
            delete skyMaterial->shader;
            delete skyMaterial->texture;
            delete skyMaterial->sampler;
            delete skyMaterial;
        }
        // Delete all objects related to post processing
        if(postprocessMaterial && dummy){
            glDeleteFramebuffers(1, &postprocessFrameBuffer);
            glDeleteVertexArrays(1, &postProcessVertexArray);
            delete colorTarget;
            delete depthTarget;
            delete postprocessMaterial->sampler;
            delete postprocessMaterial->shader;
            delete postprocessMaterial;
        }
    }

    void ForwardRenderer::render(World *world)
    {
        // First of all, we search for a camera and for all the mesh renderers
        CameraComponent *camera = nullptr;
        opaqueCommands.clear();
        transparentCommands.clear();
        lightSources.clear();
        for (auto entity : world->getEntities())
        {
            // If we hadn't found a camera yet, we look for a camera in this entity
            if (!camera)
                camera = entity->getComponent<CameraComponent>();
            // If this entity has a mesh renderer component
            if (auto meshRenderer = entity->getComponent<MeshRendererComponent>(); meshRenderer)
            {
                // We construct a command from it
                RenderCommand command;
                command.localToWorld = meshRenderer->getOwner()->getLocalToWorldMatrix();
                command.center = glm::vec3(command.localToWorld * glm::vec4(0, 0, 0, 1));
                command.mesh = meshRenderer->mesh;
                command.material = meshRenderer->material;
                // if it is transparent, we add it to the transparent commands list
                if (command.material->transparent)
                {
                    transparentCommands.push_back(command);
                }
                else
                {
                    // Otherwise, we add it to the opaque command list
                    opaqueCommands.push_back(command);
                }
            }
            // if light component store it
            if (auto lightComp = entity->getComponent<LightComponent>(); lightComp)
            {
                lightSources.push_back(lightComp);
            }
        }

        // If there is no camera, we return (we cannot render without a camera)
        if (camera == nullptr)
            return;

        //TODO: (Req 9) Modify the following line such that "cameraForward" contains a vector pointing the camera forward direction
        // HINT: See how you wrote the CameraComponent::getViewMatrix, it should help you solve this one
        
        // These lines of code define a camera position and orientation in a 3D scene . 
        // at first i get the owner of the camera
        // then getLocalToWorldMatrix 
        // then creates a 4D vector called "eye" with x, y, z, and w components set to my place in world multiplyed by (0.0f, 0.0f, 0.0f, 1.0f)->(). This represents the position of the camera in world space.
        // then creates another 4D vector called "center" with x, y, z, and w components set to my place in world multiplyed by (0.0f, 0.0f, -1.0f, 1.0f). This represents the point in world space that the camera is looking at.
        // then calculates a normalized vector called "cameraForward" by subtracting the "eye" vector from the "center" vector and then normalizing the result. This represents the direction that the camera is facing.

        auto owner = camera->getOwner();
        auto M = owner->getLocalToWorldMatrix();
        glm::vec3 eye = M * glm::vec4(0, 0, 0, 1);
        glm::vec3 center = M * glm::vec4(0, 0, -1, 1);
        glm::vec3 cameraForward = glm::normalize(center - eye);
        std::sort(transparentCommands.begin(), transparentCommands.end(), [cameraForward](const RenderCommand &first, const RenderCommand &second)
                  {
            //TODO: (Req 9) Finish this function
            // HINT: the following return should return true "first" should be drawn before "second". 
            //the dot product of two vectors is a scalar (a single number). represents the magnitude of the projection of one vector onto the other. 
            //In other words, it measures how much of one vector is pointing in the same direction as the other vector.
            //so by these i know which object i nearer to the camera
            if (glm::dot(cameraForward,first.center) > glm::dot(cameraForward,second.center)  )
            return true;
            else
            return false; });

        // TODO: (Req 9) Get the camera ViewProjection matrix and store it in VP
        glm::mat4 VP = camera->getProjectionMatrix(windowSize) * camera->getViewMatrix();
         //TODO: (Req 9) Set the OpenGL viewport using viewportStart and viewportSize
        //glViewport((lower left corner of the viewport rectangle),(width and height of the viewport))
        glViewport(0, 0, windowSize.x, windowSize.y);
        // TODO: (Req 9) Set the clear color to black and the clear depth to 1
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearDepth(1.0);
        //TODO: (Req 9) Set the color mask to true and the depth mask to true (to ensure the glClear will affect the framebuffer)
        // glColorMask controls which color channels (red, green, blue, and alpha) are enabled for writing. 
        glColorMask(true, true, true, true);
        glDepthMask(true);

        // If there is a postprocess material, bind the framebuffer
        if(postprocessMaterial && dummy){
            //TODO: (Req 11) bind the framebuffer
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postprocessFrameBuffer);
        }
        // TODO: (Req 9) Clear the color and depth buffers
        // If you have depth testing enabled you should also clear the depth buffer before each frame using GL_DEPTH_BUFFER_BIT; otherwise you're stuck with the depth values from last frame:
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // TODO: (Req 9) Draw all the opaque commands
        //  Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command
        glm::mat4 MVP_O;
        for (auto command : opaqueCommands)
        {
            // use MVP matrix to draw to object in its right place
             command.material->setup();
            MVP_O = VP * command.localToWorld;
            // if the material of the object is lighted
            if (auto light_material = dynamic_cast<LitMaterial *>(command.material); light_material)
            {
                    
                light_material->shader->set("VP", VP);
                light_material->shader->set("M", command.localToWorld);
                light_material->shader->set("eye", eye);
                light_material->shader->set("M_IT", glm::transpose(glm::inverse(command.localToWorld)));
                light_material->shader->set("light_count", (int)lightSources.size());
                
                for (int i = 0; i < (int)lightSources.size(); i++)
                {
                  if(lightSources[i]->lightType >=0){
                      // calculate position and direction of the light source based on the object
                    glm::vec3 position = lightSources[i]->getOwner()->getLocalToWorldMatrix()*glm::vec4(0,0,0,1);
                    glm::vec3 direction = lightSources[i]->getOwner()->getLocalToWorldMatrix()*glm::vec4(0,-1,0,0);
                    
                    light_material->shader->set("lights[" + std::to_string(i) + "].direction",direction);
                    light_material->shader->set("lights[" + std::to_string(i) + "].color",lightSources[i]->color);
                    light_material->shader->set("lights[" + std::to_string(i) + "].type", lightSources[i]->lightType);
                    light_material->shader->set("lights[" + std::to_string(i) + "].position", position); 
                    light_material->shader->set("lights[" + std::to_string(i) + "].diffuse", lightSources[i]->diffuse);
                    light_material->shader->set("lights[" + std::to_string(i) + "].specular", lightSources[i]->specular);
                    light_material->shader->set("lights[" + std::to_string(i) + "].attenuation", lightSources[i]->attenuation);
                    light_material->shader->set("lights[" + std::to_string(i) + "].cone_angles", lightSources[i]->cone_angles);
                    
                }}
            }
            else
            {
                command.material->shader->set("transform", MVP_O);
            }
                        
            command.mesh->draw();
        }

        // If there is a sky material, draw the sky
        if (this->skyMaterial)
        {
            //TODO: (Req 10) setup the sky material
            //this function sets up the sky attributes as a textured material 
            // it calls the setup function of the textured material
            this->skyMaterial->setup();
            //TODO: (Req 10) Get the camera position
            /*To determine the camera's position in world coordinates, 
            a homogeneous vector with components (0, 0, 0, 1) is constructed. 
            This vector is then multiplied by the transformation matrix M. 
            The resulting vector is a 4D vector, but the fourth component (w) is discarded to obtain a 3D position vector.
            The resulting vector gives the camera's position in world coordinates.*/
            //glm::vec3 eye = M * glm::vec4(0.0, 0.0, 0.0, 1.0);
            glm::vec3 cameraPosition = eye;
            //TODO: (Req 10) Create a model matrix for the sky such that it always follows the camera (sky sphere center = camera position)
            //skyModel is The matrix used to transform the vertices of the sky to make it appear as if it always follows the camera position , giving the illusion that it is infinitely far away. 

            our::Transform skyTransform;
            skyTransform.position = cameraPosition;
            glm::mat4 skyModel = skyTransform.toMat4();
            // TODO: (Req 10) We want the sky to be drawn behind everything (in NDC space, z=1)
            //  We can achieve the is by multiplying by an extra matrix after the projection but what values should we put in it?
            glm::mat4 alwaysBehindTransform = glm::mat4(
                //  Row1, Row2, Row3, Row4
                1.0f, 0.0f, 0.0f, 0.0f, // Column1
                0.0f, 1.0f, 0.0f, 0.0f, // Column2
                0.0f, 0.0f, 0.0f, 0.0f, // Column3
                0.0f, 0.0f, 1.0f, 1.0f  // Column4
            );
             //TODO: (Req 10) set the "transform" uniform
            // here we are setting the uniform transform by our mvp matrix (so we first multiply by the model matrix , then VP=P*V , so we get mvp = p*v*m , then we multiply by the matrix always behind the scene which forces the sky to always be behind anything having z=1)
            skyMaterial->shader->set("transform", alwaysBehindTransform * VP * skyModel);
             //TODO: (Req 10) draw the sky sphere
            // calling mesh draw to draw the sky
            skySphere->draw();
        }
        // TODO: (Req 9) Draw all the transparent commands
        //  Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command
        glm::mat4 MVP_T;
        for (auto command : transparentCommands)
        {
            command.material->setup();
            MVP_T = VP * command.localToWorld;
            command.material->shader->set("transform", MVP_T);
            command.mesh->draw();
        }

        // If there is a postprocess material and dummy flag is true, apply postprocessing effect
        if(postprocessMaterial && dummy){

            glActiveTexture(GL_TEXTURE1);
            Distorsion->bind();
            postprocessMaterial->sampler->bind(1);
            postprocessMaterial->shader->set("additional_sampler",1);


            //TODO: (Req 11) Return to the default framebuffer
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);       
            //TODO: (Req 11) Setup the postprocess material and draw the fullscreen triangle
            postprocessMaterial->setup();
            glBindVertexArray(postProcessVertexArray);
            glDrawArrays(GL_TRIANGLES,0,3);

            // if  there is a light material apply it
            if (lightMaterial)
            {
                lightMaterial->setup();
            }
        }
    }

}