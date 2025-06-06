// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.5.2
// LVGL version: 8.3.11
// Project name: SquareLine_Project

#include "ui.h" // This should include lvgl.h and also transitively
                // ui_Shot_Counter_Page.h and ui_Stage_Timer_Page.h
                // If not, add explicit includes:
#include "ui_Shot_Counter_Page.h" // For shot counter functions
#include "ui_Stage_Timer_Page.h"  // For timer status functions
#include <stdio.h>                // For printf

// --- Global UI object pointers for Competition Page ---
lv_obj_t * ui_Competition_Page;
lv_obj_t * ui_CompPageLabel; // Original Title

// Original Navigation buttons
lv_obj_t * ui_Page5HomeBTN;
lv_obj_t * ui_Page5HomeBTN1;
lv_obj_t * ui_Page5PrevBTN;
lv_obj_t * ui_Page5PrevBTN1;
lv_obj_t * ui_Page5NextBTN;
lv_obj_t * ui_Page5NextBTN1;

// New UI Elements
lv_obj_t * ui_CompPageTimeStaticLabel;
lv_obj_t * ui_CompPageTimeValueLabel;
lv_obj_t * ui_CompPageShotsStaticLabel;
lv_obj_t * ui_CompPageShotsValueLabel;
lv_obj_t * ui_CompPageStartButton;
lv_obj_t * ui_CompPageStartButtonLabel;
lv_obj_t * ui_CompPageResetButton;
lv_obj_t * ui_CompPageResetButtonLabel;

// LVGL Timer for live data updates on this page
static lv_timer_t * competition_page_data_refresh_timer = NULL;

// Define a default background color (e.g. black for a dark theme, or use theme default)
static lv_color_t default_competition_page_bgcolor; 

// --- Forward declarations for static functions ---
static void competition_page_refresh_timer_cb(lv_timer_t * timer);
static void competition_page_visibility_event_cb(lv_event_t * e); // For managing timer

// --- Public function to update displayed data (callable) ---
void ui_Competition_Page_update_live_data_display(void) {
    int32_t time_sec = 0; // Initialize to ensure it has a value

    // Update Time Display
    if (ui_CompPageTimeValueLabel) {
        time_sec = ui_stage_timer_get_time_remaining_seconds();
        if (time_sec < 0) time_sec = 0; // Ensure non-negative display
        uint32_t minutes = (uint32_t)time_sec / 60;
        uint32_t seconds = (uint32_t)time_sec % 60;
        char buf[8]; // "MM:SS\0"
        lv_snprintf(buf, sizeof(buf), "%02u:%02u", minutes, seconds);
        lv_label_set_text(ui_CompPageTimeValueLabel, buf);
    }

    // Update Shots Display
    if (ui_CompPageShotsValueLabel) {
        int shots = ui_shot_counter_get_shots_remaining();
        // Debug printf to see what value is being fetched for shots
        // printf("ui_Competition_Page_update_live_data_display: Fetched shots: %d\n", shots);
        char buf[4]; // Max "99\0" or "-9\0" for typical shot counts
        lv_snprintf(buf, sizeof(buf), "%d", shots);
        lv_label_set_text(ui_CompPageShotsValueLabel, buf);
    }

    // Update Start/Pause/Resume Button Label
    if (ui_CompPageStartButtonLabel) {
        if (ui_stage_timer_is_timer_running()) {
            lv_label_set_text(ui_CompPageStartButtonLabel, "Pause");
        } else {
            // If time has run down but not to zero from a paused state
            if (time_sec > 0 && time_sec < ui_stage_timer_get_current_set_time_seconds()) {
                lv_label_set_text(ui_CompPageStartButtonLabel, "Resume");
            } else { // Timer is at 0, or at set time and not started
                lv_label_set_text(ui_CompPageStartButtonLabel, "Start");
            }
        }
    }

    // Update background color based on time remaining
    if (ui_Competition_Page) { // Ensure the page object exists
        if (time_sec > 0 && time_sec <= 30) {
            lv_obj_set_style_bg_color(ui_Competition_Page, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
        } else {
            // Only change back to default if it's not already the default
            // This check helps avoid unnecessary redraws if default_competition_page_bgcolor hasn't been set yet
            // or if the color is already default.
            if (lv_obj_is_valid(ui_Competition_Page) && default_competition_page_bgcolor.full != 0 ) { // Check if default was captured
                 if (lv_obj_get_style_bg_color(ui_Competition_Page, LV_PART_MAIN).full != default_competition_page_bgcolor.full) {
                    lv_obj_set_style_bg_color(ui_Competition_Page, default_competition_page_bgcolor, LV_PART_MAIN | LV_STATE_DEFAULT);
                 }
            } else if (lv_obj_is_valid(ui_Competition_Page)) { // Fallback if default not captured (e.g. first run before load event)
                 lv_obj_set_style_bg_color(ui_Competition_Page, lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT); // Default to black
            }
        }
    }
}


// --- Event functions for original navigation buttons ---
void ui_event_Page5HomeBTN(lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);
    if(event_code == LV_EVENT_PRESSED   ) {
        _ui_screen_change(&ui_Options_Page, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 500, 0, &ui_Options_Page_screen_init);
    }
}

