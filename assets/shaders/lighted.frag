#version 330

#define MAX_LIGHTS 16

#define DIRECTIONAL 0
#define POINT 1
#define SPOT 2

// Struct to represent a light
struct Light {
    int type;           // Type of light (DIRECTIONAL, POINT, or SPOT)
    vec3 position;      // Position of the light
    vec3 direction;     // Direction of the light (used for spotlights)
    vec3 diffuse;       // Diffuse color of the light
    vec3 specular;      // Specular color of the light
    vec3 attenuation;   // Attenuation parameters (x*d^2 + y*d + z)
    vec2 cone_angles;   // Cone angles (x: inner angle, y: outer angle) for spotlights
};

// Array of lights
uniform Light lights[MAX_LIGHTS];
uniform int light_count;

// Struct to represent the sky
struct Sky {
    vec3 top, middle, bottom;
};

uniform Sky sky;

// Struct to represent the material
struct Material {
    sampler2D albedo;               // Albedo texture
    sampler2D specular;             // Specular texture
    sampler2D ambient_occlusion;    // Ambient occlusion texture
    sampler2D roughness;            // Roughness texture
    sampler2D emissive;             // Emissive texture
};

uniform Material material;

// Varying inputs from the vertex shader
in Varyings {
    vec4 color;         // Color of the fragment
    vec2 tex_coord;     // Texture coordinates of the fragment
    vec3 normal;        // Normal vector of the fragment
    vec3 view;          // View vector (direction from fragment to camera)
    vec3 world;         // World position of the fragment
} fs_in;

// Output color of the fragment
out vec4 frag_color;

void main(){
    vec3 view = normalize(fs_in.view);
    vec3 normal = normalize(fs_in.normal);

    // Retrieve material properties from textures
    vec3 material_diffuse = texture(material.albedo, fs_in.tex_coord).rgb;
    vec3 material_specular = texture(material.specular, fs_in.tex_coord).rgb;
    vec3 material_ambient = material_diffuse * texture(material.ambient_occlusion, fs_in.tex_coord).r;
    
    float material_roughness = texture(material.roughness, fs_in.tex_coord).r;
    float material_shininess = 2.0 / pow(clamp(material_roughness, 0.001, 0.999), 4.0) - 2.0;

    vec3 material_emissive = texture(material.emissive, fs_in.tex_coord).rgb;

    // Compute sky light based on the normal vector
    vec3 sky_light = (normal.y > 0) ?
        mix(sky.middle, sky.top, normal.y * normal.y) :
        mix(sky.middle, sky.bottom, normal.y * normal.y);

    // Set the initial fragment color as the sum of emissive and ambient components
    frag_color = vec4(material_emissive + material_ambient, 1.0);

    // Clamp the number of lights to the maximum allowed
    int clamped_light_count = min(MAX_LIGHTS, light_count);

    // Iterate through the lights
    for(int i = 0; i < clamped_light_count; i++){
        Light light = lights[i];

        vec3 direction_to_light = -light.direction;
        if(light.type != DIRECTIONAL){
            // For non-directional lights, compute the direction to the light from the fragment
            direction_to_light = normalize(light.position - fs_in.world);
        }
        
        // Compute the diffuse reflection
        vec3 diffuse = light.diffuse * material_diffuse * max(0, dot(normal, direction_to_light));
        
        // Compute the reflected direction for specular reflection
        vec3 reflected = reflect(-direction_to_light, normal);
        
        // Compute the specular reflection
        vec3 specular = light.specular * material_specular * pow(max(0, dot(view, reflected)), material_shininess);

        float attenuation = 1;
        if(light.type != DIRECTIONAL){
            // For non-directional lights, compute attenuation based on distance
            float d = distance(light.position, fs_in.world);
            attenuation /= dot(light.attenuation, vec3(d*d, d, 1));
            
            if(light.type == SPOT){
                // For spotlights, attenuate based on the cone angles
                float angle = acos(dot(-direction_to_light, light.direction));
                attenuation *= smoothstep(light.cone_angles.y, light.cone_angles.x, angle);
            }
        }

        // Accumulate the diffuse and specular components with attenuation
        frag_color.rgb += (diffuse + specular) * attenuation;
    }
}
