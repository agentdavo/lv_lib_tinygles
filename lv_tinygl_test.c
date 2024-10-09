/**
 * @file lv_tinygl_test.c
 *
 */

#include "lvgl/lvgl.h"
#include "src/lvgl_interface.h"    // LVGL TingGL helpers
#include "src/tinygl_interface.h"  // To access TinyGL's functions
#include "src/3dMath.h"            // To access TinyGL's aligned math functions
#include <unistd.h>                // For usleep
#include <stdio.h>                 // For fprintf
#include <string.h>
#include <sys/time.h>
#include <time.h>

// Define M_PI if not defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void lv_gamepad(void);

static void gamepad_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    const char * btn_name = lv_obj_get_user_data(obj);

    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_PRESSED) {
        printf("%s Pressed\n", btn_name);
        if(strcmp(btn_name, "Up") == 0) {
            tinygl_move_camera_forward(0.1f);
        }
        else if(strcmp(btn_name, "Down") == 0) {
            tinygl_move_camera_backward(0.1f);
        }
        else if(strcmp(btn_name, "Left") == 0) {
            tinygl_move_camera_left(0.1f);
        }
        else if(strcmp(btn_name, "Right") == 0) {
            tinygl_move_camera_right(0.1f);
        }

    }
    else if(code == LV_EVENT_RELEASED) {
        printf("%s Released\n", btn_name);
        // Handle button release if necessary
    }
}

