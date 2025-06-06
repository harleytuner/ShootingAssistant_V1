// In ui_events.c
#include "ui.h" // Or your specific screen headers if preferred

// Event handler for the Home button on Page 2 (Artificial Horizon Page)
void ui_event_Page2HomeBTN(lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);
    // lv_obj_t * target = lv_event_get_target(e); // Useful if you need the button object

    if(event_code == LV_EVENT_PRESSED) {
        // This is where you put the action for the Home button
        // For example, to go to the Options Page:
        // Replace 'ui_Options_Page' with the correct variable name for your Options screen object
        // Replace 'ui_Options_Page_screen_init' if your init function has a different name
        _ui_screen_change(&ui_Options_Page, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, &ui_Options_Page_screen_init);
        // LV_LOG_USER("PRESSED Page 2 Home Button"); // For debugging
    }
}

// Event handler for the Previous button on Page 2
void ui_event_Page2PrevBTN(lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);
    if(event_code == LV_EVENT_PRESSED) {
        // Action for the Previous button
        // For example, to go to the Bubble Level Page:
        _ui_screen_change(&ui_Bubble_Level_Page, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 0, &ui_Bubble_Level_Page_screen_init);
        // LV_LOG_USER("PRESSED Page 2 Previous Button");
    }
}

// Event handler for the Next button on Page 2
// (Your header has ui_Page2NextBTN2 for the object and implies this event handler name)
void ui_event_Page2NextBTN2(lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);
    if(event_code == LV_EVENT_PRESSED) {
        // Action for the Next button
        // For example, to go to Page 4 (Shot Counter Page, based on your ui.h includes):
        _ui_screen_change(&ui_Shot_Counter_Page, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0, &ui_Shot_Counter_Page_screen_init);
        // LV_LOG_USER("PRESSED Page 2 Next Button");
    }
}