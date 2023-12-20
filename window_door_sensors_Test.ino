// Blynk template information
#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_AUTH_TOKEN ""

// Include necessary libraries
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>

// Pin definitions
#define RXD1 14
#define TXD1 15
#define DOOR_SENSOR  25
#define buzzer 2
#define WINDOW_SENSOR 26

// Servo setup
Servo myservo;
int servoPin = 18;
int ledPinClose = 32;
int ledPinOpen = 33;
int ADC_Max = 4096;

int val;
BlynkTimer timer;

// Blynk authentication and WiFi credentials
char auth[] = ""; // Enter the authentication code sent by Blynk to your Email

char ssid[] = ""; // Enter your WIFI SSID
char pass[] = ""; // Enter your WIFI Password
int flag = 0;
int systemState = 0;  // Variable to store system state

// Blynk app write event for V1
BLYNK_WRITE(V1)
{
  int state = param.asInt();
  Serial.println(state);

  // Assuming you want to turn the system on when the state is 1 and off when it's 0
  if (state == 1) {
    systemState = 1;
    Serial.println("System is ON");
  } else {
    systemState = 0;
    Serial.println("System is OFF");
  }
}

// Blynk app write event for V2
BLYNK_WRITE(V2) {
  int val = param.asInt();

  if (val == 1023) {
    // Increase the output range to make the servo move even more
    int mappedValue = map(val, 0, ADC_Max, 0, 540);
    myservo.write(mappedValue);
    digitalWrite(ledPinOpen, HIGH);
    digitalWrite(ledPinClose, LOW);
    delay(200);
  } else {
    // Increase the output range to make the servo move even more
    digitalWrite(ledPinOpen, LOW);
    digitalWrite(ledPinClose, HIGH);
    int value = map(0, 0, ADC_Max, 0, 540);
    myservo.write(value);
    delay(200);
  }
}

// Function to notify when the door or window is opened
void notifyOnButtonPress()
{
  int isButtonPressed = digitalRead(DOOR_SENSOR);
  int isButtonpressed1 = digitalRead(WINDOW_SENSOR);

  if ((isButtonPressed == 1 || isButtonpressed1 == 1) && flag == 0 && systemState == 1) {
    Serial.println("Someone entered the house");
    Serial1.write('C'); // You can use any character as a command
    delay(1000);
    Blynk.logEvent("security_alert", "Someone entered the house");
    digitalWrite(buzzer, HIGH);
    flag = 1;
  } else {
    flag = 0;
    Serial.println("Door and Window Closed");
    Serial1.print(0);
    digitalWrite(buzzer, LOW);
  }
}

// Setup function
void setup()
{
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, RXD1, TXD1);
  Blynk.begin(auth, ssid, pass);
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);
  myservo.attach(servoPin, 500, 2400);
  pinMode(DOOR_SENSOR, INPUT_PULLUP);
  pinMode(WINDOW_SENSOR, INPUT_PULLUP);
  pinMode(ledPinClose, OUTPUT);
  pinMode(ledPinOpen, OUTPUT);
  pinMode(buzzer, OUTPUT);
  timer.setInterval(5000L, notifyOnButtonPress);
}

// Loop function
void loop()
{
  Blynk.run();
  timer.run();
}



