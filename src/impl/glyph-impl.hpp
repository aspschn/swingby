#ifndef _SWINGBY_GLYPH_IMPL_HPP
#define _SWINGBY_GLYPH_IMPL_HPP

#include <stdint.h>

#include <vector>
#include <memory>

#include <swingby/point.h>

typedef struct sb_glyph_run_t sb_glyph_run_t;

typedef struct sb_glyph_line_t sb_glyph_line_t;


class __attribute__((visibility("hidden"))) SbGlyphLineImpl
{
public:
    SbGlyphLineImpl();

    ~SbGlyphLineImpl();

    void add_run(sb_glyph_run_t *run);

    uint32_t run_count() const;

    sb_glyph_run_t** runs() const;

private:
    std::vector<sb_glyph_run_t*> _runs;
};


class __attribute__((visibility("hidden"))) SbGlyphLayoutImpl
{
public:
    SbGlyphLayoutImpl();

    ~SbGlyphLayoutImpl();

    void add_line(sb_glyph_line_t *line);

    uint32_t line_count() const;

    sb_glyph_line_t** lines() const;

private:
    std::vector<sb_glyph_line_t*> _lines;
};

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _SWINGBY_GLYPH_IMPL_HPP */
