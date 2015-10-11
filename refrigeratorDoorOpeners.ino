#include <Servo.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>

#define FREEZER_BUTTON_PIN A0
#define FREEZER_SERVO_PIN 9
#define FREEZER_SERVO_HOME 1530
#define FREEZER_SERVO_OPEN 800
#define FREEZER_OPEN_DURATION 750
#define FREEZER_CLOSE_DURATION 750

#define FRIDGE_BUTTON_PIN A1
#define FRIDGE_DOOR_MOTOR_ID 1
#define FRIDGE_DOOR_SPEED 255
#define FRIDGE_OPEN_DURATION 3000
#define FRIDGE_CLOSE_DURATION 6000 //just in case

Servo freezerDoorServo;

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *fridgeDoorMotor = AFMS.getMotor(FRIDGE_DOOR_MOTOR_ID);

//
void lerpServo(Servo *servo, int startPos, int endPos, int duration)
{
  servo->writeMicroseconds(startPos);
  delay(100); //just in case
  
  int startTime = millis();
  int endTime = startTime + duration;
  
  while (1)
  {
    int elapsed = millis() - startTime;
    if (elapsed >= duration)
    {
      servo->writeMicroseconds(endPos);
      break;
    }

    int pos = map(elapsed, 0, duration, startPos, endPos);
    servo->writeMicroseconds(pos);
  }
}

//
void openFreezerDoor()
{
  lerpServo(&freezerDoorServo, FREEZER_SERVO_HOME, FREEZER_SERVO_OPEN, FREEZER_OPEN_DURATION);
  lerpServo(&freezerDoorServo, FREEZER_SERVO_OPEN, FREEZER_SERVO_HOME, FREEZER_CLOSE_DURATION);
}

//
void openFridgeDoor()
{
  fridgeDoorMotor->run(FORWARD);
  delay(FRIDGE_OPEN_DURATION);
  fridgeDoorMotor->run(BACKWARD);
  delay(FRIDGE_CLOSE_DURATION);
}

#define OPEN_THRESHOLD 25000

//
void setup()
{
  freezerDoorServo.attach(FREEZER_SERVO_PIN);
  freezerDoorServo.writeMicroseconds(FREEZER_SERVO_HOME);
  
  AFMS.begin();
  fridgeDoorMotor->run(RELEASE);
  fridgeDoorMotor->setSpeed(FRIDGE_DOOR_SPEED);

  pinMode(FREEZER_BUTTON_PIN, INPUT_PULLUP);
  pinMode(FRIDGE_BUTTON_PIN, INPUT_PULLUP);

  Serial.begin(115200);

  uint32_t freezerCnt = 0;
  uint32_t fridgeCnt = 0;

  while (1)
  {
    if (digitalRead(FREEZER_BUTTON_PIN) == LOW)
      freezerCnt++;
    else
      freezerCnt = 0;
    
    if (digitalRead(FRIDGE_BUTTON_PIN) == LOW)
      fridgeCnt++;
    else
      fridgeCnt = 0;

    if (freezerCnt > OPEN_THRESHOLD)
    {
      openFreezerDoor();
      freezerCnt = 0;
    }

    if (fridgeCnt > OPEN_THRESHOLD)
    {
      openFridgeDoor();
      fridgeCnt = 0;
    }
  }
}

//
void loop() {} 

