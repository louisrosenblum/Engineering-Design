#include <HexConversionUtils.h>
#include <pfodArc.h>
#include <pfodBLEBufferedSerial.h>
#include <pfodBufferedStream.h>
#include <pfodCircle.h>
#include <pfodControl.h>
#include <pfodDelay.h>
#include <pfodDwgs.h>
#include <pfodDwgsBase.h>
#include <pfodEEPROM.h>
#include <pfodErase.h>
#include <pfodHide.h>
#include <pfodIndex.h>
#include <pfodInsertDwg.h>
#include <pfodLabel.h>
#include <pfodLine.h>
#include <pfodMAC.h>
#include <pfodMACClient.h>
#include <pfodParser.h>
#include <pfodParserUtils.h>
#include <pfodRadio.h>
#include <pfodRadioDriver.h>
#include <pfodRadioMsg.h>
#include <pfodRandom.h>
#include <pfodRawCmdParser.h>
#include <pfodRectangle.h>
#include <pfodRingBuffer.h>
#include <pfodSecurity.h>
#include <pfodSecurityClient.h>
#include <pfodSMS_SIM5320.h>
#include <pfodSMS_SIM900.h>
#include <pfodStream.h>
#include <pfodTouchAction.h>
#include <pfodTouchZone.h>
#include <pfodUnhide.h>
#include <pfodWaitForUtils.h>
#include <pfod_Base.h>
#include <pfod_rawOutput.h>
#include <SipHash_2_4.h>

#include <pfodESP32BufferedClient.h>
#include <pfodESP32Utils.h>

#include <DesignerSwitch.h>
#include <Gauge.h>
#include <Help.h>
#include <IndicatorButton.h>
#include <OnOffSlider.h>
#include <OnOffSwitch.h>
#include <pfodDwgControls.h>
#include <RadiationSymbol.h>
#include <Slider.h>
#include <VSlider.h>

  #include <Wire.h>
  #include <Adafruit_MotorShield.h>

  Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
  
  Adafruit_DCMotor *Motor_1 = AFMS.getMotor(1);
  Adafruit_DCMotor *Motor_2 = AFMS.getMotor(2);
  Adafruit_DCMotor *Motor_3 = AFMS.getMotor(3);
  Adafruit_DCMotor *Motor_4 = AFMS.getMotor(4);


  
/* ===== pfod Command for Menu_1 ====
pfodApp msg {.} --> {,~<prompt not set>`1000~V1|A~Fwd|B~Bwd|C~Left|D~Right}
 */
// Using ESP32 based board programmed via Arduino IDE
// follow the steps given on http://www.forward.com.au/pfod/ESP32/index.html to install ESP32 support for Arduino IDE
//Based on Neil Kolban example https://github.com/nkolban/ESP32_BLE_Arduino
/* Code generated by pfodDesignerV3 V3.0.3441
 */
/*
 * (c)2014-2018 Forward Computing and Control Pty. Ltd.
 * NSW Australia, www.forward.com.au
 * This code is not warranted to be fit for any purpose. You may only use it at your own risk.
 * This generated code may be freely used for both private and commercial use
 * provided this copyright is maintained.
 */

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#define DEBUG

// Download pfodESP32BufferedClient library from http://www.forward.com.au/pfod/pfodParserLibraries/index.html
// pfodESP32BufferedClient.zip contains pfodESP32BufferedClient and pfodESP32Utils
#include <pfodESP32Utils.h>
// download the libraries from http://www.forward.com.au/pfod/pfodParserLibraries/index.html
// pfodParser.zip V3.31+ contains pfodParser, pfodSecurity, pfodDelay, pfodBLEBufferedSerial, pfodSMS and pfodRadio
#include <pfodParser.h>

#include <pfodBLEBufferedSerial.h> // used to prevent flooding bluetooth sends
int swap01(int); // method prototype for slider end swaps

