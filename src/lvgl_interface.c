// lvgl_interface.c

#include "lvgl_interface.h"
#include "tinygl_interface.h"
#include <stdlib.h>
#include <string.h>

/* Static Variables */
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *lvgl_buffer1 = NULL;
static lv_color_t *lvgl_buffer2 = NULL;
static lv_disp_drv_t disp_drv;
static lv_obj_t *canvas = NULL;
static int display_width = 0;
static int display_height = 0;

/**
 * @brief Flush callback to transfer LVGL canvas buffer to the actual display.
 *
 * This function should be implemented based on your MCU's display hardware.
 * For desktop testing, this can be integrated with SDL or another windowing system.
 *
 * @param disp       Pointer to the display driver.
 * @param area       Pointer to the area of the display to update.
 * @param color_p    Pointer to the color buffer to flush.
 */
static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    /* 
     * Implement display-specific buffer transfer here.
     * This could involve sending the pixel data over SPI, DMA, etc.
     * For desktop testing with LVGL's SDL driver, you might not need to implement this.
     */
    
    // Example Placeholder Code:
    // for(int y = area->y1; y <= area->y2; y++) {
    //     for(int x = area->x1; x <= area->x2; x++) {
    //         // Send color_p[x + y * (area->x2 - area->x1 + 1)] to display at (x, y)
    //     }
    // }
    
    /* Inform LVGL that flushing is done */
    lv_disp_flush_ready(disp);
} lv_disp_flush_ready(disp);

/**
 * @brief Initialize LVGL and set up the display driver and canvas.
 *
 * @param width  The width of the display in pixels.
 * @param height The height of the display in pixels.
 * @return int 0 on success, -1 on failure.
 */
int lvgl_init(int width, int height) {

    display_width = width;
    display_height = height;

    /* Initialize LVGL */
    lv_init();

    /* Allocate LVGL buffers */
    lvgl_buffer1 = malloc(sizeof(lv_color_t) * width * height);
    if (!lvgl_buffer1) {
        return -1;
    }

    lvgl_buffer2 = malloc(sizeof(lv_color_t) * width * height);
    if (!lvgl_buffer2) {
        free(lvgl_buffer1);
        return -1;
    }

    /* Initialize the draw buffer with double buffering */
    lv_disp_draw_buf_init(&draw_buf, lvgl_buffer1, lvgl_buffer2, width * height);

    /* Initialize and register the display driver */
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf = &draw_buf;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.hor_res = width;
    disp_drv.ver_res = height;
    disp_drv.rotated = LV_DISP_ROT_NONE; /* Adjust if your display is rotated */
    lv_disp_drv_register(&disp_drv);

    /* Create an LVGL canvas to display TinyGL's framebuffer */
    canvas = lv_canvas_create(lv_scr_act(), NULL);
    if (!canvas) {
        free(lvgl_buffer1);
        free(lvgl_buffer2);
        return -1;
    }

    /* Set the canvas buffer */
    lv_canvas_set_buffer(canvas, lvgl_buffer1, width, height, LV_IMG_CF_TRUE_COLOR);

    /* Fill the canvas with a default color (optional) */
    lv_canvas_fill_bg(canvas, lv_color_black(), LV_OPA_COVER);

    /* Align the canvas to the center */
    lv_obj_set_size(canvas, width, height);
    lv_obj_align(canvas, NULL, LV_ALIGN_CENTER, 0, 0);

    return 0;
}

/**
 * @brief Update the LVGL canvas with TinyGL's framebuffer data.
 *
 * This function converts TinyGL's framebuffer data to LVGL's format and updates the canvas.
 */
void lvgl_update_canvas(void) {
    /* Get TinyGL's framebuffer */
    void *fb_pixels = tinygl_get_framebuffer();
    if (!fb_pixels) return;

    /* Get the LVGL canvas buffer */
    lv_color_t *canvas_buf = lv_canvas_get_buffer(canvas);
    if (!canvas_buf) return;

    /* Optimize color conversion based on TinyGL's render bits */
#if TGL_FEATURE_RENDER_BITS == 32
    /* For ARGB8888, direct memcpy is already efficient */
    memcpy(canvas_buf, fb_pixels, display_width * display_height * sizeof(lv_color_t));
#elif TGL_FEATURE_RENDER_BITS == 16
    /* For RGB565, optimize the conversion loop */
    uint16_t *fb_16 = (uint16_t*)fb_pixels;
    lv_color_t *cb = canvas_buf;
    int total_pixels = display_width * display_height;

    for(int i = 0; i < total_pixels; i++) {
        uint16_t pixel = fb_16[i];
        /* Extract RGB components */
        uint8_t r = ((pixel >> 11) & 0x1F) << 3;
        uint8_t g = ((pixel >> 5) & 0x3F) << 2;
        uint8_t b = (pixel & 0x1F) << 3;
        /* Assign to canvas buffer */
        cb[i].full = LV_COLOR_MAKE(r, g, b).full;
    }
#endif

    /* Invalidate the canvas to trigger a redraw */
    lv_obj_invalidate(canvas);
}

/**
 * @brief Cleanup LVGL resources and free allocated memory.
 */
void lvgl_cleanup(void) {
    /* Free LVGL canvas buffer */
    if (canvas) {
        lv_color_t *canvas_buf = lv_canvas_get_buffer(canvas);
        if (canvas_buf) {
            free(canvas_buf);
        }
        lv_obj_del(canvas);
        canvas = NULL;
    }

    /* Free LVGL buffers */
    if (lvgl_buffer1) {
        free(lvgl_buffer1);
        lvgl_buffer1 = NULL;
    }

    if (lvgl_buffer2) {
        free(lvgl_buffer2);
        lvgl_buffer2 = NULL;
    }

    /* Cleanup LVGL */
    lv_deinit();

    /* Note: If you have initialized display hardware, ensure to cleanup here */
}