#version 460 core

precision mediump float;

uniform sampler2D textureIn;

out vec4 fragColor;

in vec2 texCoord;

void main()
{
    fragColor = texture(textureIn, texCoord);
}