static void create_gamepad_controls(lv_obj_t *parent) {

    /* Create a container for the gamepad */
    gamepad_container = lv_obj_create(parent);
    lv_obj_set_size(gamepad_container, 300, 100);
    lv_obj_set_style_bg_color(gamepad_container, lv_color_hex(0x333333), LV_PART_MAIN);
    lv_obj_set_style_radius(gamepad_container, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_all(gamepad_container, 10, LV_PART_MAIN);
    lv_obj_set_flex_flow(gamepad_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_main_place(gamepad_container, LV_FLEX_PLACE_CENTER);
    lv_obj_set_flex_cross_place(gamepad_container, LV_FLEX_PLACE_CENTER);

    /* Create a D-pad container */
    lv_obj_t *dpad = lv_obj_create(gamepad_container);
    lv_obj_set_size(dpad, 150, 150);
    lv_obj_set_flex_flow(dpad, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_main_place(dpad, LV_FLEX_PLACE_CENTER);
    lv_obj_set_flex_cross_place(dpad, LV_FLEX_PLACE_CENTER);
    lv_obj_set_style_bg_color(dpad, lv_color_hex(0x444444), LV_PART_MAIN);
    lv_obj_set_style_radius(dpad, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_all(dpad, 5, LV_PART_MAIN);

    /* Create directional buttons */
    lv_obj_t *btn_up = lv_btn_create(dpad);
    lv_obj_set_size(btn_up, 60, 60);
    lv_obj_set_pos(btn_up, 60, 0); // Positioning manually for D-pad layout
    lv_obj_add_event_cb(btn_up, gamepad_event_cb, LV_EVENT_ALL, (void*)"Up");

    lv_obj_t *label_up = lv_label_create(btn_up);
    lv_label_set_text(label_up, LV_SYMBOL_UP);
    lv_obj_center(label_up);

    lv_obj_t *btn_down = lv_btn_create(dpad);
    lv_obj_set_size(btn_down, 60, 60);
    lv_obj_set_pos(btn_down, 60, 120);
    lv_obj_add_event_cb(btn_down, gamepad_event_cb, LV_EVENT_ALL, (void*)"Down");

    lv_obj_t *label_down = lv_label_create(btn_down);
    lv_label_set_text(label_down, LV_SYMBOL_DOWN);
    lv_obj_center(label_down);

    lv_obj_t *btn_left = lv_btn_create(dpad);
    lv_obj_set_size(btn_left, 60, 60);
    lv_obj_set_pos(btn_left, 0, 60);
    lv_obj_add_event_cb(btn_left, gamepad_event_cb, LV_EVENT_ALL, (void*)"Left");

    lv_obj_t *label_left = lv_label_create(btn_left);
    lv_label_set_text(label_left, LV_SYMBOL_LEFT);
    lv_obj_center(label_left);

    lv_obj_t *btn_right = lv_btn_create(dpad);
    lv_obj_set_size(btn_right, 60, 60);
    lv_obj_set_pos(btn_right, 120, 60);
    lv_obj_add_event_cb(btn_right, gamepad_event_cb, LV_EVENT_ALL, (void*)"Right");

    lv_obj_t *label_right = lv_label_create(btn_right);
    lv_label_set_text(label_right, LV_SYMBOL_RIGHT);
    lv_obj_center(label_right);

}


int main(void)
{

    /* Initialize TinyGL */
    if (tinygl_init(240, 320, TGL_FEATURE_RENDER_BITS) != 0) {
        fprintf(stderr, "TinyGL Initialization failed.\n");
        return -1;
    }

    /* Initialize LVGL */
    if (lvgl_init(320, 320) != 0) {
        fprintf(stderr, "LVGL Initialization failed.\n");
        tinygl_cleanup();
        return -1;
    }

    /* Set the camera */
    Vec3 eye = (Vec3){.x = 0.0f, .y = 0.0f, .z = 3.0f};
    Vec3 at = (Vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f};
    Vec3 up = (Vec3){.x = 0.0f, .y = 1.0f, .z = 0.0f};
    tinygl_set_camera(eye, at, up);

    /* Create a grid container */
    lv_obj_t * grid_container = lv_obj_create(lv_scr_act());
    lv_obj_set_size(grid_container, width, height);
    lv_obj_align(grid_container, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(grid_container, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_border_width(grid_container, 0, LV_PART_MAIN);

    /* Define grid columns and rows */
    static const lv_coord_t col_dsc[] = {LV_GRID_TEMPLATE_COL_FR(1), LV_GRID_TEMPLATE_COL_FR(1), LV_GRID_TEMPLATE_COL_FR(1), LV_GRID_TEMPLATE_COL_FR(1), LV_GRID_TEMPLATE_COL_TEMPLATE_LAST};
    static const lv_coord_t row_dsc[] = {LV_GRID_TEMPLATE_ROW_FR(7), LV_GRID_TEMPLATE_ROW_FR(3), LV_GRID_TEMPLATE_ROW_TEMPLATE_LAST};

    /* Set grid descriptor arrays */
    lv_obj_set_grid_dsc_array(grid_container, col_dsc, row_dsc);
    lv_obj_set_layout(grid_container, LV_LAYOUT_GRID);

    /* Create LVGL Canvas for TinyGL framebuffer in the top row */
    canvas = lv_canvas_create(grid_container);
    lv_obj_set_size(canvas, width - 20, (height * 7) / 10 - 20); // Subtracting padding
    lv_obj_set_grid_cell(canvas, LV_GRID_ALIGN_FILL, 0, 4,   /* Column 0, span 4 columns */
                                 LV_GRID_ALIGN_FILL, 0, 1);  /* Row 0, span 1 row */
    lv_obj_set_style_bg_color(canvas, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_border_width(canvas, 0, LV_PART_MAIN);

    /* Allocate canvas buffer */
    lv_color_t * canvas_buf = malloc(sizeof(lv_color_t) * (width - 20) * ((height * 7) / 10 - 20));
    if (!canvas_buf) {
        free(lvgl_buffer1);
        return -1;
    }
    lv_canvas_set_buffer(canvas, canvas_buf, width - 20, ((height * 7) / 10 - 20), LV_IMG_CF_TRUE_COLOR);

    /* Create Gamepad Controls in the bottom row */
    create_gamepad_controls(grid_container);

    /* Main loop */
    int running = 1;
    while(running) {

        /* Render TinyGL scene */
        tinygl_render();

        /* Update LVGL canvas with TinyGL's framebuffer */
        lvgl_update_canvas();

    }

    /* Cleanup */
    lvgl_cleanup();
    tinygl_cleanup();
    return 0;
}
