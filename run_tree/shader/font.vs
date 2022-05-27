#version 330 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tex_coord;

out vec2 TexCoord;

uniform mat4 u_projection;

void main()
{
    gl_Position = u_projection * vec4(pos, 0.0, 1.0);
    TexCoord = tex_coord;
}

