#version 330

uniform vec3 eye;    // Eye position (camera position)
uniform mat4 VP;     // View-projection matrix
uniform mat4 M;      // Model matrix
uniform mat4 M_IT;   // Inverse-transpose of the model matrix

layout(location=0) in vec3 position;      // Vertex position input
layout(location=1) in vec4 color;         // Vertex color input
layout(location=2) in vec2 tex_coord;     // Texture coordinate input
layout(location=3) in vec3 normal;        // Vertex normal input

out Varyings {
    vec4 color;         // Output color to fragment shader
    vec2 tex_coord;     // Output texture coordinate to fragment shader
    vec3 normal;        // Output normal vector to fragment shader
    vec3 view;          // Output view vector to fragment shader (direction from vertex to eye)
    vec3 world;         // Output world position of the vertex
} vs_out;

void main(){
    vec3 world = (M * vec4(position, 1.0)).xyz;     // Transform vertex position to world space
    gl_Position = VP * vec4(world, 1.0);            // Transform vertex position to clip space
    vs_out.color = color;                           // Pass color to fragment shader
    vs_out.tex_coord = tex_coord;                   // Pass texture coordinate to fragment shader
    vs_out.normal = normalize((M_IT * vec4(normal, 0.0)).xyz);   // Transform and normalize vertex normal to world space
    vs_out.view = eye - world;                      // Compute view vector (direction from vertex to eye)
    vs_out.world = world;                           // Pass world position of the vertex to fragment shader
}
