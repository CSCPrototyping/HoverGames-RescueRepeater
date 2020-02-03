/* Hamshield
 * Example: KISS
 * This is a example configures the HamShield to be used as 
 * a TNC/KISS device. You will need a KISS device to input 
 * commands to the HamShield
 * Connect the HamShield to your Arduino. Screw the antenna 
 * into the HamShield RF jack. Connect the Arduino to wall 
 * power and then to your computer via USB. Issue commands 
 * via the KISS equipment.
 * 
 * You can also just use the serial terminal to send and receive
 * APRS packets, but keep in mind that several fields in the packet
 * are bit-shifted from standard ASCII (so if you're receiving,
 * you won't get human readable callsigns or paths).
 *
 * To use the KISS example with YAAC:
 * 1. open the configure YAAC wizard
 * 2. follow the prompts and enter in your details until you get to the "Add and Configure Interfaces" window
 * 3. Choose "Add Serial KISS TNC Port"
 * 4. Choose the COM port for your Arduino
 * 5. set baud rate to 9600 (default)
 * 6. set it to KISS-only: with no command to enter KISS mode (just leave the box empty)
 * 7. Use APRS protocol (default)
 * 8. hit the next button and follow directions to finish configuration
*/
#include <Arduino.h>
#include "HamShield.h"
#include "KISS.h"
#include "DDS.h"
#include "packet.h"
#include <avr/wdt.h>

HamShield radio;
DDS dds;
AFSK afsk;
KISS kiss(&Serial, &radio, &dds, &afsk);

#define MIC_PIN 3
#define RESET_PIN A2 //reset not on HS Mini
#define SWITCH_PIN 2

const byte adcPin = 2; //A2 SHOULD BE DEFINED AS THE ADC INPUT - KB1MHD 29JAN2020

void setup() {
  // NOTE: if not using PWM out, it should be held low to avoid tx noise
  // NOTE: FOR KISS TNC WE USE PWM TO GENERATE TX AUDIO - DO NOT HOLD LOW: KB1MHD 29JAN2020
  //pinMode(MIC_PIN, OUTPUT);
  //digitalWrite(MIC_PIN, LOW);
  
  // prep the switch: Thus appear to have no function
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  
  // set up the reset control pin
  // NOTE: HamShieldMini doesn't have a reset pin, so this has no effect
  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN, HIGH);

  pinMode(MIC_PIN, OUTPUT);
  digitalWrite(MIC_PIN, LOW);  
  
  delay(5); // wait for device to come up
  
  Serial.begin(9600);

  //NOTE: INITIALIZE THESE PARAMETERS NOW, NOT SURE IF THE EXAMPLE CODE EVER DOES THIS?
  //specify MUX pin
  ADMUX = adcPin;
  //NOTE: REFS0=1 ENSURES ADC REFERENCE=VCC, NOT INTERNAL REFERENCE. THIS IS IMPORTANT DUE TO THE RANGE OF INPUT AUDIO AMPLITUDES (SEE radio.setVolume BELOW). TESTING WAS DONE WITH A 
  //5V 328P ARDUINO. 3.3V ARDUINOS MAY REQUIRE SMALLER pk-pk AUDIO LEVELS. THE HAMSHIELD AUDIO OUT TO THE ARDUINO HAS A 1V DC OFFSET, SO 1V+2V pk-pk MAY CAUSE PROBLEMS WHEN SAMPLING
  //FROM A 3.3V REFERENCE. FURTHER REDUCE PWM AMPLITUDE IF REQUIRED - KB1MHD 29JAN2020
  //bit(REFS0); puts 0s in all bits except REFS0 of ADMUX register
  ADMUX = bit(REFS0) | adcPin;
  
  //Query the HamShield for status information
  //NOTE: THIS MAY BE HELPFUL FOR DEBUG, BUT IS COMMENTED OUT FOR NOW - KB1MHD 29JAN2020
  //Serial.print("Radio status: ");
  //int result = radio.testConnection();
  //.println(result,DEC);
  
  radio.initialize();
  radio.setSQOff(); //SQUELCH DISABLED FOR PACKET MODE TO PREVENT POTENTIAL DELAYS ON RX AUDIO - KB1MHD 29JAN2020
  //NOTE: ~2V pk-pk AT SPKR OUT ON HAMSHIELD MINI - KB1MHD 29JAN2020
  radio.setVolume1(0xFF);
  radio.setVolume2(0xFF);
  
  //NOTE: COMMENTED OUT, SQUELCH DISABLED FOR PACKET MODE TO PREVENT POTENTIAL DELAYS ON RX AUDIO - KB1MHD 29JAN2020
  // radio.setSQHiThresh(-100);
  //radio.setSQLoThresh(-100);
  //radio.setSQOn();
  radio.frequency(144390);
  radio.bypassPreDeEmph();
  radio.setRfPower(15);

  //NOTE: INITIALIZE & START DDS OBJECT NOW, NOT SURE IF THIS GETS DONE OTHERWISE - KB1MHD 29JAN2020
  dds.start();
  dds.on();
  //NOTE: VERY IMPORTANT!!! NEED TO TURN THE AMPLITUDE OF THE PWM OUTPUT DOWN TO PREVENT OVERDRIVING THE HAMSHIELD MIC INPUT - KB1MHD 29JAN2020
  dds.setAmplitude(31);
  
  afsk.start(&dds);
  delay(100);
  radio.setModeReceive();
}

void loop() {
  kiss.loop();
}

ISR(TIMER2_OVF_vect) {
  TIFR2 = _BV(TOV2);
  static uint8_t tcnt = 0;
  if(++tcnt == 8) {
    dds.clockTick();
    tcnt = 0;
  }
}

ISR(ADC_vect) {
  static uint8_t tcnt = 0;
  TIFR1 = _BV(ICF1); // Clear the timer flag
  dds.clockTick();
  if(++tcnt == 1) {
    afsk.timer();
    tcnt = 0;
  }
}