// =========== pfodBLESerial definitions
const char* localName = "ESP32 BLE";  // <<<<<<  change this string to customize the adverised name of your board 
class pfodBLESerial : public Stream, public BLEServerCallbacks, public BLECharacteristicCallbacks {
  public:
    pfodBLESerial(); void begin(); void poll(); size_t write(uint8_t); size_t write(const uint8_t*, size_t); int read();
    int available(); void flush(); int peek(); void close(); bool isConnected();
    static void addReceiveBytes(const uint8_t* bytes, size_t len);
    const static uint8_t pfodEOF[1]; const static char* pfodCloseConnection;
    volatile static bool connected;
    void onConnect(BLEServer* serverPtr);
    void onDisconnect(BLEServer* serverPtr);
    void onWrite(BLECharacteristic *pCharacteristic);

  private:
    static const int BLE_MAX_LENGTH = 20;
    static const int BLE_RX_MAX_LENGTH = 256; static volatile size_t rxHead; static volatile size_t rxTail;
    volatile static uint8_t rxBuffer[BLE_RX_MAX_LENGTH];
    size_t txIdx;  uint8_t txBuffer[BLE_MAX_LENGTH];
};
volatile size_t pfodBLESerial::rxHead = 0; volatile size_t pfodBLESerial::rxTail = 0;
volatile uint8_t pfodBLESerial::rxBuffer[BLE_RX_MAX_LENGTH]; const uint8_t pfodBLESerial::pfodEOF[1] = {(uint8_t) - 1};
const char* pfodBLESerial::pfodCloseConnection = "{!}"; volatile bool pfodBLESerial::connected = false;

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
BLEServer *serverPtr = NULL;
BLECharacteristic * characteristicTXPtr;
// =========== end pfodBLESerial definitions

pfodParser parser("V2"); // create a parser with menu version string to handle the pfod messages
 // create a parser to handle the pfod messages
pfodBLESerial bleSerial; // create a BLE serial connection
pfodBLEBufferedSerial bleBufferedSerial; // create a BLE serial connection

// the setup routine runs once on reset:
void setup() {

#ifdef DEBUG
  Serial.begin(115200);
  Serial.println();
#endif

  // Create the BLE Device
  BLEDevice::init(localName);
  // Create the BLE Server
  serverPtr = BLEDevice::createServer();
  serverPtr->setCallbacks(&bleSerial);
  // Create the BLE Service
  BLEService *servicePtr = serverPtr->createService(SERVICE_UUID);
  // Create a BLE Characteristic
  characteristicTXPtr = servicePtr->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);
  characteristicTXPtr->addDescriptor(new BLE2902());
  BLECharacteristic * characteristicRXPtr = servicePtr->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);
  characteristicRXPtr->setCallbacks(&bleSerial);

  // Start the service
  servicePtr->start();
  // Start advertising
  serverPtr->getAdvertising()->start();
#ifdef DEBUG
  Serial.println("BLE Server and Advertising started");
#endif

  bleSerial.begin();
  // connect parser
  parser.connect(bleBufferedSerial.connect(&bleSerial)); // connect the parser to the i/o stream via buffer



  // <<<<<<<<< Your extra setup code goes here

  Serial.begin(9600);           // set up Serial library at 9600 bps

  AFMS.begin();  // create with the default frequency 1.6KHz





  
}

