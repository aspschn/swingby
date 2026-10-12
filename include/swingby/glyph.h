#ifndef _SWINGBY_GLYPH_H
#define _SWINGBY_GLYPH_H

#include <stdint.h>

#include <swingby/point.h>
#include <swingby/font.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct sb_list_t sb_list_t;

typedef uint32_t sb_glyph_id_t;

typedef struct sb_glyph_run_t sb_glyph_run_t;

typedef struct sb_glyph_block_t sb_glyph_block_t;


sb_glyph_run_t* sb_glyph_run_new(uint32_t count, const sb_font_t *font);

uint32_t sb_glyph_run_count(const sb_glyph_run_t *run);

sb_glyph_id_t* sb_glyph_run_glyphs(const sb_glyph_run_t *run);

sb_point_t* sb_glyph_run_positions(const sb_glyph_run_t *run);

const sb_font_t* sb_glyph_run_font(const sb_glyph_run_t *run);

void sb_glyph_run_free(sb_glyph_run_t *run);


sb_glyph_block_t* sb_glyph_block_new();

uint64_t sb_glyph_block_count(const sb_glyph_block_t *block);

void sb_glyph_block_add(sb_glyph_block_t *block, sb_glyph_run_t *run);

const sb_glyph_run_t* sb_glyph_block_at(const sb_glyph_block_t *block,
                                         uint64_t index);

float sb_glyph_block_baseline(const sb_glyph_block_t *block);

void sb_glyph_block_set_baseline(sb_glyph_block_t *block, float baseline);

void sb_glyph_block_free(sb_glyph_block_t *block);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _SWINGBY_GLYPH_H */
