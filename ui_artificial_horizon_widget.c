#include "ui_artificial_horizon_widget.h"
#include <math.h> // For sin, cos, M_PI
#include <stdio.h> // For sprintf

// Define colors (can be customized)
#define SKY_COLOR   lv_palette_main(LV_PALETTE_GREEN)
#define GROUND_COLOR lv_palette_main(LV_PALETTE_BLUE)
#define LINE_COLOR  lv_color_white() // Color for horizon line if drawn
#define RETICLE_COLOR lv_color_black()
#define STATIC_HORIZON_LINE_COLOR lv_color_white() // White static line

// Flag to ensure fixed reticle is drawn only once
static bool fixed_reticle_drawn = false;

static void draw_artificial_horizon(lv_obj_t *canvas, float pitch_deg, float roll_deg) {
    if (!canvas) return;

    lv_obj_invalidate(canvas); // Mark canvas for redraw

    lv_canvas_fill_bg(canvas, GROUND_COLOR, LV_OPA_COVER); // Start with ground color

    int32_t canvas_w = lv_obj_get_width(canvas);
    int32_t canvas_h = lv_obj_get_height(canvas);
    int32_t center_x = canvas_w / 2;
    int32_t center_y = canvas_h / 2;

    // Convert degrees to radians for math functions
    float roll_rad = roll_deg * (M_PI / 180.0f);
    float pixels_per_degree_pitch = (float)canvas_h / 90.0f; 
    int32_t pitch_shift = (int32_t)(pitch_deg * pixels_per_degree_pitch);
    int32_t line_len_half = canvas_w > canvas_h ? canvas_w : canvas_h; 

    float s_roll = sinf(roll_rad);
    float c_roll = cosf(roll_rad);

    lv_point_t p1_horizon, p2_horizon;
    p1_horizon.x = center_x - line_len_half * c_roll;
    p1_horizon.y = center_y - line_len_half * s_roll + pitch_shift;

    p2_horizon.x = center_x + line_len_half * c_roll;
    p2_horizon.y = center_y + line_len_half * s_roll + pitch_shift;

    lv_point_t sky_points[4];
    int32_t sky_height_offset = canvas_h + canvas_w; 

    sky_points[0].x = p1_horizon.x - sky_height_offset * s_roll;
    sky_points[0].y = p1_horizon.y + sky_height_offset * c_roll;

    sky_points[1].x = p2_horizon.x - sky_height_offset * s_roll;
    sky_points[1].y = p2_horizon.y + sky_height_offset * c_roll;
    
    sky_points[2] = p2_horizon; 
    sky_points[3] = p1_horizon; 

    lv_draw_rect_dsc_t sky_fill_dsc;
    lv_draw_rect_dsc_init(&sky_fill_dsc);
    sky_fill_dsc.bg_color = SKY_COLOR;   
    sky_fill_dsc.bg_opa = LV_OPA_COVER;  
    sky_fill_dsc.border_width = 0;
    sky_fill_dsc.outline_width = 0;

    lv_canvas_draw_polygon(canvas, sky_points, 4, &sky_fill_dsc);
}