void ui_event_Page5PrevBTN(lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);
    if(event_code == LV_EVENT_PRESSED   ) {
        _ui_screen_change(&ui_Stage_Timer_Page, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 500, 0, &ui_Stage_Timer_Page_screen_init);
    }
}

void ui_event_Page5NextBTN(lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);
    if(event_code == LV_EVENT_PRESSED   ) {
        _ui_screen_change(&ui_Options_Page, LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 0, &ui_Options_Page_screen_init);
    }
}

// --- Event functions for new Competition Page buttons ---
void ui_event_CompPageStartButton(lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);
    if(event_code == LV_EVENT_PRESSED   ) {
        if (ui_stage_timer_is_timer_running()) {
            ui_stage_timer_pause_timer_action();
        } else {
            if (ui_stage_timer_get_time_remaining_seconds() > 0) {
                 ui_stage_timer_start_timer_action();
            }
        }
        ui_Competition_Page_update_live_data_display(); 
    }
}

// In ui_Competition_Page.c

// ... (other functions and includes) ...

void ui_event_CompPageResetButton(lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);
    if(event_code == LV_EVENT_PRESSED   ) {
        printf("ui_Competition_Page: Reset button pressed.\n");
        ui_stage_timer_reset_timer_to_set_time_action(); // Resets time correctly.

        // --- CORRECTED SHOT COUNTER RESET ---
        ui_shot_counter_reset_to_set_count(); // Use the correct reset function
        // --- END OF CORRECTION ---

        printf("ui_Competition_Page: After reset - Shots remaining (getter): %d, Current set (getter): %d\n",
               ui_shot_counter_get_shots_remaining(), ui_shot_counter_get_current_set_shot_count());
        ui_Competition_Page_update_live_data_display();
    }
}

// ... (rest of ui_Competition_Page.c) ...

// --- LVGL Timer Callback for Competition Page live updates ---
static void competition_page_refresh_timer_cb(lv_timer_t * timer) {
    LV_UNUSED(timer);
    ui_Competition_Page_update_live_data_display();
}

// --- Event handler for screen load/unload to manage the refresh timer ---
static void competition_page_visibility_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * screen = lv_event_get_target(e); 

    if (screen == ui_Competition_Page) { 
        if (code == LV_EVENT_SCREEN_LOADED) {
            printf("ui_Competition_Page: SCREEN_LOADED event.\n"); 
            if (competition_page_data_refresh_timer) {
                lv_timer_resume(competition_page_data_refresh_timer);
            } else {
                competition_page_data_refresh_timer = lv_timer_create(competition_page_refresh_timer_cb, 330, NULL); 
            }
            if(ui_Competition_Page) { // Capture default bg color after page object is surely created
                default_competition_page_bgcolor = lv_obj_get_style_bg_color(ui_Competition_Page, LV_PART_MAIN);
                printf("ui_Competition_Page: Default BG color captured: #%06X (R:%d G:%d B:%d)\n",
                   default_competition_page_bgcolor.full,
                   default_competition_page_bgcolor.ch.red,
                   default_competition_page_bgcolor.ch.green,
                   default_competition_page_bgcolor.ch.blue);
            }
            ui_Competition_Page_update_live_data_display(); 
        } else if (code == LV_EVENT_SCREEN_UNLOADED) {
            printf("ui_Competition_Page: SCREEN_UNLOADED event.\n"); 
            if (competition_page_data_refresh_timer) {
                lv_timer_pause(competition_page_data_refresh_timer);
            }
            if(ui_Competition_Page && default_competition_page_bgcolor.full != 0) { // Check if default was captured
                lv_obj_set_style_bg_color(ui_Competition_Page, default_competition_page_bgcolor, LV_PART_MAIN | LV_STATE_DEFAULT);
            } else if (ui_Competition_Page) { // Fallback
                 lv_obj_set_style_bg_color(ui_Competition_Page, lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
            }
        }
    }
}

