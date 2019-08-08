#version 330 core

in vec2 coords;
out vec4 result;

uniform sampler2D image;
uniform vec4 color;
uniform vec4 tone;
uniform vec4 flash;
uniform float alpha;
uniform float hue;

vec3 hue_shift(vec3 color, float hueAdjust) {

    const vec3 kRGBToYPrime = vec3(0.299, 0.587, 0.114);
    const vec3 kRGBToI = vec3(0.596, -0.275, -0.321);
    const vec3 kRGBToQ = vec3(0.212, -0.523, 0.311);
    const vec3 kYIQToR = vec3(1.0, 0.956, 0.621);
    const vec3 kYIQToG = vec3(1.0, -0.272, -0.647);
    const vec3 kYIQToB = vec3(1.0, -1.107, 1.704);

    float YPrime = dot(color, kRGBToYPrime);
    float I = dot(color, kRGBToI);
    float Q = dot(color, kRGBToQ);
    float h = atan(Q, I);
    float chroma = sqrt(I * I + Q * Q);

    h += radians(hue);
    Q = chroma * sin(h);
    I = chroma * cos(h);
    vec3 yIQ = vec3(YPrime, I, Q);

    return vec3(dot(yIQ, kYIQToR), dot(yIQ, kYIQToG), dot(yIQ, kYIQToB));
}

void main() {
    result = texture(image, coords);
    if (hue > 0.0) {
        result = vec4(hue_shift(result.rgb, hue), result.a);
    }
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
    if (alpha < 1.0) {
        result *= alpha;
    }
}