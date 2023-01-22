//FRAGMENT SHADER PARA PINTAR OBJETOS, ILUMINACIÓN, ETC.

#version 330

out vec4 frag_color;
in vec3 v_color;
uniform vec3 u_color;

//AFEGIT
in vec2 v_uv;

//AFEGIT
uniform sampler2D u_texture;

//AFEGIT
in vec3 v_normal;
uniform vec3 u_light_dir;

//PHONG SHADING (més coses al void main d'aquest shader i al shader.vert)
//AFEGIT, specular (specular = material color * light color * (RdotE)shininess)
//AFEGIT, specular (per fragment)
//Declare a new uniforms and upload data to fill them
uniform vec3 u_light_color;	//a rgb color
uniform vec3 u_eye;	//where the camera is placed
uniform float u_glossiness;	//a pow factor, try 50
//AFEGIT, variables que faltava declarar perquè funcionés el Phong Shading
in vec3 v_pos;
uniform vec3 u_ambient;

void main(void)
{
	//AFEGIT
	//vec4 texture_color = texture(u_texture, v_uv);

	//AFEGIT
	vec3 N = normalize(v_normal);
	vec3 L = normalize(u_light_dir);
	//float NdotL = max(dot(N,L),0.0);
	//vec3 final_color = texture_color.xyz*NdotL;
	//fragColor = vec4(final_color, 1.0);

	//PHONG SHADING (ambient+diffuse+specular=phong reflection)
	//AFEGIT, ambient (ambient = material color * ambient light)
	vec3 texture_color = texture(u_texture, v_uv).xyz;
	vec3 ambient_color = texture_color * u_ambient;	//u_ambient is a float factor, try with 0.1f.
	//AFEGIT, diffuse (diffuse = material color * NdotL (* light colour))
	float NdotL = max( dot(N,L) , 0.0);
	vec3 diffuse_color = texture_color * NdotL;
	//AFEGIT, specular (per fragment)
	//Calculate specular factor
	vec3 R = reflect(L,N);
	vec3 E = normalize(u_eye-v_pos);
	float RdotE = max(0.0,dot(R,E));
	vec3 specular_color=u_light_color*pow(RdotE, u_glossiness);
	//AFEGIT, ajuntar-ho tot
	frag_color = vec4(ambient_color+diffuse_color+specular_color,1.0);
}
