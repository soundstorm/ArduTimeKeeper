/*
 * Pitch time keeping sketch for countdown of 32s
 * Created by Luca Zimmermann
 * hannio.org
 * Used e.g. at Barcamp Hannover
 */

#include "LedControl.h"

/*
  Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
  pin 12 is connected to the DataIn
  pin 11 is connected to the CLK
  pin 10 is connected to LOAD
  We have only a single MAX72XX.
*/
LedControl lc = LedControl(12, 11, 10, 4);

volatile bool doRst = false;

void setup() {
  pinMode(2, INPUT_PULLUP);
  /*
    The MAX72XX is in power-saving mode on startup,
    we have to do a wakeup call
  */
  uint8_t devices = lc.getDeviceCount();
  for (uint8_t i = 0; i < devices; i++) {
    lc.shutdown(i, false);
    /* Set the brightness to a medium values */
    lc.setIntensity(i, 8);
    /* and clear the display */
    lc.clearDisplay(i);
  }
  attachInterrupt(digitalPinToInterrupt(2), rst, RISING);
}

void rst() {
  doRst = true;
}

// Delay time or abort if button pushed
void del(uint16_t tp) {
  uint32_t t = millis() + tp;
  while (!doRst && t > millis());
}

void writeArduinoOnMatrix() {
  // Count down all segments if button not pressed again
  for (uint8_t i = 0; i < 32 && !doRst; i++) {
    lc.setColumn(3 - i / 8, i % 8, 0);
    // Count down until the last 4 segments
    if (i < 28) {
      del(1000);
    } else {
      // Blink
      del(200);
      lc.setColumn(3 - i / 8, i % 8, 0xFF);
      del(200);
      lc.setColumn(3 - i / 8, i % 8, 0);
      del(200);
      lc.setColumn(3 - i / 8, i % 8, 0xFF);
      del(200);
      lc.setColumn(3 - i / 8, i % 8, 0);
      del(200);
    }
  }
}

void loop() {
  // Push button to start
  while (!doRst);
  doRst = false;
  for (uint8_t i = 0; i < 32; i++) {
    lc.setColumn(i / 8, 7 - i % 8, 0xFF);
  }
  writeArduinoOnMatrix();
}
