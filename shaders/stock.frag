#version 330 core

in vec2 coords;
out vec4 result;

uniform sampler2D image;
uniform vec4 color;
uniform vec4 tone;
uniform vec4 flash;
uniform float alpha;

uniform vec4 screen = vec4(0.0);
uniform int screen_z = 5000;
uniform int z;

void main()
{
    result = texture(image, coords);
    if (color.a > 0.0) {
        result = vec4(mix(result.rgb, color.rgb, color.a), result.a);
    }
    if (tone.a > 0.0) {
        float avg = (result.r + result.g + result.b) / 3.0;
        result.r = result.r - ((result.r - avg) * tone.a);
        result.g = result.g - ((result.g - avg) * tone.a);
        result.b = result.b - ((result.b - avg) * tone.a);
    }
    result = vec4(clamp(result.rgb + tone.rgb, 0.0, 1.0), result.a);
    
    if (flash.a > 0.0) {
        result = vec4(mix(result.rgb, flash.rgb, flash.a), result.a);
    }
    if (z < screen_z) {
        if (screen.a > 0.0) {
            float avg = (result.r + result.g + result.b) / 3.0;
            result.r = result.r - ((result.r - avg) * screen.a);
            result.g = result.g - ((result.g - avg) * screen.a);
            result.b = result.b - ((result.b - avg) * screen.a);
        }
        result = vec4(clamp(result.rgb + screen.rgb, 0.0, 1.0), result.a);
    }
    if (alpha < 1.0) {
        result *= alpha;
    }
}