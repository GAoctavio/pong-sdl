#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoords;

out vec2 TexCoords;
uniform mat4 model;
out vec3 FragPos;
void main()
{
    gl_Position = model * vec4(position.x, position.y, -0.99f, 1.0f);
    TexCoords = texCoords;
    FragPos = gl_Position.xyz;
}
