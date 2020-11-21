/*  Cube Camera Pan - v4
    Servo controle - speed and movement 0 to 180 degree
    31-5-2020 - Jens Krogsgaard
    rev A - 9-6-2020: Hardcoded wifi credentials and blynk token is replaces 
                      with logon-box.
    rev B - 21-11-2020: Blynk auth token replaced with xxxxxxxxx
*/

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>
#include <DNSServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <EEPROM.h>
#include <DoubleResetDetector.h>

#define DRD_TIMEOUT 10
#define DRD_ADDRESS 0

DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);

// auth kode fra BLYNK app - Drejekamera V3 - defualt kan bee replaced
char auth[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

int oldPos; // servo - old position
int newPos; // servo - new position
int stepPos; // step position for servo - used when you step from old to new position
int servoSpeed; // speed of servo - delay - big value - slow, small value fast

Servo servo;

void setup()
{
    // initial values
  EEPROM.begin(512);
  Serial.begin(115200);

  // WIFI - connect
  WiFiManager wifiManager;
  // wifiManager.resetSettings();    //Uncomment this to wipe WiFi settings from EEPROM on boot.  Comment out and recompile/upload after 1 boot cycle.
  // Input blynk token
  WiFiManagerParameter BlynkToken("auth", "Blynk Token", auth, 34);
  wifiManager.addParameter(&BlynkToken);

  if (drd.detectDoubleReset()) {
    // when reset button is activated by a double click a reconnect to wifi i initiated.
    Serial.println("Double Reset Detected");
    digitalWrite(LED_BUILTIN, LOW);
    wifiManager.startConfigPortal("ConnectCameraTrain");

  } else {
    Serial.println("No Double Reset Detected");
    digitalWrite(LED_BUILTIN, HIGH);
    // temporary hotspot ConnectCameraTrain is credted
    wifiManager.autoConnect("ConnectCameraTrain");
  }

  strcpy(auth, BlynkToken.getValue());

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output, I like blinkies.
  Blynk.begin(auth, WiFi.SSID().c_str(), WiFi.psk().c_str());

  servoSpeed = 10;
  servo.attach(2); // 2 means D4 pin of ESP8266
}

// Slider angle - 0 to 180 degree
BLYNK_WRITE(V0) {
  turnServo(param.asInt());
}

// Slider speed - from 20 to 0
BLYNK_WRITE(V1) {
  servoSpeed = param.asInt();
}


// Button - 0 degree
BLYNK_WRITE(V2) {
  turnServo(0);
}
// Button - 45 degree
BLYNK_WRITE(V3) {
  turnServo(45);
}
// Button - 90 degree
BLYNK_WRITE(V4) {
  turnServo(90);
}
// Button - 135 degree
BLYNK_WRITE(V5) {
  turnServo(135);
}
// Button - 180 degree
BLYNK_WRITE(V6) {
  turnServo(180);
}



// Turn servo an angle
// Speed is implemented as delay between each angle
//   long delay - slow speed
//   short delay - fast speed

void turnServo(int turnTo)
{
  oldPos = servo.read();
  newPos = turnTo;

  if (oldPos <= newPos)
  {
    for (stepPos = oldPos ; stepPos <= newPos; stepPos += 1)
    {
      servo.write(stepPos);
      delay(servoSpeed);
    }
  }
  else
  {
    for (stepPos = oldPos ; stepPos >= newPos; stepPos -= 1)
    {
      servo.write(stepPos);
      delay(servoSpeed);
    }
  }
}


void loop()
{
  Blynk.run();
}
