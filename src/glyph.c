#include <swingby/glyph.h>

#include <stddef.h>
#include <stdlib.h>

#include <swingby/font.h>
#include <swingby/point.h>

#include "impl/glyph-impl.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct sb_glyph_run_t {
    sb_glyph_t *glyphs;
    uint32_t count;
    sb_font_t font;
};

struct sb_glyph_line_t {
    SbGlyphLineImpl *impl;
};

struct sb_glyph_layout_t {
    SbGlyphLayoutImpl *impl;
};

sb_glyph_run_t* sb_glyph_run_new(uint32_t count, const sb_font_t *font)
{
    sb_glyph_run_t *glyph_run = malloc(sizeof(sb_glyph_run_t));

    glyph_run->glyphs = malloc(sizeof(sb_glyph_t) * count);
    glyph_run->count = count;
    glyph_run->font = *font;

    return glyph_run;
}

uint32_t sb_glyph_run_count(const sb_glyph_run_t *glyph_run)
{
    return glyph_run->count;
}

sb_glyph_t* sb_glyph_run_glyphs(sb_glyph_run_t *glyph_run)
{
    return glyph_run->glyphs;
}

const sb_font_t* sb_glyph_run_font(const sb_glyph_run_t *glyph_run)
{
    return &glyph_run->font;
}

void sb_glyph_run_free(sb_glyph_run_t *glyph_run)
{
    free(glyph_run->glyphs);
    free(glyph_run);
}


sb_glyph_line_t* sb_glyph_line_new()
{
    sb_glyph_line_t *line = malloc(sizeof(sb_glyph_line_t));

    line->impl = sb_glyph_line_impl_new();

    return line;
}

void sb_glyph_line_add_run(sb_glyph_line_t *line, sb_glyph_run_t *run)
{
    sb_glyph_line_impl_add_run(line->impl, run);
}

uint32_t sb_glyph_line_run_count(const sb_glyph_line_t *line)
{
    return sb_glyph_line_impl_run_count(line->impl);
}

const sb_glyph_run_t** sb_glyph_line_runs(const sb_glyph_line_t *line)
{
    return sb_glyph_line_impl_runs(line->impl);
}

void sb_glyph_line_free(sb_glyph_line_t *line)
{
    sb_glyph_line_impl_free(line->impl);
    free(line);
}


sb_glyph_layout_t* sb_glyph_layout_new()
{
    sb_glyph_layout_t *layout = malloc(sizeof(sb_glyph_layout_t));

    layout->impl = sb_glyph_layout_impl_new();

    return layout;
}

void sb_glyph_layout_add_line(sb_glyph_layout_t *layout, sb_glyph_line_t *line)
{
    sb_glyph_layout_impl_add_line(layout->impl, line);
}

uint32_t sb_glyph_layout_line_count(const sb_glyph_layout_t *layout)
{
    return sb_glyph_layout_impl_line_count(layout->impl);
}

const sb_glyph_line_t** sb_glyph_layout_lines(const sb_glyph_layout_t *layout)
{
    return sb_glyph_layout_impl_lines(layout->impl);
}

void sb_glyph_layout_free(sb_glyph_layout_t *layout)
{
    sb_glyph_layout_impl_free(layout->impl);
    free(layout);
}

#ifdef __cplusplus
}
#endif // __cplusplus
