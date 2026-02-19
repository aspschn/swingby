#include <stdio.h>
#include <string.h>

#include <pango/pango.h>
#include <pango/pangoft2.h>
#include <pango/pangofc-font.h>
#include <fontconfig/fontconfig.h>

#include <swingby/swingby.h>

static int font_index = 0;
static char font_paths[8][256];

static void on_preferred_scale(sb_event_t *event, void *user_data)
{
    sb_surface_t *surface = event->target;

    fprintf(stderr, "on_preferred_scale.\n");

    sb_surface_set_scale(surface, event->scale.scale);
}

static sb_glyph_layout_t* pango_layout(const char *text, float size)
{
    // Fontconfig init.
    if (!FcInit()) {
        fprintf(stderr, "Failed to initialize Fontconfig.\n");
        return NULL;
    }

    PangoFontMap *fontmap = pango_ft2_font_map_new();
    if (!fontmap) {
        fprintf(stderr, "Failed to create FT2 font map.\n");
        return NULL;
    }

    // Create context.
    PangoContext *context = pango_font_map_create_context(fontmap);
    if (!context) {
        fprintf(stderr, "Failed to create Pango context.\n");
        g_object_unref(fontmap);
        return NULL;
    }

    // Create layout.
    PangoLayout *layout = pango_layout_new(context);
    if (!layout) {
        fprintf(stderr, "Failed to create Pango layout.\n");
        g_object_unref(context);
        g_object_unref(fontmap);
        return NULL;
    }

    // Set text.
    pango_layout_set_text(layout, text, -1);

    // ??
    pango_layout_set_width(layout, 600 * PANGO_SCALE);
    pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);

    PangoFontDescription *desc = pango_font_description_new();
    pango_font_description_set_absolute_size(desc, size * PANGO_SCALE);
    pango_layout_set_font_description(layout, desc);

    pango_layout_context_changed(layout);

    sb_glyph_layout_t *sb_layout = sb_glyph_layout_new();

    int line_count = pango_layout_get_line_count(layout);
    printf(" line count: %d\n", line_count);
    for (int i = 0; i < line_count; ++i) {
        sb_glyph_line_t *sb_line = sb_glyph_line_new();

        PangoLayoutLine *line = pango_layout_get_line_readonly(layout, i);
        float baseline = pango_layout_get_baseline(layout) / (float)PANGO_SCALE;

        // Count glyph runs.
        int run_count = 0;
        for (GSList *it = line->runs; it != NULL; it = it->next) {
            ++run_count;
        }

        for (GSList *l = line->runs; l != NULL; l = l->next) {
            PangoGlyphItem *item = (PangoGlyphItem*)l->data;

            PangoFont *font = item->item->analysis.font;

            // Get font description.
            char *desc_str = pango_font_description_to_string(desc);

            printf("  Font description: %s\n", desc_str);

            PangoFcFont *fcfont = PANGO_FC_FONT(font);
            FcPattern *pattern = pango_fc_font_get_pattern(fcfont);

            FcChar8 *file = NULL;
            int index = 0;

            FcResult res;
            res = FcPatternGetString(pattern, FC_FILE, 0, &file);
            if (res == FcResultMatch) {
                // Store to the global variable.
                strcpy(font_paths[font_index], file);

                res = FcPatternGetInteger(pattern, FC_INDEX, 0, &index);
                if (res != FcResultMatch) {
                    index = 0;
                }

                printf("  Font file: %s\n", file);
                printf("  TTC index: %d\n", index);
            } else {
                printf("  Font file: unknown\n");
            }

            // Direction.
            printf("  Direction: %s\n",
                (item->item->analysis.level % 2) ? "RTL" : "LTR");

            sb_font_t sb_font;
            sb_font.path = font_paths[font_index++];
            sb_font.ttc_index = index;
            sb_font.size = size;
            sb_glyph_run_t *sb_run = sb_glyph_run_new(item->glyphs->num_glyphs, &sb_font);
            sb_glyph_t *sb_glyphs = sb_glyph_run_glyphs(sb_run);

            // Get glyph data.
            PangoGlyphString *glyphs = item->glyphs;
            int glyph_count = glyphs->num_glyphs;
            for (int idx = 0; idx < glyphs->num_glyphs; ++idx) {
                PangoGlyphInfo *gi = &glyphs->glyphs[idx];

                float advance = gi->geometry.width / (float)PANGO_SCALE;
                printf("  glyph: %u advance: %f x_offset: %f y_offset: %f\n",
                    gi->glyph,
                    advance,
                    gi->geometry.x_offset / (float)PANGO_SCALE,
                    gi->geometry.y_offset / (float)PANGO_SCALE
                );

                sb_glyphs[idx].id = gi->glyph;
                sb_glyphs[idx].advance = advance;
                sb_glyphs[idx].offset.x = gi->geometry.x_offset / (float)PANGO_SCALE;
                sb_glyphs[idx].offset.y = gi->geometry.y_offset / (float)PANGO_SCALE;
            }
            sb_glyph_line_add_run(sb_line, sb_run);

            printf("\n");

            g_free(desc_str);
        }
        sb_glyph_layout_add_line(sb_layout, sb_line);
    }

    pango_font_description_free(desc);
    g_object_unref(layout);
    g_object_unref(context);
    g_object_unref(fontmap);

    FcFini();

    return sb_layout;
}

int main(int argc, char *argv[])
{
    sb_application_t *app = sb_application_new(argc, argv);

    sb_desktop_surface_t *surface = sb_desktop_surface_new(
        SB_DESKTOP_SURFACE_ROLE_TOPLEVEL);

    sb_surface_add_event_listener(
        sb_desktop_surface_surface(surface),
        SB_EVENT_TYPE_PREFERRED_SCALE,
        on_preferred_scale,
        NULL);

    sb_rect_t geometry = { { 10.0f, 10.0f }, { 140.0f, 100.0f } };
    sb_view_t *view = sb_view_new(
        sb_surface_root_view(sb_desktop_surface_surface(surface)), &geometry);
    sb_color_t color = { 0.9f, 0.9f, 0.9f, 1.0f };
    sb_view_set_color(view, &color);

    sb_view_set_clip(view, true);

    geometry.pos.x = 0.0f;
    geometry.pos.y = 0.0f;
    geometry.size.width = 60.0f;
    geometry.size.height = 30.0f;
    sb_view_t *glyphs_view = sb_view_new(view, &geometry);
    sb_view_set_render_type(glyphs_view, SB_VIEW_RENDER_TYPE_GLYPHS);

    sb_glyph_layout_t *glyph_layout = pango_layout("Hello مرحبا 你好", 16.0f);

    sb_view_set_glyph_layout(glyphs_view, glyph_layout);

    sb_desktop_surface_show(surface);

    return sb_application_exec(app);
}