// the loop routine runs over and over again forever:
void loop() {
  uint8_t cmd = parser.parse(); // parse incoming data from connection
  uint8_t i;
  // parser returns non-zero when a pfod command is fully parsed
  if (cmd != 0) { // have parsed a complete msg { to }
    uint8_t* pfodFirstArg = parser.getFirstArg(); // may point to \0 if no arguments in this msg.
    pfod_MAYBE_UNUSED(pfodFirstArg); // may not be used, just suppress warning
    long pfodLongRtn; // used for parsing long return arguments, if any
    pfod_MAYBE_UNUSED(pfodLongRtn); // may not be used, just suppress warning
    if ('.' == cmd) {
      // pfodApp has connected and sent {.} , it is asking for the main menu
      if (!parser.isRefresh()) {
        sendMainMenu(); // send back the menu designed
      } else {
        sendMainMenuUpdate(); // menu is cached just send update
      }

    // now handle commands returned from button/sliders

 

    // FORWARD
    } else if('A'==cmd) { // user pressed -- 'Fwd'
      // in the main Menu of Menu_1 
      // << add your action code here for this button

        Motor_1->run(FORWARD);
        Motor_2->run(FORWARD);
        Motor_3->run(FORWARD);
        Motor_4->run(FORWARD);
        for (i=0; i<100; i++) {
        Motor_1->setSpeed(i);
        Motor_2->setSpeed(i);
        Motor_3->setSpeed(i);
        Motor_4->setSpeed(i);  
        delay(10);
        }

        Motor_1->setSpeed(0);
        Motor_2->setSpeed(0);
        Motor_3->setSpeed(0);
        Motor_4->setSpeed(0);

        delay(100);

      parser.print(F("{}")); // change this return as needed.
       // always send back a pfod msg otherwise pfodApp will disconnect.

    // BACKWARD
    } else if('B'==cmd) { // user pressed -- 'Bwd'
      // in the main Menu of Menu_1 
      // << add your action code here for this button

      
        Motor_1->run(BACKWARD);
        Motor_2->run(BACKWARD);
        Motor_3->run(BACKWARD);
        Motor_4->run(BACKWARD);
        for (i=0; i<100; i++) {
        Motor_1->setSpeed(i);
        Motor_2->setSpeed(i);
        Motor_3->setSpeed(i);
        Motor_4->setSpeed(i);  
        delay(10);
        }

        Motor_1->setSpeed(0);
        Motor_2->setSpeed(0);
        Motor_3->setSpeed(0);
        Motor_4->setSpeed(0);

        delay(100);


      
      parser.print(F("{}")); // change this return as needed.
       // always send back a pfod msg otherwise pfodApp will disconnect.

    // LEFT
    } else if('C'==cmd) { // user pressed -- 'Left'
      // in the main Menu of Menu_1 
      // << add your action code here for this button

        Motor_1->run(BACKWARD);
        Motor_2->run(BACKWARD);
        Motor_3->run(FORWARD);
        Motor_4->run(FORWARD);
        for (i=0; i<50; i++) {
        Motor_1->setSpeed(i);
        Motor_2->setSpeed(i);
        Motor_3->setSpeed(i);
        Motor_4->setSpeed(i);  
        delay(4);
        }

        Motor_1->setSpeed(0);
        Motor_2->setSpeed(0);
        Motor_3->setSpeed(0);
        Motor_4->setSpeed(0);

        delay(50);

      
      parser.print(F("{}")); // change this return as needed.
       // always send back a pfod msg otherwise pfodApp will disconnect.

    // RIGHT
    } else if('D'==cmd) { // user pressed -- 'Right'
      // in the main Menu of Menu_1 
      // << add your action code here for this button

        Motor_1->run(FORWARD);
        Motor_2->run(FORWARD);
        Motor_3->run(BACKWARD);
        Motor_4->run(BACKWARD);
        for (i=0; i<50; i++) {
        Motor_1->setSpeed(i);
        Motor_2->setSpeed(i);
        Motor_3->setSpeed(i);
        Motor_4->setSpeed(i);  
        delay(4);
        }

        Motor_1->setSpeed(0);
        Motor_2->setSpeed(0);
        Motor_3->setSpeed(0);
        Motor_4->setSpeed(0);

        delay(50);
      
      parser.print(F("{}")); // change this return as needed.
       // always send back a pfod msg otherwise pfodApp will disconnect.

    } else if ('!' == cmd) {
      // CloseConnection command
      closeConnection(parser.getPfodAppStream());
    } else {
      // unknown command
      parser.print(F("{}")); // always send back a pfod msg otherwise pfodApp will disconnect.
    }
  }
  //  <<<<<<<<<<<  Your other loop() code goes here 
  
}

void closeConnection(Stream *io) {
    // nothing special here
}

