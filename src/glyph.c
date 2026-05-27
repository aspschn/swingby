#include <swingby/glyph.h>

#include <stddef.h>
#include <stdlib.h>

#include <swingby/font.h>
#include <swingby/point.h>
#include <swingby/list.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct sb_glyph_run_t {
    sb_glyph_t *glyphs;
    uint32_t count;
    sb_font_t font;
};

struct sb_glyph_line_t {
    sb_list_t *runs;
};

struct sb_glyph_layout_t {
    sb_list_t *lines;
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

    line->runs = sb_list_new();

    return line;
}

void sb_glyph_line_add_run(sb_glyph_line_t *line, sb_glyph_run_t *run)
{
    sb_list_push(line->runs, run);
}

uint32_t sb_glyph_line_run_count(const sb_glyph_line_t *line)
{
    return sb_list_length(line->runs);
}

const sb_list_t* sb_glyph_line_runs(const sb_glyph_line_t *line)
{
    return line->runs;
}

void sb_glyph_line_free(sb_glyph_line_t *line)
{
    while (sb_list_length(line->runs) > 0) {
        uint64_t last = sb_list_length(line->runs) - 1;
        sb_glyph_run_t *run = sb_list_remove(line->runs, last);
        sb_glyph_run_free(run);
    }
    sb_list_free(line->runs);

    free(line);
}


sb_glyph_layout_t* sb_glyph_layout_new()
{
    sb_glyph_layout_t *layout = malloc(sizeof(sb_glyph_layout_t));

    layout->lines = sb_list_new();

    return layout;
}

void sb_glyph_layout_add_line(sb_glyph_layout_t *layout, sb_glyph_line_t *line)
{
    sb_list_push(layout->lines, line);
}

uint32_t sb_glyph_layout_line_count(const sb_glyph_layout_t *layout)
{
    return sb_list_length(layout->lines);
}

const sb_list_t* sb_glyph_layout_lines(const sb_glyph_layout_t *layout)
{
    return layout->lines;
}

void sb_glyph_layout_free(sb_glyph_layout_t *layout)
{
    while (sb_list_length(layout->lines) > 0) {
        uint64_t last = sb_list_length(layout->lines) - 1;
        sb_glyph_line_t *line = sb_list_remove(layout->lines, last);
        sb_glyph_line_free(line);
    }
    free(layout);
}

#ifdef __cplusplus
}
#endif // __cplusplus