// --- New function to handle recoil event, called externally ---
void ui_competition_page_handle_recoil_event(void) {
    printf("ui_competition_page_handle_recoil_event: Called.\n");

    bool timer_is_active = ui_stage_timer_is_timer_running();
    printf("ui_competition_page_handle_recoil_event: Timer running status: %s\n", timer_is_active ? "true" : "false");

    if (timer_is_active) {
        printf("ui_competition_page_handle_recoil_event: Timer is active. Current shots before decrement: %d\n", ui_shot_counter_get_shots_remaining());
        ui_shot_counter_decrement_shot();          
        printf("ui_competition_page_handle_recoil_event: After decrement. New shots: %d\n", ui_shot_counter_get_shots_remaining());
        ui_Competition_Page_update_live_data_display(); 
        printf("ui_competition_page_handle_recoil_event: Display updated.\n");
    } else {
        printf("ui_competition_page_handle_recoil_event: Timer not running, shot not decremented.\n");
    }
}


// --- Build function for Competition Page screen ---
void ui_Competition_Page_screen_init(void) {
    ui_Competition_Page = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Competition_Page, LV_OBJ_FLAG_SCROLLABLE);
    // Initialize default_competition_page_bgcolor.full to 0 or some indicator it's not set.
    // It will be properly captured in LV_EVENT_SCREEN_LOADED.
    // Setting a default here could be overwritten by the theme if not careful.
    default_competition_page_bgcolor.full = 0; // Indicate not yet captured


    ui_CompPageLabel = lv_label_create(ui_Competition_Page);
    lv_obj_set_width(ui_CompPageLabel, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_CompPageLabel, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_CompPageLabel, 0);
    lv_obj_set_y(ui_CompPageLabel, -140);
    lv_obj_set_align(ui_CompPageLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_CompPageLabel, "Competition Live");

    ui_CompPageTimeStaticLabel = lv_label_create(ui_Competition_Page);
    lv_label_set_text(ui_CompPageTimeStaticLabel, "Time:");
    lv_obj_set_x(ui_CompPageTimeStaticLabel, -80);
    lv_obj_set_y(ui_CompPageTimeStaticLabel, -75);
    lv_obj_set_align(ui_CompPageTimeStaticLabel, LV_ALIGN_CENTER);

    ui_CompPageTimeValueLabel = lv_label_create(ui_Competition_Page);
    lv_obj_set_width(ui_CompPageTimeValueLabel, 120);
    lv_obj_align_to(ui_CompPageTimeValueLabel, ui_CompPageTimeStaticLabel, LV_ALIGN_OUT_RIGHT_MID, 15, -5);
    lv_obj_set_style_text_font(ui_CompPageTimeValueLabel, &lv_font_montserrat_36, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(ui_CompPageTimeValueLabel, "00:00");

    ui_CompPageShotsStaticLabel = lv_label_create(ui_Competition_Page);
    lv_label_set_text(ui_CompPageShotsStaticLabel, "Shots:");
    lv_obj_set_x(ui_CompPageShotsStaticLabel, -80);
    lv_obj_set_y(ui_CompPageShotsStaticLabel, -25);
    lv_obj_set_align(ui_CompPageShotsStaticLabel, LV_ALIGN_CENTER);

    ui_CompPageShotsValueLabel = lv_label_create(ui_Competition_Page);
    lv_obj_set_width(ui_CompPageShotsValueLabel, 80);
    lv_obj_align_to(ui_CompPageShotsValueLabel, ui_CompPageShotsStaticLabel, LV_ALIGN_OUT_RIGHT_MID, 15, -5);
    lv_obj_set_style_text_font(ui_CompPageShotsValueLabel, &lv_font_montserrat_36, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(ui_CompPageShotsValueLabel, "0");

    ui_CompPageStartButton = lv_btn_create(ui_Competition_Page);
    lv_obj_set_width(ui_CompPageStartButton, 90);
    lv_obj_set_height(ui_CompPageStartButton, 40);
    lv_obj_set_x(ui_CompPageStartButton, -55);
    lv_obj_set_y(ui_CompPageStartButton, 80);
    lv_obj_set_align(ui_CompPageStartButton, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_CompPageStartButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_CompPageStartButton, LV_OBJ_FLAG_SCROLLABLE);

    ui_CompPageStartButtonLabel = lv_label_create(ui_CompPageStartButton);
    lv_obj_set_align(ui_CompPageStartButtonLabel, LV_ALIGN_CENTER);

    ui_CompPageResetButton = lv_btn_create(ui_Competition_Page);
    lv_obj_set_width(ui_CompPageResetButton, 90);
    lv_obj_set_height(ui_CompPageResetButton, 40);
    lv_obj_set_x(ui_CompPageResetButton, 55);
    lv_obj_set_y(ui_CompPageResetButton, 80);
    lv_obj_set_align(ui_CompPageResetButton, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_CompPageResetButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_CompPageResetButton, LV_OBJ_FLAG_SCROLLABLE);

    ui_CompPageResetButtonLabel = lv_label_create(ui_CompPageResetButton);
    lv_label_set_text(ui_CompPageResetButtonLabel, "Reset");
    lv_obj_set_align(ui_CompPageResetButtonLabel, LV_ALIGN_CENTER);

    const lv_coord_t nav_btn_y_offset = 125;

    ui_Page5HomeBTN = lv_btn_create(ui_Competition_Page);
    lv_obj_set_width(ui_Page5HomeBTN, 50);
    lv_obj_set_height(ui_Page5HomeBTN, 25);
    lv_obj_set_x(ui_Page5HomeBTN, -60);
    lv_obj_set_y(ui_Page5HomeBTN, nav_btn_y_offset);
    lv_obj_set_align(ui_Page5HomeBTN, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Page5HomeBTN, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Page5HomeBTN, LV_OBJ_FLAG_SCROLLABLE);
    ui_Page5HomeBTN1 = lv_label_create(ui_Page5HomeBTN);
    lv_label_set_text(ui_Page5HomeBTN1, "HOME");
    lv_obj_set_align(ui_Page5HomeBTN1, LV_ALIGN_CENTER);

    ui_Page5PrevBTN = lv_btn_create(ui_Competition_Page);
    lv_obj_set_width(ui_Page5PrevBTN, 50);
    lv_obj_set_height(ui_Page5PrevBTN, 25);
    lv_obj_set_x(ui_Page5PrevBTN, 0);
    lv_obj_set_y(ui_Page5PrevBTN, nav_btn_y_offset);
    lv_obj_set_align(ui_Page5PrevBTN, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Page5PrevBTN, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Page5PrevBTN, LV_OBJ_FLAG_SCROLLABLE);
    ui_Page5PrevBTN1 = lv_label_create(ui_Page5PrevBTN);
    lv_label_set_text(ui_Page5PrevBTN1, "PREV");
    lv_obj_set_align(ui_Page5PrevBTN1, LV_ALIGN_CENTER);

    ui_Page5NextBTN = lv_btn_create(ui_Competition_Page);
    lv_obj_set_width(ui_Page5NextBTN, 50);
    lv_obj_set_height(ui_Page5NextBTN, 25);
    lv_obj_set_x(ui_Page5NextBTN, 60);
    lv_obj_set_y(ui_Page5NextBTN, nav_btn_y_offset);
    lv_obj_set_align(ui_Page5NextBTN, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Page5NextBTN, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Page5NextBTN, LV_OBJ_FLAG_SCROLLABLE);
    ui_Page5NextBTN1 = lv_label_create(ui_Page5NextBTN);
    lv_label_set_text(ui_Page5NextBTN1, "NEXT");
    lv_obj_set_align(ui_Page5NextBTN1, LV_ALIGN_CENTER);

    lv_obj_add_event_cb(ui_Page5HomeBTN, ui_event_Page5HomeBTN, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_Page5PrevBTN, ui_event_Page5PrevBTN, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_Page5NextBTN, ui_event_Page5NextBTN, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_CompPageStartButton, ui_event_CompPageStartButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_CompPageResetButton, ui_event_CompPageResetButton, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(ui_Competition_Page, competition_page_visibility_event_cb, LV_EVENT_SCREEN_LOADED, NULL);
    lv_obj_add_event_cb(ui_Competition_Page, competition_page_visibility_event_cb, LV_EVENT_SCREEN_UNLOADED, NULL);

    ui_Competition_Page_update_live_data_display();
}

void ui_Competition_Page_screen_destroy(void) {
    if (competition_page_data_refresh_timer) {
        lv_timer_del(competition_page_data_refresh_timer);
        competition_page_data_refresh_timer = NULL;
    }

    if (ui_Competition_Page) {
        lv_obj_remove_event_cb_with_user_data(ui_Competition_Page, competition_page_visibility_event_cb, NULL);
        lv_obj_del(ui_Competition_Page);
    }

    ui_Competition_Page = NULL;
    ui_CompPageLabel = NULL;
    ui_Page5HomeBTN = NULL;
    ui_Page5HomeBTN1 = NULL;
    ui_Page5PrevBTN = NULL;
    ui_Page5PrevBTN1 = NULL;
    ui_Page5NextBTN = NULL;
    ui_Page5NextBTN1 = NULL;

    ui_CompPageTimeStaticLabel = NULL;
    ui_CompPageTimeValueLabel = NULL;
    ui_CompPageShotsStaticLabel = NULL;
    ui_CompPageShotsValueLabel = NULL;
    ui_CompPageStartButton = NULL;
    ui_CompPageStartButtonLabel = NULL;
    ui_CompPageResetButton = NULL;
    ui_CompPageResetButtonLabel = NULL;
}
