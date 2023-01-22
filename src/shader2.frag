//AFEGIT, FRAGMENT SHADER PARA SOLO PINTAR LA TEXTURA DEL ESPACIO

#version 330
out vec4 fragColor;
in vec3 v_color;
uniform vec3 u_color;
in vec2 v_uv;
uniform sampler2D u_texture;
in vec3 v_normal;
uniform vec3 u_light_dir;

void main(void)
{
	vec4 texture_color = texture(u_texture, v_uv);
	fragColor = vec4(texture_color.xyz, 1.0);
}