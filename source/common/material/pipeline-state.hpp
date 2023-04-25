#pragma once

#include <glad/gl.h>
#include <glm/vec4.hpp>
#include <json/json.hpp>

namespace our {
    // There are some options in the render pipeline that we cannot control via shaders
    // such as blending, depth testing and so on
    // Since each material could require different options (e.g. transparent materials usually use blending),
    // we will encapsulate all these options into a single structure that will also be responsible for configuring OpenGL's pipeline
    struct PipelineState {
        // This set of pipeline options specifies whether face culling will be used or not and how it will be configured
        struct {
            bool enabled = false;
            GLenum culledFace = GL_BACK;
            GLenum frontFace = GL_CCW;
        } faceCulling;

        // This set of pipeline options specifies whether depth testing will be used or not and how it will be configured
        struct {
            bool enabled = false;
            GLenum function = GL_LEQUAL;
        } depthTesting;

        // This set of pipeline options specifies whether blending will be used or not and how it will be configured
        struct {
            bool enabled = false;
            GLenum equation = GL_FUNC_ADD;
            GLenum sourceFactor = GL_SRC_ALPHA;
            GLenum destinationFactor = GL_ONE_MINUS_SRC_ALPHA;
            glm::vec4 constantColor = {0, 0, 0, 0};
        } blending;

        // These options specify the color and depth mask which can be used to
        // prevent the rendering/clearing from modifying certain channels of certain targets in the framebuffer
        glm::bvec4 colorMask = {true, true, true, true}; // To know how to use it, check glColorMask
        bool depthMask = true; // To know how to use it, check glDepthMask


        // This function should set the OpenGL options to the values specified by this structure
        // For example, if faceCulling.enabled is true, you should call glEnable(GL_CULL_FACE), otherwise, you should call glDisable(GL_CULL_FACE)
        void setup() const {
            //TODO: (Req 4) Write this function
            //Specifing whether the individual color components in the frame buffer can be written. It affects all thedrawin buffers
            glColorMask(colorMask.r, colorMask.g, colorMask.b, colorMask.a);
            //Controls writing access in the depth buffer
            glDepthMask(depthMask);
            //Here we are setting the faceculling options using the values in the struct instance 'faceCulling'
            if(faceCulling.enabled)
            {
                glEnable(GL_CULL_FACE);
                glCullFace(faceCulling.culledFace);
                glFrontFace(faceCulling.frontFace);
            }
            else glDisable(GL_CULL_FACE);

            //Here we are setting the blending options using the values in the struct instance 'blending'
            if(blending.enabled)    //Check if we want to enable blending options
            {
                glEnable(GL_BLEND);
                glBlendEquation(blending.equation);     //Setting the blending equation to be used
                glBlendFunc(blending.sourceFactor, blending.destinationFactor);     //Setting the source and destination factors. exampleEq: srcColor*srcFactor + destColor*destFactor
                // srcColor:  color output of the fragment shader
                // destColor: color vector currently stored in the color buffer
                // srcFactor: sets the impact of the alpha value on the source color
                // destFactor: sets the impact of the alpha value on the destination color
                //Note: These factors are meaningless if the blending equation is GL_MIN or GL_MAX
                glBlendColor(blending.constantColor.r, blending.constantColor.g, blending.constantColor.b, blending.constantColor.a);   //Setting the blendColor
            }
            else glDisable(GL_BLEND);

            //Here we are setting the depth testing options using the values in the struct instance 'depthTesting'
            if(depthTesting.enabled)
            {
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(depthTesting.function);    //Setting the comparator operator used by openGL. To control when OpenGL should pass or discard fragments and when to update depth buffer
            }
            else glDisable(GL_DEPTH_TEST);
        }

        // Given a json object, this function deserializes a PipelineState structure
        void deserialize(const nlohmann::json& data);
    };

}