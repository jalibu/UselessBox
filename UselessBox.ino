#include "Servo.h"
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial softwareSerial(10,11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

Servo handServo;
Servo boxServo;

#define PIN_SWITCH_BOX 2
#define PIN_SERVO_HAND 5
#define PIN_SERVO_BOX 6
#define PIN_RED 3
#define PIN_GREENBLUE 9
#define DISCO_SPEED 200

#define POSITION_HAND_IDLE 30
#define POSITION_HAND_BUTTON_PRESS POSITION_HAND_IDLE + 127
#define POSITION_HAND_BUTTON_TOUCH POSITION_HAND_IDLE + 110
#define POSITION_HAND_MIDDLE POSITION_HAND_IDLE + 95


#define POSITION_BOX_CLOSE 0
#define POSITION_BOX_OPEN POSITION_BOX_CLOSE + 50
#define POSITION_BOX_EYES POSITION_BOX_CLOSE + 30
#define POSITION_BOX_SLOT POSITION_BOX_CLOSE + 20

#define DELAY_BOX 320
#define DELAY_HAND 250

#define AUDIO_DEFAULT_VOLUME 22

#define NUMBER_ACTIONS 11
#define NUMBER_SOUNDS 20

int action = 1;

boolean playerAvailable = false;
boolean disco = false;

int discoColors[5][2] = { { 255,0 },
                       { 0, 255 },
                       { 255, 255 },
                       {64, 255},
                       {255, 64}};
int colorIndex = 0;

void startDisco() {
  startDisco(random(NUMBER_SOUNDS - 1) + 1, AUDIO_DEFAULT_VOLUME);
  delayWithLight(1000);
}

void startDisco(int number, int volume) {
  disco = true;
  if (playerAvailable) {
    Serial.println("Play Audio #" + String(number));
    myDFPlayer.volume(volume);  //Set volume value. From 0 to 30
    myDFPlayer.playMp3Folder(number);
  }
}

void stopDisco() {
  if(disco) {
    Serial.println("> Disco stopped");
  }
  myDFPlayer.stop();
  disco = false;
  updateColor();
}

void detachServos() {
  handServo.detach();
  boxServo.detach();
}

void attachServos() {
  handServo.attach(PIN_SERVO_HAND);
  boxServo.attach(PIN_SERVO_BOX);
  delay(1);
}

void move(Servo servo, int end, int speed, int discoStop) {
  int start = servo.read();

  if (start <= end) {
    for (int i = start; i <= end; i++)
    {
      servo.write(i);
      if (i == discoStop) {
        stopDisco();
      }
      delay(speed);
    }
  } else {
    for (int i = start; i >= end; i--)
    {
      servo.write(i);
      if (i == discoStop) {
        stopDisco();
      }
      delay(speed);
    }
  }
}

void jumpBack(int preDelay, int postDelay) {
  delay(preDelay);
  hand(POSITION_HAND_IDLE);
  delay(DELAY_BOX);
  box(POSITION_BOX_CLOSE);
  delay(postDelay);
}

void jump(Servo servo, int position) {
  servo.write(position);
}

void box(int position, int speed) {
  move(boxServo, position, speed, -1);
}

void box(int position) {
  jump(boxServo, position);
}

void hand(int position, int speed) {
  move(handServo, position, speed, POSITION_HAND_BUTTON_PRESS);
}

void hand(int position) {
  move(handServo, position, 5, POSITION_HAND_BUTTON_PRESS);
}

void delayWithLight(int delayTime){
  int intervals = delayTime / DISCO_SPEED;
  for(int i = 0; i <= intervals; i++){
    updateColor();
    delay(DISCO_SPEED);
  }
  delay(delayTime - intervals * DISCO_SPEED);
}

void updateColor()
 {
  if(disco){
    int color = random(0, 4);
    analogWrite(PIN_RED, discoColors[color][0]);
    analogWrite(PIN_GREENBLUE, discoColors[color][1]);
  } else {
    analogWrite(PIN_RED, 0);
    analogWrite(PIN_GREENBLUE, 0);
  }
}

void setup()
{
  Serial.begin(9600);
  softwareSerial.begin(9600);
  pinMode(PIN_SWITCH_BOX, INPUT);
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREENBLUE, OUTPUT);

  // Reset Servos
  attachServos();
  hand(POSITION_HAND_IDLE);
  box(POSITION_BOX_CLOSE);
  delay(1000);
  detachServos();

  Serial.println(F("Initializing Player"));
  if (myDFPlayer.begin(softwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("DFPlayer online."));
    myDFPlayer.volume(AUDIO_DEFAULT_VOLUME);  //Set volume value. From 0 to 30
    playerAvailable = true;
  } else {
    Serial.println(F("No Player found."));
  }

  // Reset ligth
  updateColor();

  randomSeed(analogRead(0));
}

