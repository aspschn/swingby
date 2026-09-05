# CHANGELOG

## v0.10.0-dev

**[Breaking changes]**

- Some simple, basic types now value-type. e.g. `sb_point_t`, `sb_color_t` etc.

- Integer-version value types changed to signed integer.

- `sb_rect_t::pos` now removed.


## v0.9.0-rc.1

**[Breaking changes]**

- `set_title` method in `sb_desktop_surface_t` type.

- Anti-aliasing property added to image typed `sb_view_t`.

- Some performance improvements.

- Fractional scale factor. This breaks API/ABIs.

- External Wayland protocols support.

- `sb_paint_t` as an opaque pointer.


## v0.8.0

- Enhanced glyph rendering. Specifically multiline glyphs.

- `visible` property in `sb_view_t` type.


## v0.7.0

- `sb_rect_t::pos` now deprecated. Use `sb_rect_t::position` instead.

- Enhance `SB_VIEW_RENDER_TYPE_GL` typed `sb_view_t`.

- Initialization function for `sb_font_metrics_t` struct.

- Glyphs type view now rendering better. But still is in experimental.


## v0.6.1

No public patches in this version.


## v0.6.0

- `SB_VIEW_RENDER_TYPE_CANVAS` added.

- Building Skia now faster.

- `SB_VIEW_RENDER_TYPE_GL` added.

- Drop 'win32' experimental platform support.
