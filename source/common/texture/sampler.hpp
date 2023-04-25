#pragma once

#include <glad/gl.h>
#include <json/json.hpp>
#include <glm/vec4.hpp>

namespace our {

    // This class defined an OpenGL sampler
    class Sampler {
        // The OpenGL object name of this sampler 
        GLuint name;
    public:
        // This constructor creates an OpenGL sampler and saves its object name in the member variable "name" 
        /**
         *   @brief Constructor function for the Sampler class.
         *   This constructor creates a new sampler object using the OpenGL graphics library
         *   and saves its object name in the member variable "name".
         *   @param None
         *   @return None
        */

        Sampler() {
            //TODO: (Req 6) Complete this function
            /*
            "glGenSamplers" function is used to generate a single new sampler object.
            The function takes two parameters: the number of sampler objects to generate (in this case, 1)
            , and a pointer to an integer variable where the generated object's name will be stored. 
            In this case, the variable being pointed to is the "name" member variable of the Sampler class.
            Briefly what is sampler ? a sampler is an object used to control how textures are sampled in OpenGL,
            and it plays an important role in determining how images are displayed on a screen.
            */
            glGenSamplers(1, &name);
        };

        // This deconstructor deletes the underlying OpenGL sampler
        /**
            @brief destructor function for the Sampler class.
            This destructor is responsible for deleting the underlying OpenGL sampler object
            @param None
            @return None
        */
        ~Sampler() { 
            //TODO: (Req 6) Complete this function'
            /*
            The glDeleteSamplers function frees any resources 
            associated with the specified sampler object(s) and deletes the object(s) 
            from the current OpenGL context.
            This is important to avoid memory leaks and to ensure that the OpenGL context is properly cleaned up when the program exits.
            The function takes two parameters: the number of sampler objects to be deleted (in this case, 1)
            , and a pointer to an integer variable where the object is stored. 
            In this case, the variable being pointed to is the "name" member variable of the Sampler class.
            */
            glDeleteSamplers(1, &name);
         }

        // This method binds this sampler to the given texture unit
        /**
         * Binds this sampler object to the specified texture unit in the OpenGL context.
         * 
         * @param textureUnit - the texture unit to which the sampler object should be bound
         * @return None
         */
        
        void bind(GLuint textureUnit) const {
            //TODO: (Req 6) Complete this function
            /*
            `glBindSampler` is an OpenGL function used to bind/unbind a sampler object to a texture unit in the current OpenGL context.
            The `glBindSampler` function takes two arguments:
            - `textureUnit`: This argument specifies the index of the texture unit to which the sampler object should be bound.
            Texture units are used to specify which texture or textures are to be used in a shader program.
            - `name`: This argument specifies the name or identifier of the sampler object that should be bound to the specified texture unit.
            */
            glBindSampler(textureUnit, name);
            
        }

        // This static method ensures that no sampler is bound to the given texture unit
        /**
         * unBinds the given texture unit from the given sampler object .
         * 
         * @param textureUnit - the texture unit to which the sampler object should be unbound
         * @return None
         */
        static void unbind(GLuint textureUnit){
            //TODO: (Req 6) Complete this function
            /*
            As specified above , `glBindSampler` is an OpenGL function used to bind a sampler object to a texture unit in the current OpenGL context.
            The `glBindSampler` function takes two arguments:
            - `textureUnit`: This argument specifies the index of the texture unit to which the sampler object should be bound.
            Texture units are used to specify which texture or textures are to be used in a shader program.
            - `name`: This argument specifies the name or identifier of the sampler object that should be bound to the specified texture unit,
            here 0 means no sampler will be bound to this texture .
            */
            glBindSampler(textureUnit, 0);
        }

        // This function sets a sampler paramter where the value is of type "GLint"
        // This can be used to set the filtering and wrapping parameters
        /**
         * Sets a sampler parameter where the value is of type GLint.
         * 
         * @param parameter - the parameter to be set (filtering or wrapping)
         * @param value - value used to set this parameter
         */
        void set(GLenum parameter, GLint value) const {
            //TODO: (Req 6) Complete this function
            /*
            `glSamplerParameteri`is used to set a single integer parameter for a sampler object. The function takes three arguments:
            `name`: the name of the sampler object to set the parameter for.
            `parameter`: this specifies which sampler parameter to set i.e : GL_TEXTURE_WRAP_S for wrapping or GL_TEXTURE_MAG_FILTER for filtering.
            `value`: an integer value that represents the new value for the specified sampler parameter(wrapping mode , filter type) i.e: GL_REPEAT for wrapping or GL_LINEAR for filtering .
            */
            glSamplerParameteri(name, parameter, value);
        }

        // This function sets a sampler paramter where the value is of type "GLfloat"
        // This can be used to set the "GL_TEXTURE_MAX_ANISOTROPY_EXT" parameter
        /**
         * Sets a sampler parameter where the value is of type GLfloat.
         * 
         * @param parameter - the parameter to be set (GL_TEXTURE_MAX_ANISOTROPY_EXT here)
         * @param value - value used to set this parameter
         */
        void set(GLenum parameter, GLfloat value) const {
            //TODO: (Req 6) Complete this function
            /*
            Anisotropic filtering is a technique used in computer graphics to improve the visual quality 
            of textures when they are viewed at oblique angles. It is used to reduce the distortion and 
            blurring that can occur when textures are viewed at a sharp angle, which can result in a loss
            of detail and a "smearing" effect.In traditional texture filtering, each texel (texture element)
            is sampled at the same rate, regardless of its orientation relative to the camera. This can 
            result in distortion and blurring for textures that are viewed at an angle. Anisotropic 
            filtering solves this problem by sampling the texture at different rates in different directions,
            based on the angle of the texture relative to the camera. This allows for sharper and more detailed
            textures, even when viewed at oblique angles.Anisotropic filtering is especially useful for textures
            that contain fine detail, such as grass, foliage, or pavement.
            */
            glSamplerParameterf(name, parameter, value);
        }

        // This function sets a sampler paramter where the value is of type "GLfloat[4]"
        // This can be used to set the "GL_TEXTURE_BORDER_COLOR" parameter
        void set(GLenum parameter, glm::vec4 value) const {
            glSamplerParameterfv(name, parameter, &(value.r));
        }

        // Given a json object, this function deserializes the sampler state
        void deserialize(const nlohmann::json& data);

        Sampler(const Sampler&) = delete;
        Sampler& operator=(const Sampler&) = delete;
    };

}