#ifndef _SWINGBY_GLYPH_H
#define _SWINGBY_GLYPH_H

#include <stdint.h>

#include <swingby/point.h>
#include <swingby/font.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct sb_glyph_t sb_glyph_t;

typedef struct sb_glyph_run_t sb_glyph_run_t;

typedef struct sb_glyph_line_t sb_glyph_line_t;

typedef struct sb_glyph_layout_t sb_glyph_layout_t;

struct sb_glyph_t {
    uint32_t id;
    float advance;
    sb_point_t offset;
};


sb_glyph_run_t* sb_glyph_run_new(uint32_t count, const sb_font_t *font);

uint32_t sb_glyph_run_count(const sb_glyph_run_t *glyph_run);

sb_glyph_t* sb_glyph_run_glyphs(sb_glyph_run_t *glyph_run);

const sb_font_t* sb_glyph_run_font(const sb_glyph_run_t *glyph_run);

void sb_glyph_run_free(sb_glyph_run_t *glyph_run);


sb_glyph_line_t* sb_glyph_line_new();

void sb_glyph_line_add_run(sb_glyph_line_t *line, sb_glyph_run_t *run);

uint32_t sb_glyph_line_run_count(const sb_glyph_line_t *line);

const sb_glyph_run_t** sb_glyph_line_runs(const sb_glyph_line_t *line);

void sb_glyph_line_free(sb_glyph_line_t *line);


sb_glyph_layout_t* sb_glyph_layout_new();

void sb_glyph_layout_add_line(sb_glyph_layout_t *layout, sb_glyph_line_t *line);

uint32_t sb_glyph_layout_line_count(const sb_glyph_layout_t *layout);

const sb_glyph_line_t** sb_glyph_layout_lines(const sb_glyph_layout_t *layout);

void sb_glyph_layout_free(sb_glyph_layout_t *layout);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _SWINGBY_GLYPH_H */
