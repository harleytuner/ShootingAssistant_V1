// In ui_artificial_horizon_widget.h

#ifndef UI_ARTIFICIAL_HORIZON_WIDGET_H
#define UI_ARTIFICIAL_HORIZON_WIDGET_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

// Declare the C-callable functions to get sensor data from the .ino sketch
float get_qmi8658_roll_degrees();
float get_qmi8658_pitch_degrees();

// Structure to hold canvas, buffer, and new labels
typedef struct {
    lv_obj_t* canvas;
    lv_color_t* cbuf;
    lv_obj_t* fixed_reticle_container; // Optional: if you want to group reticle parts
    lv_obj_t* pitch_label;             // <<< ADD THIS LINE
    lv_obj_t* roll_label;              // <<< ADD THIS LINE
} ui_ahrs_widget_t;


// Call this in ui_Artificial_Horizon_Page_screen_init()
void create_artificial_horizon_widget(lv_obj_t *parent_obj, ui_ahrs_widget_t *ahrs_widget_data, int w, int h);

// Call this periodically to update the display
void update_artificial_horizon_widget(ui_ahrs_widget_t *ahrs_widget_data);

// Call this when the screen is destroyed to free memory
void destroy_artificial_horizon_widget(ui_ahrs_widget_t *ahrs_widget_data);


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_ARTIFICIAL_HORIZON_WIDGET_H*/