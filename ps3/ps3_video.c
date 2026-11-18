#include "ps3_platform.h"
#include <tiny3d.h>
#include "font_msx.h"
#include <string.h>
#include <stdlib.h>

volatile int g_xmb_menu_open = 0, g_take_screenshot_pending = 0;
static u32 *g_vram_buf = NULL, g_vram_off = 0, g_font_off = 0;

int ps3_video_init(void) {
    int r = tiny3d_Init(4 * 1024 * 1024);
    ps3_log("1. tiny3d_Init(4MB) ret=0x%x\n", r);
    for (int k = 0; k < 3; k++) { tiny3d_Clear(0xFF101830, TINY3D_CLEAR_ALL); tiny3d_Flip(); }
    ps3_log("2. HDMI 3-Flip OK\n");
    u32 *vram = tiny3d_AllocTexture(8 * 1024 * 1024);
    g_vram_buf = (u32 *)(((uintptr_t)vram + 255) & ~((uintptr_t)255));
    g_vram_off = tiny3d_TextureOffset(g_vram_buf);
    u32 *fbuf = (u32 *)(((uintptr_t)(g_vram_buf + 512 * 512) + 255) & ~((uintptr_t)255));
    g_font_off = tiny3d_TextureOffset(fbuf);
    memset(fbuf, 0, 128 * 128 * 4);
    for (int c = 32; c < 128; c++) {
        int gx = (c % 16) * 8, gy = ((c - 32) / 16) * 8;
        for (int row = 0; row < 8; row++) {
            u8 bits = msx[(c - 32) * 8 + row];
            for (int col = 0; col < 8; col++)
                fbuf[(gy + row) * 128 + (gx + col)] = (bits & (0x80 >> col)) ? 0xFFFFFFFFu : 0x00000000u;
        }
    }
    ps3_log("3. Textura MSX 128x128 en VRAM OK\n");
    return 0;
}
void ps3_video_shutdown(void) {}
void ps3_video_clear(uint8_t r, uint8_t g, uint8_t b) {
    u32 col = ((u32)r << 24) | ((u32)g << 16) | ((u32)b << 8) | 0xFFu;
    tiny3d_Clear(col, TINY3D_CLEAR_ALL);
    tiny3d_AlphaTest(1, 0x10, TINY3D_ALPHA_FUNC_GEQUAL);
    tiny3d_BlendFunc(1, TINY3D_BLEND_FUNC_SRC_RGB_SRC_ALPHA | TINY3D_BLEND_FUNC_SRC_ALPHA_SRC_ALPHA,
                     TINY3D_BLEND_FUNC_DST_RGB_ONE_MINUS_SRC_ALPHA | TINY3D_BLEND_FUNC_DST_ALPHA_ZERO,
                     TINY3D_BLEND_RGB_FUNC_ADD | TINY3D_BLEND_ALPHA_FUNC_ADD);
    tiny3d_Project2D();
}
void ps3_video_present(void) { tiny3d_Flip(); }
static u32 argb_to_rgba(uint32_t c) { return (c << 8) | (c >> 24); }
void ps3_draw_rect(int x, int y, int w, int h, uint32_t argb) {
    float x0 = x * (848.0f / 1280.0f), y0 = y * (512.0f / 720.0f);
    float x1 = (x + w) * (848.0f / 1280.0f), y1 = (y + h) * (512.0f / 720.0f);
    u32 col = argb_to_rgba(argb);
    tiny3d_SetPolygon(TINY3D_QUADS);
    tiny3d_VertexPos(x0, y0, 10); tiny3d_VertexColor(col);
    tiny3d_VertexPos(x1, y0, 10); tiny3d_VertexColor(col);
    tiny3d_VertexPos(x1, y1, 10); tiny3d_VertexColor(col);
    tiny3d_VertexPos(x0, y1, 10); tiny3d_VertexColor(col);
    tiny3d_End();
}
void ps3_draw_hline(int x, int y, int w, uint32_t argb) { ps3_draw_rect(x, y, w, 2, argb); }
void ps3_draw_text(int x, int y, uint32_t argb, int scale, const char *s) {
    if (!s) return;
    tiny3d_SetTexture(0, g_font_off, 128, 128, 512, TINY3D_TEX_FORMAT_A8R8G8B8, TEXTURE_NEAREST);
    float sc = (scale >= 2) ? 2.2f : 1.4f, cw = 8.0f * sc, ch = 8.0f * sc;
    float bx = x * (848.0f / 1280.0f), by = y * (512.0f / 720.0f);
    u32 col = argb_to_rgba(argb);
    for (int i = 0; s[i]; i++) {
        unsigned char c = (unsigned char)s[i]; if (c < 32 || c > 127) continue;
        float u0 = ((c % 16) * 8) / 128.0f, v0 = (((c - 32) / 16) * 8) / 128.0f;
        float u1 = u0 + 7.8f / 128.0f, v1 = v0 + 7.8f / 128.0f, px = bx + i * (cw + sc);
        tiny3d_SetPolygon(TINY3D_QUADS);
        tiny3d_VertexPos(px, by, 5);      tiny3d_VertexColor(col); tiny3d_VertexTexture(u0, v0);
        tiny3d_VertexPos(px + cw, by, 5); tiny3d_VertexColor(col); tiny3d_VertexTexture(u1, v0);
        tiny3d_VertexPos(px + cw, by + ch, 5); tiny3d_VertexColor(col); tiny3d_VertexTexture(u1, v1);
        tiny3d_VertexPos(px, by + ch, 5); tiny3d_VertexColor(col); tiny3d_VertexTexture(u0, v1);
        tiny3d_End();
    }
}
void ps3_draw_text_shadow(int x, int y, uint32_t argb, int scale, const char *text) { ps3_draw_text(x, y, argb, scale, text); }
void ps3_pcfx_blit_frame(const uint32_t *px, int rx, int ry, int rw, int rh, int pitch) {
    if (!g_vram_buf || !px || rw <= 0 || rh <= 0) return;
    int cw = (rw > 512) ? 512 : rw, ch = (rh > 512) ? 512 : rh;
    for (int y = 0; y < ch; y++) memcpy(g_vram_buf + y * 512, px + (size_t)(ry + y) * pitch + rx, cw * 4);
    tiny3d_Clear(0x000000FF, TINY3D_CLEAR_ALL); tiny3d_Project2D();
    tiny3d_SetTexture(0, g_vram_off, 512, 512, 512 * 4, TINY3D_TEX_FORMAT_A8R8G8B8, TEXTURE_NEAREST);
    float u1 = (float)cw / 512.0f, v1 = (float)ch / 512.0f;
    tiny3d_SetPolygon(TINY3D_QUADS);
    tiny3d_VertexPos(82, 0, 10);    tiny3d_VertexTexture(0, 0);
    tiny3d_VertexPos(766, 0, 10);   tiny3d_VertexTexture(u1, 0);
    tiny3d_VertexPos(766, 512, 10); tiny3d_VertexTexture(u1, v1);
    tiny3d_VertexPos(82, 512, 10);  tiny3d_VertexTexture(0, v1);
    tiny3d_End(); tiny3d_Flip();
}
int ps3_load_rom_preview(const char *a, const char *b, uint32_t *c, int d, int e) { (void)a;(void)b;(void)c;(void)d;(void)e; return 0; }
void ps3_save_rom_preview(const char *a) { (void)a; }
