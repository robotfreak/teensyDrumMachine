/* Encoder Library - TwoKnobs Example
 * http://www.pjrc.com/teensy/td_libs_Encoder.html
 *
 * This example code is in the public domain.
 */

#include <Encoder_t3.h>

// Change these pin numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder enc1(24, 25);
Encoder enc2(26, 27);
Encoder enc3(28, 29);
Encoder enc4(30, 31);
//   avoid using pins with LEDs attached

void setup() {
  Serial.begin(9600);
  Serial.println("TwoKnobs Encoder Test:");
}

long pos1  = -999;
long pos2  = -999;
long pos3  = -999;
long pos4  = -999;

void loop() {
  long newEnc1, newEnc2, newEnc3, newEnc4;
  newEnc1 = enc1.read();
  newEnc2 = enc2.read();
  newEnc3 = enc3.read();
  newEnc4 = enc4.read();
  if (newEnc1 != pos1 || newEnc2 != pos2 || newEnc3 != pos3 || newEnc4 != pos4 ) {
    Serial.print("Encoder = ");
    Serial.print(newEnc1);
    Serial.print(", ");
    Serial.print(newEnc2);
    Serial.print(", ");
    Serial.print(newEnc3);
    Serial.print(", ");
    Serial.print(newEnc4);
    Serial.println();
    pos1 = newEnc1;
    pos2 = newEnc2;
    pos3 = newEnc3;
    pos4 = newEnc4;
  }
  // if a character is sent from the serial monitor,
  // reset both back to zero.
  if (Serial.available()) {
    Serial.read();
    Serial.println("Reset all knobs to zero");
    enc1.write(0);
    enc2.write(0);
    enc3.write(0);
    enc4.write(0);
  }
}
