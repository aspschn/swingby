#ifndef _SWINGBY_GLYPH_IMPL_H
#define _SWINGBY_GLYPH_IMPL_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct SbGlyphLineImpl SbGlyphLineImpl;

typedef struct SbGlyphLayoutImpl SbGlyphLayoutImpl;

SbGlyphLineImpl* sb_glyph_line_impl_new();

void sb_glyph_line_impl_add_run(SbGlyphLineImpl *impl, sb_glyph_run_t *run);

uint32_t sb_glyph_line_impl_run_count(const SbGlyphLineImpl *impl);

const sb_glyph_run_t** sb_glyph_line_impl_runs(const SbGlyphLineImpl *impl);

void sb_glyph_line_impl_free(SbGlyphLineImpl *impl);


SbGlyphLayoutImpl* sb_glyph_layout_impl_new();

void sb_glyph_layout_impl_add_line(SbGlyphLayoutImpl *impl,
                                   sb_glyph_line_t *line);

uint32_t sb_glyph_layout_impl_line_count(const SbGlyphLayoutImpl *impl);

const sb_glyph_line_t**
sb_glyph_layout_impl_lines(const SbGlyphLayoutImpl *impl);

void sb_glyph_layout_impl_free(SbGlyphLayoutImpl *impl);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _SWINGBY_GLYPH_IMPL_H */
