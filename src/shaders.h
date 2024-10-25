#ifndef _FOUNDATION_SHADERS
#define _FOUNDATION_SHADERS

static const char *rect_vert_shader =
"#version 460 core\n"
"\n"
"layout (location = 0) in vec2 point;\n"
"layout (location = 1) in vec2 vTexCoord;\n"
"\n"
"uniform vec2 resolution;\n"
"\n"
"out vec2 texCoord;\n"
"\n"
"vec2 calculateCoord(vec2 point)\n"
"{\n"
"    float halfWidth = resolution.x / 2;\n"
"    float halfHeight = resolution.y / 2;\n"
"\n"
"    vec2 glPos = vec2(0.0, 0.0);\n"
"\n"
"    glPos.x = (point.x < halfWidth)\n"
"        ? -(1.0 + -(point.x / halfWidth))\n"
"        : -((halfWidth - point.x) / halfWidth);\n"
"    glPos.y = (point.y < halfHeight)\n"
"        ? (1.0 + -(point.y / halfHeight))\n"
"        : ((halfHeight - point.y) / halfHeight);\n"
"\n"
"    return glPos;\n"
"}\n"
"\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(calculateCoord(point), 0.0, 1.0);\n"
"    texCoord = vTexCoord;\n"
"}\n"
"\n"
;

static const char *texture_frag_shader =
"#version 460 core\n"
"\n"
"precision mediump float;\n"
"\n"
"uniform sampler2D textureIn;\n"
"\n"
"out vec4 fragColor;\n"
"\n"
"in vec2 texCoord;\n"
"\n"
"void main()\n"
"{\n"
"    fragColor = texture(textureIn, texCoord);\n"
"}\n"
"\n"
;

static const char *canvas_frag_shader =
"#version 460 core\n"
"\n"
"precision mediump float;\n"
"\n"
"uniform sampler2D textureIn;\n"
"\n"
"out vec4 fragColor;\n"
"\n"
"in vec2 texCoord;\n"
"\n"
"void main()\n"
"{\n"
"    fragColor = texture(textureIn, texCoord);\n"
"}\n"
"\n"
;

static const char *sample_vert_shader =
"#version 330 core\n"
"\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec2 aTexCoord;\n"
"\n"
"out vec2 texCoord;\n"
"\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(aPos, 1.0);\n"
"    texCoord = aTexCoord;\n"
"}\n"
"\n"
;

static const char *color_frag_shader =
"#version 460 core\n"
"\n"
"precision mediump float;\n"
"\n"
"uniform vec4 color;\n"
"\n"
"out vec4 fragColor;\n"
"\n"
"void main()\n"
"{\n"
"    fragColor = color;\n"
"}\n"
"\n"
;

static const char *sample_frag_shader =
"#version 330 core\n"
"\n"
"out vec4 fragColor;\n"
"\n"
"in vec2 texCoord;\n"
"\n"
"void main()\n"
"{\n"
"    fragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
"}\n"
"\n"
;

static const char *canvas_vert_shader =
"#version 460 core\n"
"\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec2 aTexCoord;\n"
"\n"
"out vec2 texCoord;\n"
"\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(aPos, 1.0);\n"
"    texCoord = aTexCoord;\n"
"}\n"
"\n"
;

#endif
