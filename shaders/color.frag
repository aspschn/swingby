#version 460 core

precision mediump float;

uniform vec4 color;

out vec4 fragColor;

void main()
{
    fragColor = color;
}
