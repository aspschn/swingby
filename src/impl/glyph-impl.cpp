#include "glyph-impl.hpp"

#include <swingby/glyph.h>

#include "glyph-impl.h"

//!<=================
//!< Glyph Line
//!<=================

SbGlyphLineImpl::SbGlyphLineImpl()
{
}

SbGlyphLineImpl::~SbGlyphLineImpl()
{
    for (auto run: _runs) {
        sb_glyph_run_free(run);
    }
}

void SbGlyphLineImpl::add_run(sb_glyph_run_t *run)
{
    _runs.push_back(run);
}

uint32_t SbGlyphLineImpl::run_count() const
{
    return _runs.size();
}

sb_glyph_run_t** SbGlyphLineImpl::runs() const
{
    return (sb_glyph_run_t**)_runs.data();
}

//!<=================
//!< Glyph Layout
//!<=================

SbGlyphLayoutImpl::SbGlyphLayoutImpl()
{
}

SbGlyphLayoutImpl::~SbGlyphLayoutImpl()
{
    for (auto line: _lines) {
        sb_glyph_line_free(line);
    }
}

void SbGlyphLayoutImpl::add_line(sb_glyph_line_t *line)
{
    _lines.push_back(line);
}

uint32_t SbGlyphLayoutImpl::line_count() const
{
    return _lines.size();
}

sb_glyph_line_t** SbGlyphLayoutImpl::lines() const
{
    return (sb_glyph_line_t**)_lines.data();
}


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

SbGlyphLineImpl* sb_glyph_line_impl_new()
{
    return new SbGlyphLineImpl;
}

void sb_glyph_line_impl_add_run(SbGlyphLineImpl *impl, sb_glyph_run_t *run)
{
    impl->add_run(run);
}

uint32_t sb_glyph_line_impl_run_count(const SbGlyphLineImpl *impl)
{
    return impl->run_count();
}

const sb_glyph_run_t** sb_glyph_line_impl_runs(const SbGlyphLineImpl *impl)
{
    return (const sb_glyph_run_t**)impl->runs();
}

void sb_glyph_line_impl_free(SbGlyphLineImpl *impl)
{
    delete impl;
}


SbGlyphLayoutImpl* sb_glyph_layout_impl_new()
{
    return new SbGlyphLayoutImpl;
}

void sb_glyph_layout_impl_add_line(SbGlyphLayoutImpl *impl,
                                   sb_glyph_line_t *line)
{
    impl->add_line(line);
}

uint32_t sb_glyph_layout_impl_line_count(const SbGlyphLayoutImpl *impl)
{
    return impl->line_count();
}

const sb_glyph_line_t**
sb_glyph_layout_impl_lines(const SbGlyphLayoutImpl *impl)
{
    return (const sb_glyph_line_t**)impl->lines();
}

void sb_glyph_layout_impl_free(SbGlyphLayoutImpl *impl)
{
    delete impl;
}

#ifdef __cplusplus
}
#endif // __cplusplus
