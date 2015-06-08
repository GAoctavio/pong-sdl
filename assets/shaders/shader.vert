#version 330 core
layout (location = 0) in vec3 position;

out vec3 FragPos;

uniform mat4 model;
/*uniform mat4 view;*/
uniform mat4 projection;
//uniform mat4 projXview;
void main()
{
    //gl_Position = projXview * model * vec4(position, 1.0f);
    gl_Position= projection*model * vec4(position,1.0f);//view is static
    FragPos = gl_Position.xyz;
    //TexCoords = texCoords;
    //FragPos = gl_Position.xyz;
    //Normal = mat3(transpose(inverse(model))) * normal;
}