void loop()
{
  if (digitalRead(PIN_SWITCH_BOX) == HIGH) {
    attachServos();
    startDisco();
    Serial.println("Play Action #" + String(action));
    // int action = random(NUMBER_ACTIONS - 1) + 1;
    switch (action) {
      case 1:
        box(POSITION_BOX_OPEN);
        delayWithLight(1000);
        hand(POSITION_HAND_BUTTON_PRESS);
        jumpBack(100,0);
        break;
      case 2:
        box(POSITION_BOX_OPEN);
        delayWithLight(1500);
        hand(POSITION_HAND_BUTTON_TOUCH);
        delayWithLight(2300);
        hand(POSITION_HAND_BUTTON_PRESS);
        jumpBack(100,0);
        break;
      case 3:
        box(POSITION_BOX_OPEN);
        delayWithLight(1000);
        hand(POSITION_HAND_BUTTON_PRESS);
        delayWithLight(100);
        hand(POSITION_HAND_IDLE);
        delayWithLight(400);
        box(POSITION_BOX_CLOSE);
        delayWithLight(2000);
        box(POSITION_BOX_OPEN);
        delayWithLight(2000);
        box(POSITION_BOX_CLOSE);
        break;
      case 4:
        box(POSITION_BOX_EYES, 100);
        delayWithLight(1000);
        box(POSITION_BOX_CLOSE);
        delayWithLight(100);
        box(POSITION_BOX_SLOT);
        delayWithLight(100);
        box(POSITION_BOX_CLOSE);
        delayWithLight(100);
        box(POSITION_BOX_SLOT);
        delayWithLight(100);
        box(POSITION_BOX_CLOSE);
        delayWithLight(100);
        box(POSITION_BOX_SLOT);
        delayWithLight(100);
        box(POSITION_BOX_CLOSE);
        delayWithLight(100);
        box(POSITION_BOX_SLOT);
        delayWithLight(100);
        box(POSITION_BOX_CLOSE);
        delayWithLight(100);
        box(POSITION_BOX_SLOT);
        delayWithLight(100);
        box(POSITION_BOX_CLOSE);
        delayWithLight(100);
        box(POSITION_BOX_SLOT);
        delayWithLight(100);
        box(POSITION_BOX_OPEN, 100);
        delayWithLight(1000);
        hand(POSITION_HAND_BUTTON_PRESS, 90);
        hand(POSITION_HAND_IDLE, 10);
        delayWithLight(500);
        box(POSITION_BOX_SLOT);
        delayWithLight(2500);
        box(POSITION_BOX_OPEN);
        delayWithLight(1500);
        box(POSITION_BOX_CLOSE);
        break;
      case 5:
        box(POSITION_BOX_OPEN);
        delayWithLight(200);
        hand(POSITION_HAND_BUTTON_PRESS);
        delayWithLight(200);
        box(POSITION_BOX_EYES);
        delayWithLight(100);
        box(POSITION_BOX_SLOT);
        delayWithLight(100);
        box(POSITION_BOX_EYES);
        delayWithLight(100);
        box(POSITION_BOX_SLOT);
        delayWithLight(100);
        box(POSITION_BOX_EYES);
        delayWithLight(100);
        box(POSITION_BOX_SLOT);
        delayWithLight(100);
        box(POSITION_BOX_EYES);
        delayWithLight(100);
        box(POSITION_BOX_SLOT);
        delayWithLight(100);
        box(POSITION_BOX_OPEN);
        jumpBack(1000,0);
        break;
      case 6:
        box(POSITION_BOX_OPEN);
        delayWithLight(200);
        hand(POSITION_HAND_BUTTON_PRESS);
        delayWithLight(1800);
        hand(POSITION_HAND_BUTTON_TOUCH);
        delayWithLight(100);
        hand(POSITION_HAND_BUTTON_PRESS);
        delayWithLight(100);
        hand(POSITION_HAND_BUTTON_TOUCH);
        delayWithLight(100);
        hand(POSITION_HAND_BUTTON_PRESS);
        delayWithLight(100);
        hand(POSITION_HAND_BUTTON_TOUCH);
        delayWithLight(100);
        hand(POSITION_HAND_BUTTON_PRESS);
        delayWithLight(100);
        hand(POSITION_HAND_BUTTON_TOUCH);
        delayWithLight(100);
        hand(POSITION_HAND_BUTTON_PRESS);
        jumpBack(2000,0);
        break;
      case 7:
        box(POSITION_BOX_OPEN);
        delayWithLight(200);
        hand(POSITION_HAND_BUTTON_TOUCH, 100);
        delayWithLight(1200);
        hand(POSITION_HAND_BUTTON_PRESS);
        delayWithLight(100);
        hand(POSITION_HAND_IDLE);
        delayWithLight(200);
        box(POSITION_BOX_CLOSE);
        delayWithLight(1800);
        box(POSITION_BOX_EYES);
        delayWithLight(2000);
        box(POSITION_BOX_CLOSE);
        break;
      case 8:
        box(POSITION_BOX_EYES);
        delayWithLight(100);
        box(POSITION_BOX_CLOSE);
        delayWithLight(100);
        box(POSITION_BOX_EYES);
        delayWithLight(100);
        box(POSITION_BOX_CLOSE);
        delayWithLight(100);
        box(POSITION_BOX_EYES);
        delayWithLight(100);
        box(POSITION_BOX_CLOSE);
        delayWithLight(100);
        box(POSITION_BOX_EYES);
        delayWithLight(100);
        box(POSITION_BOX_CLOSE);
        delayWithLight(100);
        box(POSITION_BOX_EYES);
        delayWithLight(100);
        box(POSITION_BOX_CLOSE);
        delayWithLight(100);
        box(POSITION_BOX_EYES);
        delayWithLight(100);
        box(POSITION_BOX_CLOSE);
        delayWithLight(100);
        box(POSITION_BOX_EYES);
        delayWithLight(100);
        box(POSITION_BOX_CLOSE);
        delayWithLight(100);
        box(POSITION_BOX_OPEN);
        delayWithLight(100);
        hand(POSITION_HAND_BUTTON_PRESS);
        jumpBack(300,0);
        break;
      case 9:
        box(POSITION_BOX_OPEN);
        delayWithLight(1500);
        hand(POSITION_HAND_BUTTON_PRESS);
        delayWithLight(200);
        hand(POSITION_HAND_IDLE);
        delayWithLight(400);
        box(POSITION_BOX_CLOSE, 10);
        delayWithLight(1400);
        box(POSITION_BOX_OPEN, 10);
        delayWithLight(500);
        hand(POSITION_HAND_BUTTON_PRESS, 15);
        delayWithLight(1200);
        hand(POSITION_HAND_BUTTON_TOUCH);
        delayWithLight(100);
        hand(POSITION_HAND_BUTTON_PRESS);
        delayWithLight(100);
        hand(POSITION_HAND_BUTTON_TOUCH);
        delayWithLight(100);
        hand(POSITION_HAND_BUTTON_PRESS);
        delayWithLight(100);
        hand(POSITION_HAND_BUTTON_TOUCH);
        delayWithLight(100);
        hand(POSITION_HAND_BUTTON_PRESS);
        jumpBack(4000,0);
        break;
      case 10:
        box(POSITION_BOX_EYES, 100);
        delayWithLight(1000);
        box(POSITION_BOX_OPEN, 100);
        delayWithLight(100);
        hand(POSITION_HAND_BUTTON_TOUCH, 100);
        delayWithLight(3000);
        hand(POSITION_HAND_BUTTON_PRESS, 100);
        jumpBack(1000,0);
        break;
      case 11:
        box(POSITION_BOX_OPEN);
        delayWithLight(100);
        hand(POSITION_HAND_BUTTON_TOUCH);
        delayWithLight(600);
        hand(POSITION_HAND_MIDDLE);
        delayWithLight(600);
        hand(POSITION_HAND_BUTTON_TOUCH);
        delayWithLight(600);
        hand(POSITION_HAND_MIDDLE);
        delayWithLight(600);
        hand(POSITION_HAND_BUTTON_TOUCH);
        delayWithLight(600);
        hand(POSITION_HAND_MIDDLE);
        delayWithLight(600);
        hand(POSITION_HAND_BUTTON_TOUCH);
        delayWithLight(600);
        hand(POSITION_HAND_MIDDLE);
        delayWithLight(600);
        hand(POSITION_HAND_BUTTON_TOUCH, 1);
        delayWithLight(600);
        hand(POSITION_HAND_MIDDLE);
        delayWithLight(600);
        hand(POSITION_HAND_BUTTON_PRESS, 1);
        jumpBack(200,0);
        break;
    }
    
    action = action == NUMBER_ACTIONS ? 1 : ++action;
    delay(700);
    detachServos();
  }
}