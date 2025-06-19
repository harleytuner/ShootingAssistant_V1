#include "PWR_Key.h"

static uint8_t BAT_State = 0;     // Controls whether we are monitoring the power button (2 = monitoring)
static uint8_t Device_State = 0;  // General state, e.g., 0=normal, 1=sleep pending, 2=restart pending
static uint16_t Long_Press = 0;   // Counter for button hold duration


void PWR_Loop(void)
{
  // We only monitor the power button if the device is considered "on" (BAT_State == 2)
  if(BAT_State == 2){
    // Check if the power button (GPIO 15) is pressed (LOW, due to INPUT_PULLUP)
    if(!digitalRead(PWR_KEY_Input_PIN)){
      Long_Press ++; // Increment counter for how long the button is held
      
      // Check if the long press duration for shutdown has been reached
      if(Long_Press >= Device_Shutdown_Time){
        Shutdown(); // If shutdown threshold is reached, call Shutdown function
        // Note: After Shutdown(), the MCU might power off, so this loop will stop.
        // The 'Fall_Asleep' and 'Restart' conditions are currently not implemented
        // to actually put the device to sleep or restart, but they are here if you
        // wish to add that functionality later.
      }
    }
    else{ // Button is released
      Long_Press = 0; // Reset the long press counter when the button is released
      Device_State = 0; // Clear any intermediate states (like Sleep/Restart flags)
    }
  }
}

void Fall_Asleep(void)
{
  // Implement your sleep logic here.
  // You might need to set PWR_Control_PIN LOW temporarily or enter deep sleep.
  // For deep sleep, ensure a wake-up source (like the power button interrupt) is configured.
  // digitalWrite(PWR_Control_PIN, LOW); // Example: Cut power for sleep
  // esp_deep_sleep_start(); // Example: ESP32 deep sleep
}

void Restart(void)
{
  // Implement your restart logic here.
  // This could be a software reset or a momentary power cut and re-application.
  // esp_restart(); // Example: ESP32 software reset
}

void Shutdown(void)
{
  digitalWrite(PWR_Control_PIN, LOW); // Pull PWR_Control_PIN LOW to unlatch power
  LCD_Backlight = 0; // Turn off backlight
  // Add any other shutdown procedures here (e.g., saving data, unmounting SD)
  // The MCU will likely lose power shortly after this, depending on the latching circuit.
}

void PWR_Init(void) {
  // IMPORTANT: Drive the latch pin HIGH *before* configuring pinMode to prevent a LOW glitch
  digitalWrite(PWR_Control_PIN, HIGH);   // Set output level to HIGH immediately
  pinMode(PWR_Control_PIN, OUTPUT);      // Now set as OUTPUT

  // Configure the power key input pin with internal pull-up resistor.
  pinMode(PWR_KEY_Input_PIN, INPUT_PULLUP);  // BATT PWR button between pin and GND

  // OPTIONAL: Serial debug (remove if not needed)
   //Serial.begin(115200);
   //Serial.println("Power latch set HIGH");

  // Initialize state variables: device is now considered powered on and stable.
  BAT_State = 2;
  Long_Press = 0;
}
