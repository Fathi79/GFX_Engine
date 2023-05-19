#version 330

#define DIRECTIONAL 0
#define POINT       1
#define SPOT        2

struct Light {
   //spot light has direction we position we angle kaman 
    int type;
	// this position is for point light
    vec3 position;
	//This is the direcrion of the light (This is forthe type directional light )
    vec3 direction;
    vec3 color;
    vec3 attenuation;
	// 2 angles inner we outer law mosh motakhaylha bos fi el slides , gowa el inner bykoon el do2 aqwa haga barah el outer bi 0 fi elnos bykon  mix
    vec2 cone_angles;
};

#define MAX_LIGHTS 8

uniform Light lights[MAX_LIGHTS];
uniform int light_count;

struct Sky {
    vec3 top, horizon, bottom;
};

uniform Sky sky;

vec3 compute_sky_light(vec3 normal){
// law el normal bybos lfo2 yb2a el mafrod yakhod do2 mn el top , we law el normal bybos ltaht el mafrod yakhod mn el bottom
    vec3 extreme = normal.y > 0 ? sky.top : sky.bottom;
    // sky.horizon -> el do2 el gay mn elgnab
    return mix(sky.horizon, extreme, normal.y * normal.y);//mix interpolatesbetween sky.horizon , extreme

}

struct Material {
//hena qarrarna nestakhdem el texture fi el light we ngeeb el qal2aa ganb el bahr
    sampler2D albedo;
    sampler2D specular;
    sampler2D roughness;
    // gives the effect of 3d -> misal : el hitat elmedalema fi elwesh
    sampler2D ambient_occlusion;
	// emmisive ma2naha en el object binafsoh bytala2 do2 zay el shasha masalan
    sampler2D emissive;
};

uniform Material material;

in Varyings {
    vec4 color;
    vec2 tex_coord;
    vec3 normal;
    vec3 view;
    vec3 world;
} fs_in;

out vec4 frag_color;



void main() {
    vec3 normal = normalize(fs_in.normal);
    vec3 view = normalize(fs_in.view);
	// actually because ana 2ayez eldo2 elly khareg men el material mosh el saket aleha
    //light_dir=-light.dir
	// dah 2ashan ana el mohasela hia hasel darb el ambient el constantant fi el makan fi el ambient beta2 el material aslan ba2a
	//ambient_light=material.ambient*light.ambient*1.0
	// el 2 dol ba2a mo2adlat ehfazhom khalas
	//diffuse=light.color*material .diffuse*lambert(normal,light_dir)
	//specular=light.color*material.specular*phong(reflected,view,material.shiness)
    vec3 ambient_light = compute_sky_light(normal);

    vec3 diffuse = texture(material.albedo, fs_in.tex_coord).rgb;
    vec3 specular = texture(material.specular, fs_in.tex_coord).rgb;
    float roughness = texture(material.roughness, fs_in.tex_coord).r;
    vec3 ambient = diffuse * texture(material.ambient_occlusion, fs_in.tex_coord).r;
    vec3 emissive = texture(material.emissive, fs_in.tex_coord).rgb;
	// clamp di bitemna2 el shininess mn enaha tekon bi 0 or infinity 2ashan el 2 haysabboly mashakil fa bitlimit el 0 bi qimah we kathalek el infinity
    float shininess = 2.0 / pow(clamp(roughness, 0.001, 0.999), 4.0) - 2.0;
    
    vec3 world_to_light_dir;
	// el emmissive bytdaf zay ma hoa mn gher ma a2mel fieh haga
	// law ana 2awez el haga el emmisive tenawar el ba2ee momken ahot gwaha point light kakhod2aa fia teb2aa kanaha emmissive object bgd
    vec3 color = emissive + ambient_light * ambient;

    for(int light_idx = 0; light_idx < min(MAX_LIGHTS, light_count); light_idx++){
        Light light = lights[light_idx];
        float attenuation = 1.0;
        if(light.type == DIRECTIONAL){
		// if directional light, el direction bykon 2aks
            world_to_light_dir = -light.direction;
        } else {
		// if it is point light this is the equation
            world_to_light_dir = light.position - fs_in.world;
            float d = length(world_to_light_dir);
			// leah ba2sim ala distance of light ? 2ashan ana 2aizo yed2af kol ma neb2ed
            world_to_light_dir /= d;
			// el attenuation dah haga hom ekhtra2oha keda 2ashan el point light bykon da2if gidan fa2alo badal ma hane2semm 2ala d squared nedy el user el ekhtiar wi hoa yekhtar el light attenuation ellly hoa awzoh ema ye2sem ala d squared aw ala d bas aw hata mai2semsh khales
			// fi halet eno haye2sem ala d squared haykhtar vector el light attenuation bi (1,0,0) , d-> (0,1,0) , mesh haye2sem -> (0,0,1)
            attenuation = 1.0 / dot(light.attenuation, vec3(d*d, d, 1.0));

            if(light.type == SPOT){
                float angle = acos(dot(light.direction, -world_to_light_dir));
				// smoothstep is a step like function but not sharp step
                attenuation *= smoothstep(light.cone_angles.y/*outer angle*/, light.cone_angles.x /*inner angle*/, angle/*el angle ellybytharak biha been el 2*/);
            }
        }

        vec3 computed_diffuse = light.color * diffuse * max(0.0, dot(normal, world_to_light_direction));

        vec3 reflected = reflect(-world_to_light_dir, normal);
        vec3 computed_specular = light.color * specular * pow(max(0.0, dot(reflected, view)), shininess);

        color += (computed_diffuse + computed_specular) * attenuation;

    }
    
    frag_color = vec4(color, 1.0);
    // frag_color = vec4(fs_in.normal, 1.0);
}