#version 330 core

in vec2 coords;
out vec4 result;
uniform sampler2D text;
uniform vec4 color;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, coords).r);
    result = vec4(color) * sampled;
}  