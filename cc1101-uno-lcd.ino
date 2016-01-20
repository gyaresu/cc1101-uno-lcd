#include "EEPROM.h"
#include "cc1101.h"    // minimal-cc1101 Where did this come from?
#include <Wire.h>  // I2C arduino library ( comes with arduino )
#include <LiquidCrystal_I2C.h> // https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/Home

// set the LCD address to 0x27 for a 20 chars 4 line display
// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

CC1101 cc1101;

// LED & Button
uint8_t buttonPin = 3;
uint8_t ledPin = 7;

// button states
uint8_t state = HIGH;
uint8_t reading;
uint8_t previous = LOW;

// the follow variables are  uintmax_t because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
uintmax_t time = 0;
uintmax_t debounce = 300;

// counter to get increment in each loop
uint8_t counter;
uint8_t b;

// SETUP HERE
void setup()
{
  // Assign Button and LED pins
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  
  // SyncWord
  //uint32_t syncWord = 61166; // 0xEEEE
  uint8_t syncH = 0xEE; // 
  uint8_t syncL = 0xEE;
  
  
  Serial.begin(115200);
  lcd.begin(20, 4);  // initialize the lcd for 20 chars 4 lines, turn on backlight
  // reset the counter
  counter = 0;

  // initialize the RF Chip
  /*
  lcd.print("SyncH: 0x");
  lcd.print(syncH, HEX);
  lcd.setCursor(0, 1);
  lcd.print("SyncL: 0x");
  lcd.print(syncL, HEX);
  */
  lcd.setCursor(0, 1);
  lcd.print(syncH, HEX);
  lcd.print(' ');
  lcd.print(syncL, HEX);
  delay(2000);
  cc1101.init();
  //cc1101.setSyncWord(syncH, syncL, false);
  cc1101.setSyncWord(syncH, syncL, false);
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

  cc1101.flushTxFifo ();

  CCPACKET data;
  data.length = 5;
  cc1101.writeReg(CC1101_MDMCFG3, 0x22); // Register 0x11 set to 115200 Baud
  uint8_t count = counter++;

  data.data[0] = '@';
  data.data[1] = 'b';
  data.data[2] = 'c';
  data.data[3] = 'x';
  data.data[4] = 'y';
  

/*
  for (uint8_t i = 0; i < data.length; i++){
    data.data[i] = i * thing[i];
  }
  */
  
  if (cc1101.sendData(data)) {

    delay(100);

    // LED ON
    digitalWrite(ledPin, HIGH);
  
    lcd.setCursor(0, 0);
    lcd.print("Binary :");
    lcd.setCursor(9, 0);
    lcd.print(count, BIN);

    lcd.setCursor(0, 1);
    lcd.print("Decimal:");
    lcd.setCursor(9, 1);
    lcd.print(count, DEC);


    lcd.setCursor(0, 2);
    lcd.print("DR:");
    lcd.setCursor(3, 2);
    lcd.print(cc1101.readReg(0x11, CC1101_STATUS_REGISTER), HEX);
    lcd.print(' ');
    lcd.print("PL:");
    lcd.print(data.length, HEX);
    lcd.setCursor(0, 3);
    lcd.print(data.data[0], HEX);
    lcd.print(' ');
    lcd.print(data.data[1], HEX);
    lcd.print(' ');
    lcd.print(data.data[2], HEX);
    lcd.print(' ');
    lcd.print(data.data[3], HEX);
    lcd.print(' ');
    lcd.print(data.data[4], HEX);

    /*
    for (uint8_t i = 0; i < data.length; i++){
      lcd.print(data.data[i], HEX);
    }*/

    lcd.blink();
    //lcd.scrollDisplayRight();

    // LED OFF
    digitalWrite(ledPin, LOW);

  } else {
    lcd.setCursor(0, 3);
    lcd.print("  send failed :(");
    delay(200);
  }
}

// send data ( does this send all the data in state machine?? )

void loop()
{
  reading = digitalRead(buttonPin);

  if (reading == HIGH && previous == LOW && millis() - time > debounce) {
    if (state == HIGH)
      state = LOW;
    else
      state = HIGH;

    time = millis();
  }

  if (state)
    send_data();
    
  delay(1000);
}
