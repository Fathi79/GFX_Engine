#pragma once

#include <glad/gl.h>
#include "vertex.hpp"
#include <string>

namespace our {

    #define ATTRIB_LOC_POSITION 0
    #define ATTRIB_LOC_COLOR    1
    #define ATTRIB_LOC_TEXCOORD 2
    #define ATTRIB_LOC_NORMAL   3

    class Mesh {
        // Here, we store the object names of the 3 main components of a mesh:
        // A vertex array object, A vertex buffer and an element buffer
        unsigned int VBO;/// Vertix buffer object
         unsigned int EBO; /// Element buffer object
        unsigned int VAO; ///vertex array object
        // We need to remember the number of elements that will be draw by glDrawElements 
        GLsizei elementCount;
    public:

        // save the max and min values for the vertices in order to use them to calculate the center of 
        // the character or the bounding box
        float minX, maxX;
        float minY, maxY;
        float minZ, maxZ;
        std::string name;
        // The constructor takes two vectors:
        // - vertices which contain the vertex data.
        // - elements which contain the indices of the vertices out of which each rectangle will be constructed.
        // The mesh class does not keep a these data on the RAM. Instead, it should create
        // a vertex buffer to store the vertex data on the VRAM,
        // an element buffer to store the element data on the VRAM,
        // a vertex array object to define how to read the vertex & element buffer during rendering 
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& elements)
        {
            // calculate the values of the mesh boundries
            calculateMinMaxPoints(vertices);
            //TODO: (Req 1) Write this function
            
    
            elementCount = (GLsizei)elements.size(); //elements size 
            //Vertex array generate
              glGenVertexArrays(1, &VAO);
              //Vertex array generate bind
              glBindVertexArray(VAO);
             //vertex buffer generate 
               glGenBuffers(1, &VBO);
                //vertex buffer generate bind
               glBindBuffer(GL_ARRAY_BUFFER, VBO);
               ///reates a new data store for the buffer object currently bound to target
                       
              glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

              //position location
               glEnableVertexAttribArray(ATTRIB_LOC_POSITION);
               glVertexAttribPointer(ATTRIB_LOC_POSITION, 3, GL_FLOAT, false, sizeof(Vertex), (void*)0);
                //color location
               glEnableVertexAttribArray(ATTRIB_LOC_COLOR);
              glVertexAttribPointer(ATTRIB_LOC_COLOR, 4, GL_UNSIGNED_BYTE, true, sizeof(Vertex), (void*)offsetof(Vertex,color));

               //Texture location
               glEnableVertexAttribArray(ATTRIB_LOC_TEXCOORD);	
               glVertexAttribPointer(ATTRIB_LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));
               //Normal Location
                glEnableVertexAttribArray(ATTRIB_LOC_NORMAL);	
                glVertexAttribPointer(ATTRIB_LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
                // Element buffer generate and bind
               glGenBuffers(1, &EBO);
               glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
               glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementCount*sizeof(unsigned int), &elements[0], GL_STATIC_DRAW);
               glBindVertexArray(0); 

            // remember to store the number of elements in "elementCount" since you will need it for drawing
            // For the attribute locations, use the constants defined above: ATTRIB_LOC_POSITION, ATTRIB_LOC_COLOR, etc
            
        }

        /**
         * This function calculates the min and max values x, y, z for the mesh 
         * in order to use it in different aspects like boundry box, calculate the center, etc..
         * @param vertices The vertices array
         * @return void
        */
        void calculateMinMaxPoints(const std::vector<Vertex>& vertices)
        {
            minX = FLT_MAX;
            maxX = FLT_MIN;
            minY = FLT_MAX;
            maxY = FLT_MIN;
            minZ = FLT_MAX;
            maxZ = FLT_MIN;
            for(auto vertex : vertices)
            {
                glm::vec3 pos = vertex.position;
                if(pos[0] > maxX)
                maxX = pos[0];
                else if(pos[0] < minX)
                minX = pos[0];

                if(pos[1] > maxY)
                maxY = pos[1];
                else if(pos[1] < minY)
                minY = pos[1];

                if(pos[2] > maxZ)
                maxZ = pos[2];
                else if(pos[2] < minZ)
                minZ = pos[2];
            }
        }

        // this function should render the mesh
        void draw() 
        {
           
            //TODO: (Req 1) Write this function
          
             ///bind vertex array       
            glBindVertexArray(VAO); 
            ///draw elements in screen 
           glDrawElements(GL_TRIANGLES,  elementCount, GL_UNSIGNED_INT, (void*)0);
              
        }

        // this function should delete the vertex & element buffers and the vertex array object
        ~Mesh(){
            //TODO: (Req 1) Write this function
            glDeleteVertexArrays(1, &VAO); ///delete VAO
            glDeleteBuffers(1, &VBO); ///delete VBO
            glDeleteBuffers(1, &EBO); ///delete EBO
        }

        Mesh(Mesh const &) = delete;
        Mesh &operator=(Mesh const &) = delete;
    };

}