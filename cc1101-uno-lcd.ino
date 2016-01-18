#include "EEPROM.h"
#include "cc1101.h"    //cc1101.flushTxFifo ();   // NOT SURE if ever NEEDED?
#include <Wire.h>  // I2C arduino library ( comes with arduino )
#include <LiquidCrystal_I2C.h>

// set the LCD address to 0x27 for a 20 chars 4 line display
// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

CC1101 cc1101;

// setup the LED & Button
uint8_t buttonPin = 3;
uint8_t ledPin = 7;
uint8_t buttonState = 0;

// counter to get increment in each loop
uint8_t counter;
uint8_t b;
uint8_t syncWord = 0x6d6d;   // sender and reciever must have same!!! like ip address?
// '0b0110110101101101'

//LCD SCREEN SETUP

// SETUP HERE
void setup()
{
  // LED & Button
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  Serial.begin(115200);
  lcd.begin(20, 4);  // initialize the lcd for 20 chars 4 lines, turn on backlight
  // reset the counter
  counter = 253;

  // initialize the RF Chip
  lcd.print("initializing...");
  cc1101.init();
  cc1101.setSyncWord(&syncWord, false);
  cc1101.setCarrierFreq(CFREQ_433);
  cc1101.disableAddressCheck();
  //cc1101.setTxPowerAmp(PA_LongDistance);  // if you want HIGH POWER
  cc1101.setTxPowerAmp(PA_LowPower);
  delay(500);
  lcd.clear();
}
/// end void setup

// send_data() becomes a function to be used in void loop()

void send_data()
{

  cc1101.flushTxFifo ();  // is this needed???

  CCPACKET data;
  data.length = 2;
  uint8_t fixed = '\xce';
  uint8_t count = counter++;

  data.data[0] = fixed;
  data.data[1] = count;

  if (cc1101.sendData(data)) {

    delay(100);
    
    /*
    Serial.println(B0);
    Serial.print("data[0] = ");
    Serial.println(data.data[0], BIN);
    Serial.print("data[1] = ");
    Serial.println(data.data[1], DEC);
    Serial.print("\n");
    */
    
    lcd.setCursor(0, 0);
    lcd.print("Binary: ");
    lcd.setCursor(9, 0);
    lcd.print(count, BIN);

    lcd.setCursor(0, 1);
    lcd.print("Decimal:");
    lcd.setCursor(9, 1);
    lcd.print(count, DEC);


    lcd.setCursor(0, 2);
    lcd.print("Hex    :");
    lcd.setCursor(9, 2);
    lcd.print(count, HEX);

    lcd.blink();
    //lcd.scrollDisplayRight();

  } else {
    lcd.setCursor(0, 3);
    lcd.print("  sent failed :(");
    delay(200);
    lcd.setCursor(0, 3);
    lcd.print("                 ");
  }
}

// send data ( does this send all the data in state machine?? )

void loop()
{
  buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH) {
    digitalWrite(ledPin, HIGH);
    if (counter == 0){
      lcd.setCursor(9, 0);
      lcd.print("           ");
      lcd.setCursor(9, 1);
      lcd.print("           ");
      lcd.setCursor(9, 2);
      lcd.print("           ");
      lcd.setCursor(9, 3);
      lcd.print("           ");
      send_data();
    } else {
      send_data();
    }
  } else {
    digitalWrite(ledPin, LOW);
  }

  delay(100);
}
