#include <swingby/glyph.h>

#include <stddef.h>
#include <stdlib.h>

#include <swingby/font.h>
#include <swingby/point.h>
#include <swingby/list.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//!<==================
//!< New Glyph APIs
//!<==================

struct sb_glyph_run_t {
    sb_glyph_id_t *glyphs;
    sb_point_t *positions;
    sb_font_t font;
    uint32_t count;
};

struct sb_glyph_block_t {
    sb_list_t *runs;
    float baseline;
};

sb_glyph_run_t* sb_glyph_run_new(uint32_t count, const sb_font_t *font)
{
    sb_glyph_run_t *run = malloc(sizeof(sb_glyph_run_t));
    run->count = count;
    run->glyphs = malloc(sizeof(sb_glyph_id_t) * count);
    run->positions = malloc(sizeof(sb_point_t) * count);
    run->font = *font;

    return run;
}

uint32_t sb_glyph_run_count(const sb_glyph_run_t *run)
{
    return run->count;
}

sb_glyph_id_t* sb_glyph_run_glyphs(const sb_glyph_run_t *run)
{
    return run->glyphs;
}

sb_point_t* sb_glyph_run_positions(const sb_glyph_run_t *run)
{
    return run->positions;
}

const sb_font_t* sb_glyph_run_font(const sb_glyph_run_t *run)
{
    return &run->font;
}

void sb_glyph_run_free(sb_glyph_run_t *run)
{
    free(run->glyphs);
    free(run->positions);

    free(run);
}


sb_glyph_block_t* sb_glyph_block_new()
{
    sb_glyph_block_t *block = malloc(sizeof(sb_glyph_block_t));

    block->runs = sb_list_new();
    block->baseline = 0.0f;

    return block;
}

uint64_t sb_glyph_block_count(const sb_glyph_block_t *block)
{
    return sb_list_length(block->runs);
}

void sb_glyph_block_add(sb_glyph_block_t *block, sb_glyph_run_t *run)
{
    sb_list_push(block->runs, run);
}

float sb_glyph_block_baseline(const sb_glyph_block_t *block)
{
    return block->baseline;
}

const sb_glyph_run_t* sb_glyph_block_at(const sb_glyph_block_t *block,
                                         uint64_t index)
{
    return (const sb_glyph_run_t*)sb_list_at(block->runs, index);
}

void sb_glyph_block_set_baseline(sb_glyph_block_t *block, float baseline)
{
    block->baseline = baseline;
}

static void runs_free_func(void *run)
{
    sb_glyph_run_free(run);
}

void sb_glyph_block_free(sb_glyph_block_t *block)
{
    sb_list_clear(block->runs, runs_free_func);
    sb_list_free(block->runs);
    free(block);
}

#ifdef __cplusplus
}
#endif // __cplusplus
