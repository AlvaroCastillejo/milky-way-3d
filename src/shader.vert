//CANVIAT
#version 330

//AFEGIT
in vec3 a_vertex;
in vec2 a_uv;
out vec2 v_uv;

//AFEGIT
in vec3 a_normal;
out vec3 v_normal;

uniform mat4 u_model;
uniform mat4 u_projection;
uniform mat4 u_view;

//PHONG SHADING (més coses al void main d'aquest shader i al shader1.frag)
//AFEGIT, specular (specular = material color * light color * (RdotE)shininess)
//AFEGIT, specular (per vertex)
//Declare a new varying to pass position to fragment shader
out vec3 v_pos;


void main()
{
	//AFEGIT
	v_uv = a_uv;
	v_normal = a_normal;

	// position of the vertex
	gl_Position = u_projection * u_view * u_model * vec4( a_vertex , 1.0 );

	//PHONG SHADING (ambient+diffuse+specular=phong reflection)
	//AFEGIT, specular (per vertex)
	//Save world space position (multiplying with the model matrix) into v_pos
	v_pos = (u_model*vec4(a_vertex,1.0)).xyz;
	gl_Position = u_projection*u_view*vec4(v_pos,1.0);
}