static void draw_fixed_reticle(lv_obj_t *parent_obj_for_reticle, int parent_w, int parent_h) {
    // This function should only create the reticle objects ONCE.
    if (fixed_reticle_drawn) { 
        return;
    }

    // Main static white horizontal reference line (using a styled lv_obj_t)
    lv_obj_t *static_main_horizon_bar = lv_obj_create(parent_obj_for_reticle);
    // Make the line width proportional to parent_w, e.g., 75% of parent_w
    int32_t bar_width = (parent_w * 3) / 4; // 75% of parent width
    if (bar_width < 20) bar_width = 20; // Minimum width
    lv_obj_set_size(static_main_horizon_bar, bar_width, 2); // Use calculated width, 2px high
    lv_obj_set_style_bg_color(static_main_horizon_bar, STATIC_HORIZON_LINE_COLOR, 0);
    lv_obj_set_style_bg_opa(static_main_horizon_bar, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(static_main_horizon_bar, 0, 0);
    lv_obj_set_style_pad_all(static_main_horizon_bar, 0, 0); // No padding
    lv_obj_align(static_main_horizon_bar, LV_ALIGN_CENTER, 0, 0); // Align this bar to the center


    // Vertical part of the reticle (black lines)
    // These are drawn using coordinates calculated from the parent's center.
    int32_t calculated_center_x = parent_w / 2;
    int32_t calculated_center_y = parent_h / 2;

    lv_obj_t *reticle_v_line_top = lv_line_create(parent_obj_for_reticle);
    lv_point_t v_points_top[] = {{calculated_center_x, calculated_center_y - 10}, {calculated_center_x, calculated_center_y - 2}}; 
    lv_line_set_points(reticle_v_line_top, v_points_top, 2);
    lv_obj_set_style_line_color(reticle_v_line_top, RETICLE_COLOR, 0); 
    lv_obj_set_style_line_width(reticle_v_line_top, 2, 0);
    lv_obj_set_style_line_opa(reticle_v_line_top, LV_OPA_COVER, 0);

    lv_obj_t *reticle_v_line_bottom = lv_line_create(parent_obj_for_reticle);
    lv_point_t v_points_bottom[] = {{calculated_center_x, calculated_center_y + 2}, {calculated_center_x, calculated_center_y + 10}}; 
    lv_line_set_points(reticle_v_line_bottom, v_points_bottom, 2);
    lv_obj_set_style_line_color(reticle_v_line_bottom, RETICLE_COLOR, 0); 
    lv_obj_set_style_line_width(reticle_v_line_bottom, 2, 0);
    lv_obj_set_style_line_opa(reticle_v_line_bottom, LV_OPA_COVER, 0);

    fixed_reticle_drawn = true; // Set flag after attempting to draw
}


void create_artificial_horizon_widget(lv_obj_t *parent_obj, ui_ahrs_widget_t *ahrs_widget_data, int w, int h) {
    if (!parent_obj || !ahrs_widget_data) return;

    fixed_reticle_drawn = false; // Reset flag when widget is created/re-created

    if (w <= 0 || h <= 0) { 
        // LV_LOG_WARN("AHRS create: Invalid dimensions w=%d, h=%d. Widget not created.", w, h);
        return; 
    }

    size_t buf_size = (size_t)w * h * (LV_COLOR_DEPTH / 8);
    if (buf_size == 0) {
        // LV_LOG_ERROR("AHRS create: buf_size is 0. Widget not created.");
        return;
    }
    ahrs_widget_data->cbuf = (lv_color_t *)malloc(buf_size); 

    if (!ahrs_widget_data->cbuf) {
        // LV_LOG_ERROR("AHRS create: cbuf malloc failed. Widget not created.");
        return;
    }

    ahrs_widget_data->canvas = lv_canvas_create(parent_obj);
    lv_canvas_set_buffer(ahrs_widget_data->canvas, ahrs_widget_data->cbuf, w, h, LV_IMG_CF_TRUE_COLOR);
    lv_obj_set_size(ahrs_widget_data->canvas, w, h); 
    lv_obj_align(ahrs_widget_data->canvas, LV_ALIGN_CENTER, 0, 0); 

    draw_artificial_horizon(ahrs_widget_data->canvas, 0.0f, 0.0f); 
    
    draw_fixed_reticle(parent_obj, w, h); 

    ahrs_widget_data->pitch_label = lv_label_create(parent_obj); 
    lv_label_set_text(ahrs_widget_data->pitch_label, "P: ---");
    lv_obj_set_style_text_color(ahrs_widget_data->pitch_label, lv_color_white(), 0);
    lv_obj_set_width(ahrs_widget_data->pitch_label, LV_SIZE_CONTENT); 
    lv_obj_align(ahrs_widget_data->pitch_label, LV_ALIGN_BOTTOM_LEFT, 5, -5); 

    ahrs_widget_data->roll_label = lv_label_create(parent_obj); 
    lv_label_set_text(ahrs_widget_data->roll_label, "R: ---");
    lv_obj_set_style_text_color(ahrs_widget_data->roll_label, lv_color_white(), 0);
    lv_obj_set_width(ahrs_widget_data->roll_label, 55); 
    lv_obj_set_style_text_align(ahrs_widget_data->roll_label, LV_TEXT_ALIGN_LEFT, 0); 
    // X offset changed back to -5 to give 5px padding from the right edge
    lv_obj_align(ahrs_widget_data->roll_label, LV_ALIGN_BOTTOM_RIGHT, 15, -5); 
}

void update_artificial_horizon_widget(ui_ahrs_widget_t *ahrs_widget_data) {
    if (!ahrs_widget_data || !ahrs_widget_data->canvas) return;

    float roll = get_qmi8658_roll_degrees();
    float pitch = get_qmi8658_pitch_degrees();

    draw_artificial_horizon(ahrs_widget_data->canvas, pitch, roll);

    if (ahrs_widget_data->pitch_label) {
        char buf[16];
        sprintf(buf, "P: %.0f", pitch); 
        lv_label_set_text(ahrs_widget_data->pitch_label, buf);
    }
    if (ahrs_widget_data->roll_label) {
        char buf[16];
        sprintf(buf, "R: %.0f", roll);
        lv_label_set_text(ahrs_widget_data->roll_label, buf);
    }
}

void destroy_artificial_horizon_widget(ui_ahrs_widget_t *ahrs_widget_data) {
    if (ahrs_widget_data) {
        if (ahrs_widget_data->cbuf) {
            free(ahrs_widget_data->cbuf);
            ahrs_widget_data->cbuf = NULL;
        }
        ahrs_widget_data->canvas = NULL; 
        ahrs_widget_data->pitch_label = NULL;
        ahrs_widget_data->roll_label = NULL;
    }
    fixed_reticle_drawn = false; 
}
