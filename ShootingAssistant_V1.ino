// ... (keep all your existing includes and initializations) ...
#include "Display_ST7789.h"
// #include "Audio_PCM5101.h"
// #include "RTC_PCF85063.h"
#include "Gyro_QMI8658.h" // Your QMI8658 library
#include "LVGL_Driver.h"
// #include "PWR_Key.h"
// #include "SD_Card.h"
// #include "LVGL_Example.h"
// #include "BAT_Driver.h"
// #include "Wireless.h"
#include "ui.h" // Your main UI header from SquareLine Studio
                // This should make ui_competition_page_handle_recoil_event() available

#include <math.h> // For atan2f, sqrtf and M_PI

// --- Calibration Offsets ---
float g_roll_calibration_offset = -5.0f;  // <<<< SET YOUR OBSERVED ROLL OFFSET HERE
float g_pitch_calibration_offset = 0.0f; // <<<< SET YOUR OBSERVED PITCH OFFSET HERE (barrel level)

// --- Recoil Detection Parameters ---
// IMPORTANT: RECOIL_Z_AXIS_THRESHOLD_NEGATIVE assumes Z-axis points FORWARD along the barrel.
// Recoil causes a sharp NEGATIVE acceleration on Z. Tune this value carefully!
// Start with a value like -2.0 or -2.5 and adjust based on testing.
const float RECOIL_Z_AXIS_THRESHOLD_NEGATIVE = -1.0f; // Example: -1.0G, adjust as needed
const unsigned long RECOIL_COOLDOWN_MS = 1500;       // Cooldown period in milliseconds (e.g., 1.5 seconds)
unsigned long last_recoil_detection_time = 0;        // Timestamp of the last recoil detection

// Forward declarations for C++ functions
float get_roll_from_sensor_impl();
float get_pitch_from_sensor_impl();
bool checkForRecoil();

// C-compatible wrapper functions for LVGL
extern "C" float get_qmi8658_roll_degrees() {
    return get_roll_from_sensor_impl();
}

extern "C" float get_qmi8658_pitch_degrees() {
    return get_pitch_from_sensor_impl();
}

// Note: ui_competition_page_handle_recoil_event() and ui_external_decrement_shot_count()
// should be declared in your UI header files (e.g., ui_Competition_Page.h, ui_Shot_Counter_Page.h)
// and made available through ui.h.


void setup()
{
    Serial.begin(115200);
    Serial.println("Setup: Starting system initialization...");

    Serial.print("Setup: Using roll calibration offset: "); Serial.println(g_roll_calibration_offset);
    Serial.print("Setup: Using pitch calibration offset: "); Serial.println(g_pitch_calibration_offset);

    I2C_Init();
    Serial.println("Setup: I2C Initialized");

    Backlight_Init();
    Serial.println("Setup: Backlight Initialized.");

    LCD_Init();
    Serial.println("Setup: LCD Initialized.");

    Lvgl_Init();
    Serial.println("Setup: LVGL Initialized.");

    QMI8658_Init();
    Serial.println("Setup: QMI8658_Init called.");

    setAccScale(ACC_RANGE_4G); // Or your preferred range. Ensure this range is suitable for recoil G's.
                                // If recoil exceeds 4G, you might need ACC_RANGE_8G or higher,
                                // and adjust RECOIL_Z_AXIS_THRESHOLD_NEGATIVE accordingly.
    Serial.println("Setup: QMI8658 Accelerometer scale set.");

    ui_init();
    Serial.println("Setup: ui_init (LVGL UI) called.");

    Serial.println("Setup: System initialization complete.");
}

// C++ function for roll calculation
float get_roll_from_sensor_impl() {
    float acc_y = getAccY();
    float acc_x = getAccX();
    // float acc_z = getAccZ(); // Not strictly needed for this roll definition

    // User's existing roll formula
    float raw_roll_radians = atan2f(-acc_y, -acc_x);
    float raw_roll_degrees = raw_roll_radians * (180.0f / M_PI);
    float calibrated_roll_degrees = raw_roll_degrees - g_roll_calibration_offset;

    return calibrated_roll_degrees;
}

// C++ function for pitch calculation
float get_pitch_from_sensor_impl() {
    float acc_x = getAccX();
    float acc_y = getAccY();
    float acc_z = getAccZ(); // Z-axis accelerometer reading

    // Pitch calculation: angle of Z-axis relative to gravity vector in XY plane
    // Assumes Z-axis of sensor points along the rifle barrel
    float raw_pitch_radians = atan2f(acc_z, sqrtf(acc_x * acc_x + acc_y * acc_y));
    float raw_pitch_degrees = raw_pitch_radians * (180.0f / M_PI);
    
    float calibrated_pitch_degrees = raw_pitch_degrees - g_pitch_calibration_offset;

    return calibrated_pitch_degrees;
}

// --- Recoil Detection Function ---
bool checkForRecoil() {
    // Ensure QMI8658_Loop() in the main loop keeps sensor data fresh,
    // or call an explicit sensor read function here if needed.
    float az = getAccZ(); // Read Z-axis acceleration

    // Debugging: Print Z acceleration to help tune the threshold
    // Comment this out for normal operation to avoid flooding serial
    // Serial.print("Current az: "); Serial.println(az); 

    // Check if Z acceleration exceeds the negative threshold (assuming Z points forward along barrel)
    if (az < RECOIL_Z_AXIS_THRESHOLD_NEGATIVE) {
        // Serial.print("checkForRecoil: Recoil detected! Z-acceleration: "); Serial.println(az); // Debug
        return true;
    }
    return false;
}

void loop()
{
    QMI8658_Loop(); // Make sure this reads new sensor data if it's not automatic

    // --- Recoil Detection Logic ---
    unsigned long current_time = millis();
    if (current_time - last_recoil_detection_time > RECOIL_COOLDOWN_MS) { // Check if cooldown has passed
        if (checkForRecoil()) {
            Serial.println("Main.ino: Physical recoil DETECTED!");

            // Call the Competition Page's recoil handler.
            // This function internally checks if the timer is running.
            ui_competition_page_handle_recoil_event();
            
            // If you also want the Shot Counter Page to react to recoil when it's active,
            // you could add a check here:
            // if (lv_scr_act() == ui_Shot_Counter_Page) {
            //    ui_external_decrement_shot_count(); // This is for the Shot_Counter_Page itself
            // }
            // However, ui_competition_page_handle_recoil_event() already handles the competition scenario.
            // ui_external_decrement_shot_count() is primarily for when the Shot Counter Page is active and you want to test shots there.

            last_recoil_detection_time = current_time; // Update timestamp for cooldown
            // The printf calls inside ui_competition_page_handle_recoil_event will provide more details.
        }
    }
    // --- End of Recoil Detection Logic ---

    Lvgl_Loop(); // LVGL's main loop handler
    delay(5);    // Keep your LVGL loop responsive
}
