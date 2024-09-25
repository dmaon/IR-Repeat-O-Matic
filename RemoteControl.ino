#include <EEPROM.h>

// #define DEBUG // for show more info
#include <IRremote.hpp>

const int receiverPin = 5;
const int builtInLED = 2;
const int transmitterPin = 4;
const int btnPin1 = 12;
const int btnPin2 = 13;
const int EEPROMSize = 16;

int protocol = 0;
unsigned int address = 0;
unsigned int valBtn1 = 0;
unsigned int valBtn2 = 0;

int flagCommit = false;

void setup()
{
  pinMode(builtInLED, OUTPUT);
  pinMode(btnPin1, INPUT_PULLUP);
  pinMode(btnPin2, INPUT_PULLUP);

  IrReceiver.begin(receiverPin, ENABLE_LED_FEEDBACK);
  IrSender.begin(transmitterPin, ENABLE_LED_FEEDBACK, USE_DEFAULT_FEEDBACK_LED_PIN);
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  if (!EEPROM.begin(EEPROMSize)) {
    Serial.println("failed to initialize EEPROM");
    delay(1000000);
  }

  EEPROM.get(0, protocol); // Read protocol from address 0
  EEPROM.get(4, address); // Read protocol from address 0
  EEPROM.get(8, valBtn1);  // Read first button value from address 4
  EEPROM.get(12, valBtn2);  // Read second button value from address 8

  Serial.print("\nlast protocol is ");
  Serial.println(protocol);
  Serial.print("last address is ");
  Serial.println(address);
  Serial.print("last value pf btn1 is ");
  Serial.println(valBtn1);
  Serial.print("last value pf btn2 is ");
  Serial.println(valBtn2);

}
void loop()
{
  // init
  int btn1 = digitalRead(btnPin1);
  int btn2 = digitalRead(btnPin2);


  if (IrReceiver.decode() ) {
    IRRawDataType rawData = IrReceiver.decodedIRData.decodedRawData;

    if (rawData > 0) {

      int senderProtocol = IrReceiver.decodedIRData.protocol;

      if (senderProtocol > 0) {

        // set address
        address = IrReceiver.decodedIRData.address;

        // set last sender protocol
        protocol = senderProtocol;
        if (btn1 == LOW) {
          digitalWrite(builtInLED, 1);

          valBtn1 = IrReceiver.decodedIRData.command;
          EEPROM.put(0, senderProtocol);
          EEPROM.put(4, address);
          EEPROM.put(8, valBtn1);
          flagCommit = true;

          Serial.print("value of ");
          Serial.print(valBtn1);
          Serial.println(" has recorded.");
        } else {
          digitalWrite(builtInLED, 0);
        }

        if (btn2 == LOW) {
          digitalWrite(builtInLED, 1);

          valBtn2 = IrReceiver.decodedIRData.command;
          EEPROM.put(0, senderProtocol);
          EEPROM.put(4, address);
          EEPROM.put(12, valBtn2);
          flagCommit = true;

          Serial.print("value of ");
          Serial.print(valBtn1);
          Serial.println(" has recorded.");
        } else {
          digitalWrite(builtInLED, 0);
        }


        if (flagCommit)
          EEPROM.commit();

        if (btn1 == HIGH && btn2 == HIGH) {
          // Serial.println("protocol: ");
          // Serial.println(senderProtocol);
          // Serial.println("Decode Data: ");
          // Serial.println(rawData, HEX); // Print "old" raw data
          // Serial.println("command: ");
          // Serial.println(IrReceiver.decodedIRData.command, HEX);
          Serial.println("printIRResultShort: ");
          IrReceiver.printIRResultShort(&Serial); // Print complete received data in one line
          // Serial.println("the raw timing data received");
          // IrReceiver.printIRResultRawFormatted(&Serial, true);
          // Serial.println("how to send the received data: ");
          // IrReceiver.printIRSendUsage(&Serial);   // Print the statement required to send this data
        }

      } else if (senderProtocol == 0) { // for unknown protocol
        Serial.println("unknown protocol");
      }
    }
    IrReceiver.resume(); // Enable receiving of the next value
  } else {

    if (valBtn1 !=0 && btn1 == LOW) {
      digitalWrite(builtInLED, 1);

      if (protocol == 8) {
        IrSender.sendNEC(address | 0x0, valBtn1 | 0x0, 3);
      }
      
      Serial.print("value of ");
      Serial.print(valBtn1);
      Serial.println(" has sent.");
      
    } else {
      digitalWrite(builtInLED, 0);
    }

    if (valBtn2 !=0 && btn2 == LOW) {
      digitalWrite(builtInLED, 1);

      if (protocol == 8) {
        IrSender.sendNEC(address | 0x0, valBtn2 | 0x0, 3);
      }
      Serial.print("value of ");
      Serial.print(valBtn2);
      Serial.println(" has sent.");
    } else {
      digitalWrite(builtInLED, 0);
    }

  }


}