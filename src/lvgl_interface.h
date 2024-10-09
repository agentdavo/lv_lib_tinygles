// lvgl_interface.h

#ifndef LVGL_INTERFACE_H
#define LVGL_INTERFACE_H

#include "lvgl/lvgl.h"

/**
 * @brief Initialize LVGL and create a canvas for rendering TinyGL's framebuffer.
 *
 * @param width  The width of the display in pixels.
 * @param height The height of the display in pixels.
 * @return int 0 on success, -1 on failure.
 */
int lvgl_init(int width, int height);

/**
 * @brief Update the LVGL canvas with TinyGL's framebuffer data.
 *
 * This function should be called every frame after TinyGL has rendered the scene.
 */
void lvgl_update_canvas(void);

/**
 * @brief Cleanup LVGL resources and any associated display hardware.
 */
void lvgl_cleanup(void);

#endif /* LVGL_INTERFACE_H */
