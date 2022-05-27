#version 330 core

in vec2 TexCoord;
out vec4 frag_color;

uniform sampler2D u_texture;


void main()
{
    frag_color = vec4(0.0, 0.0, 0.0, texture(u_texture, TexCoord).r);
}