void sendMainMenu() {
  // !! Remember to change the parser version string
  //    every time you edit this method
  parser.print(F("{,"));  // start a Menu screen pfod message
  // send menu background, format, prompt, refresh and version
  parser.print(F("~<prompt not set>`1000"));
  parser.sendVersion(); // send the menu version 
  // send menu items
  parser.print(F("|A"));
  parser.print(F("~Fwd"));
  parser.print(F("|B"));
  parser.print(F("~Bwd"));
  parser.print(F("|C"));
  parser.print(F("~Left"));
  parser.print(F("|D"));
  parser.print(F("~Right"));
  parser.print(F("}"));  // close pfod message
}

void sendMainMenuUpdate() {
  parser.print(F("{;"));  // start an Update Menu pfod message
  // send menu items
  parser.print(F("}"));  // close pfod message
  // ============ end of menu ===========
}
// ========== pfodBLESerial methods
pfodBLESerial::pfodBLESerial() {}

bool pfodBLESerial::isConnected() {
  return (connected);
}
void pfodBLESerial::begin() {}

void pfodBLESerial::close() {}

void pfodBLESerial::poll() {}

size_t pfodBLESerial::write(const uint8_t* bytes, size_t len) {
  for (size_t i = 0; i < len; i++) {  write(bytes[i]);  }
  return len; // just assume it is all written
}

size_t pfodBLESerial::write(uint8_t b) {
  if (!isConnected()) { return 1; }
  txBuffer[txIdx++] = b;
  if ((txIdx == sizeof(txBuffer)) || (b == ((uint8_t)'\n')) || (b == ((uint8_t)'}')) ) {
    flush(); // send this buffer if full or end of msg or rawdata newline
  }
  return 1;
}

int pfodBLESerial::read() {
  if (rxTail == rxHead) { return -1; }
  // note increment rxHead befor writing
  // so need to increment rxTail befor reading
  rxTail = (rxTail + 1) % sizeof(rxBuffer);
  uint8_t b = rxBuffer[rxTail];
  return b;
}

// called as part of parser.parse() so will poll() each loop()
int pfodBLESerial::available() {
  flush(); // send any pending data now. This happens at the top of each loop()
  int rtn = ((rxHead + sizeof(rxBuffer)) - rxTail ) % sizeof(rxBuffer);
  return rtn;
}

void pfodBLESerial::flush() {
  if (txIdx == 0) { return; }
  characteristicTXPtr->setValue((uint8_t*)txBuffer, txIdx);
  txIdx = 0;
  characteristicTXPtr->notify();
}

int pfodBLESerial::peek() {
  if (rxTail == rxHead) { return -1; }
  size_t nextIdx = (rxTail + 1) % sizeof(rxBuffer);
  uint8_t byte = rxBuffer[nextIdx];
  return byte;
}

void pfodBLESerial::addReceiveBytes(const uint8_t* bytes, size_t len) {
  // note increment rxHead befor writing
  // so need to increment rxTail befor reading
  for (size_t i = 0; i < len; i++) {
    rxHead = (rxHead + 1) % sizeof(rxBuffer);
    rxBuffer[rxHead] = bytes[i];
  }
}

//=========== ESP32 BLE callback methods
void pfodBLESerial:: onConnect(BLEServer* serverPtr) {
  // clear parser with -1 in case partial message left, should not be one
  addReceiveBytes(bleSerial.pfodEOF, sizeof(pfodEOF));
  connected = true;
}

void pfodBLESerial::onDisconnect(BLEServer* serverPtr) {
  // clear parser with -1 and insert {!} incase connection just lost
  addReceiveBytes(bleSerial.pfodEOF, sizeof(pfodEOF));
  addReceiveBytes((const uint8_t*)pfodCloseConnection, sizeof(pfodCloseConnection));
  connected = false;
}

void pfodBLESerial::onWrite(BLECharacteristic *pCharacteristic) {
  std::string rxValue = pCharacteristic->getValue();
  uint8_t *data = (uint8_t*)rxValue.data();
  size_t len = rxValue.length();
  addReceiveBytes((const uint8_t*)data, len);
}
//======================= end pfodBLESerial methods


int swap01(int in) {
  return (in==0)?1:0;
}
// ============= end generated code =========
 

