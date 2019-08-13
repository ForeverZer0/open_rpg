// Author: Fernando Kuteken
// License: MIT
// Hexagonal math from: http://www.redblobgames.com/grids/hexagons/

uniform int steps;                // = 50;
uniform float horizontalHexagons; // = 20;

vec3 roundHexagon(vec3 hex) {

    float q = floor(hex.x + 0.5);
    float r = floor(hex.y + 0.5);
    float s = floor(hex.z + 0.5);

    float deltaQ = abs(q - hex.x);
    float deltaR = abs(r - hex.y);
    float deltaS = abs(s - hex.z);

    if (deltaQ > deltaR && deltaQ > deltaS)
        q = -r - s;
    else if (deltaR > deltaS)
        r = -q - s;
    else
        s = -q - r;

    return vec3(q, r, -q - r);
}

vec3 hexagonFromPoint(vec2 point, float size) {

    point.y /= ratio;
    point = (point - 0.5) / size;

    float q = (sqrt(3.0) / 3.0) * point.x + (-1.0 / 3.0) * point.y;
    float r = 0.0 * point.x + 2.0 / 3.0 * point.y;

    vec3 hex = vec3(q, r, -q - r);
    return roundHexagon(hex);
}

vec2 pointFromHexagon(vec3 hex, float size) {

    float x = (sqrt(3.0) * hex.x + (sqrt(3.0) / 2.0) * hex.y) * size + 0.5;
    float y = (0.0 * hex.x + (3.0 / 2.0) * hex.y) * size + 0.5;

    return vec2(x, y * ratio);
}

vec4 transition(vec2 uv) {

    float dist = 2.0 * min(progress, 1.0 - progress);
    dist = steps > 0 ? ceil(dist * float(steps)) / float(steps) : dist;

    float size = (sqrt(3.0) / 3.0) * dist / horizontalHexagons;
    vec2 point = dist > 0.0 ? pointFromHexagon(hexagonFromPoint(uv, size), size) : uv;

    return mix(getFromColor(point), getToColor(point), progress);
}
