#version 330 core
in vec2 TexCoords;
//out vec4 color;

uniform sampler2D screenTexture;
in vec3 FragPos;
void main()
{
    gl_FragColor = texture(screenTexture, TexCoords);
}
