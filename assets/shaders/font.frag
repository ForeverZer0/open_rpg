#version 330 core

in vec2 coords;
out vec4 result;
uniform sampler2D text;
uniform vec4 color;

uniform float thickness = .09;
uniform vec4 outline = vec4(0, 0, 0, 0.5);
uniform float threshold = 0.5;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, coords).r);
    result = vec4(color) * sampled;

    // result = vec4(1.0, 1.0, 1.0, texture(text, coords).r) * color;

    // if (result.a <= threshold && result.a > 0.0) {
    //     ivec2 size = textureSize(text, 0);

    //     float uv_x = coords.x * size.x;
    //     float uv_y = coords.y * size.y;

    //     float sum = 0.0;
    //     for (int n = 0; n < 9; ++n) {
    //         uv_y = (coords.y * size.y) + (thickness * float(n - 4.5));
    //         sum += texelFetch(text, ivec2(uv_x - (4.0 * thickness), uv_y), 0).a;
    //         sum += texelFetch(text, ivec2(uv_x - (3.0 * thickness), uv_y), 0).a;
    //         sum += texelFetch(text, ivec2(uv_x - (2.0 * thickness), uv_y), 0).a;
    //         sum += texelFetch(text, ivec2(uv_x - thickness, uv_y), 0).a;
    //         sum += texelFetch(text, ivec2(uv_x, uv_y), 0).a;
    //         sum += texelFetch(text, ivec2(uv_x + thickness, uv_y), 0).a;
    //         sum += texelFetch(text, ivec2(uv_x + (2.0 * thickness), uv_y), 0).a;
    //         sum += texelFetch(text, ivec2(uv_x + (3.0 * thickness), uv_y), 0).a;
    //         sum += texelFetch(text, ivec2(uv_x + (4.0 * thickness), uv_y), 0).a;
    //     }
    //     sum /= 9.0;
    //     if (sum > 0.1) {
    //         result = outline;
    //     }
    // }
}  