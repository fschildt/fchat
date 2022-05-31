#include "../general.h"

bool renderer_init();
void renderer_viewport(s32 x, s32 y, s32 width, s32 height);
void renderer_draw_color(f32 r, f32 g, f32 b);
stbtt_bakedchar* renderer_get_stbtt_baked_chars(); // TODO: find a nice position

bool renderer_setup_text_drawing(u8 *ttf_buff, const char *vs_src, const char *fs_src);
void renderer_draw_text(const char *text, f32 x, f32 y);
