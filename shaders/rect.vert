#version 460 core

layout (location = 0) in vec2 point;
layout (location = 1) in vec2 vTexCoord;

uniform vec2 resolution;

out vec2 texCoord;

vec2 calculateCoord(vec2 point)
{
    float halfWidth = resolution.x / 2;
    float halfHeight = resolution.y / 2;

    vec2 glPos = vec2(0.0, 0.0);

    glPos.x = (point.x < halfWidth)
        ? -(1.0 + -(point.x / halfWidth))
        : -((halfWidth - point.x) / halfWidth);
    glPos.y = (point.y < halfHeight)
        ? (1.0 + -(point.y / halfHeight))
        : ((halfHeight - point.y) / halfHeight);

    return glPos;
}

void main()
{
    gl_Position = vec4(calculateCoord(point), 0.0, 1.0);
    texCoord = vTexCoord;
}
