SET(SOURCES ui_comp_page4.c
    ui_comp.c
    ui_Options_Page.c
    ui_Bubble_Level_Page.c
    ui_Artificial_Horizon_Page.c
    ui_Shot_Counter_Page.c
    ui_Stage_Timer_Page.c
    ui_Competition_Page.c
    ui.c
    ui_comp_hook.c
    ui_helpers.c)

add_library(ui ${SOURCES})
