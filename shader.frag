#version 330 core
// This is a sample fragment shader.

//Original
struct Light{
    int light_mode;
    vec3 light_color;
    vec3 light_pos;
    vec3 light_dir;
    float cons_att;
    float linear_att;
    float quad_att;
    float cutoff_angle;
    float exponent;
};

struct Material{
    int object_mode;
    vec3 color_diff;
    vec3 color_spec;
    vec3 color_ambi;
};

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
//in float sampleExtraOutput;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform Light light;
uniform Material material;
uniform int rendermode;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 color;
vec3 doLight(Light light, vec3 normal, vec3 viewDir, vec3 fragPos);

void main()
{
    // Color everything a hot pink color. An alpha of 1.0f means it is not transparent.
    //color = vec4(1.0f, 0.41f, 0.7f, sampleExtraOutput);
	vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
	vec3 result = doLight(light, norm, viewDir ,FragPos);
	if (rendermode == 0) {
	    color = vec4(result, 1.0);
	}
	else if (rendermode == 1) {
	    color = vec4(Normal, 1.0);
    }
}

vec3 doLight(Light light, vec3 normal, vec3 viewDir, vec3 fragPos){
    vec3 lightDir;
	vec3 reflectDir;
	vec3 toreturn;
	float att = 1.0;
	float k = 1.0;
	float k2 = 0.01;
	if (light.light_mode == 1) {
	    lightDir = normalize(-light.light_dir);
	}
	else if (light.light_mode == 2){
	    lightDir = normalize(light.light_pos - fragPos);
		att = k*length(light.light_pos - fragPos);
	}
	else if(light.light_mode == 3) {
	    lightDir = normalize(light.light_pos - fragPos);
		att = k2*pow(length(light.light_pos - fragPos), 2);
		if(-dot(lightDir, light.light_dir) <= light.cutoff_angle){
			light.light_color = vec3(0, 0, 0);
		}
		else{
		    light.light_color = light.light_color * pow(-dot(lightDir, light.light_dir), light.exponent);
		}
	}

	reflectDir = reflect(-lightDir, normal);
	float diff = dot(normal, lightDir);
	float spec = pow(dot(reflectDir, viewDir), 16);
	vec3 diffuse = (1/att)*light.light_color * material.color_diff*diff;
	vec3 specular = (1/att)*light.light_color * material.color_spec*spec;
	vec3 ambient = light.light_color * material.color_ambi;
    
	if (material.object_mode == 1){
	    toreturn = specular + ambient;
	}
	else if (material.object_mode == 2){
	    toreturn = diffuse + ambient;
	}
	else if (material.object_mode == 3){
	    toreturn = diffuse + specular + ambient;
	}

	return toreturn;
}
