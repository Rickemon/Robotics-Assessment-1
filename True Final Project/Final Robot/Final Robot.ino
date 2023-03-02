#include <Wire.h>
#include <Zumo32U4.h>



Zumo32U4Motors motors;
Zumo32U4Buzzer buzzer;
Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors proxSensors;
Zumo32U4ButtonA buttonA;

const uint16_t messageTimeoutMs = 50000;//times in MS that you resend a command or the zumo stops
const unsigned long checkFatigueInterval = 2500;
const unsigned long checkInterval = 4000;



uint16_t lastMessageTimeMs = 0;//record of when the last command was sent is MS
unsigned long lastCheckTimeForFatigue = 0;//record of when the last scan for people was done in MS
unsigned long lastCheckTime = 0;//record of when the last person was found in MS

bool messageActive = false;//is a comand being run?


const uint8_t sensorThreshold = 1;//limit of when the sensor detect an object
const uint16_t turnSpeedMax = 100;//basic speed settings 
const uint16_t turnSpeedMin = 10;
const uint16_t deceleration = 10;
const uint16_t acceleration = 10;
uint16_t mode = 1;//mode starts in manual
const int lineSpeed = 100;//avverage speed when following the line




const int cornerThreshold = 300;
const int turnDuration = 500;

char path[1000];//the path the robot took 
uint8_t pathLength = 0;//how many instruction are in path


#define LEFT 0
#define RIGHT 1

bool senseDir = RIGHT;
bool looking = false;
bool turningLeft = false;
bool turningRight = false;
uint16_t turnSpeed = turnSpeedMax;

void calibrateLineSensors()//calibration function needed to use line sensors
{

  ledYellow(1);

  for (uint16_t i = 0; i < 100; i++)
  {
    lineSensors.calibrate();
  }

  ledYellow(0);

}




void setup() {//waits for a button to be pressed 
  Serial.begin(9600);
  proxSensors.initThreeSensors();
  lineSensors.initThreeSensors();
  calibrateLineSensors();


}




void processEvents()//process serial input to commands that the application understands
{

  if (Serial.available() != 0) {
    char msg = Serial.read();
    if(msg == 'w'){
      Serial.write("Zumo: moving forwards");
      motors.setSpeeds(100, 100);
      if(path[pathLength-1] != 'w'){
        path[pathLength] = 'w';
        pathLength += 1;
      }
      
      messageActive = true;
      lastMessageTimeMs = millis();
      }
    else if(msg == 'a'){
      Serial.write("Zumo: moving left");
      motors.setSpeeds(-75, 75);
      if(path[pathLength-1] != 'a'){
        path[pathLength] = 'a';
        pathLength += 1;
                                  }
      messageActive = true;
      lastMessageTimeMs = millis();
      }
    else if(msg == 's'){
      Serial.write("Zumo: moving backwards");
      motors.setSpeeds(-50, -50);
      if(path[pathLength-1] != 's'){
        path[pathLength] = 's';
        pathLength += 1;
                                  }
      
      messageActive = true;
      lastMessageTimeMs = millis();
      }
    else if(msg == 'd'){
      Serial.write("Zumo: moving right");
      motors.setSpeeds(75, -75);
      if(path[pathLength-1] != 'd'){
        path[pathLength] = 'd';
        pathLength += 1;
                                  }
      messageActive = true;
      lastMessageTimeMs = millis();
      }
    else if(msg == 'x'){
      mode = 1;
      Serial.write("Zumo: stopped moving");
      stop();
      }
    else if(msg == 'p'){
      Serial.write("Zumo: is looking for person");
      messageActive = true;
      lastMessageTimeMs = millis();
      looking = true;
      }
    else if(msg == 'c'){
      Serial.write("Zumo: Callibrating line sensors");
      calibrateLineSensors();
      Serial.write("Zumo: Callibrating finished");
      }
    else if(msg == '1'){
      mode = 1;
      pathLength = 0;
      stop();
      }
    else if(msg == '2'){
      mode = 2;
      Serial.write("Zumo: semi-begin");
      pathLength = 0;
      stop();
      }
    else if(msg == '3'){
      mode = 3;
      Serial.write("Zumo: full auto begin");
      pathLength = 0 ;
      stop();
      }
    else{
      Serial.write("Error Invalid Command");
      }
    }     
  

}
void turnRight()
{
  motors.setSpeeds(turnSpeed, -turnSpeed);
  turningLeft = false;
  turningRight = true;
}

void turnLeft()
{
  motors.setSpeeds(-turnSpeed, turnSpeed);
  turningLeft = true;
  turningRight = false;
}


void stop()
{
  motors.setSpeeds(0, 0);
  turningLeft = false;
  turningRight = false;
  messageActive = false;
  looking = false;
}

void look()
{
  
  
  proxSensors.read();
  uint8_t leftValue = proxSensors.countsFrontWithLeftLeds();
  uint8_t rightValue = proxSensors.countsFrontWithRightLeds();

  // Determine if an object is visible or not.
  bool objectSeen = leftValue >= sensorThreshold || rightValue >= sensorThreshold;

  if (objectSeen)
  {
    turnSpeed -= deceleration;
  }
  else
  {
    turnSpeed += acceleration;
  }

  turnSpeed = constrain(turnSpeed, turnSpeedMin, turnSpeedMax);

  if (objectSeen)
  {
    if (leftValue < rightValue)//turns to point to person
    {
      turnRight();
      senseDir = RIGHT;
    }
    else if (leftValue > rightValue)
    {
      turnLeft();
      senseDir = LEFT;
    }
    else
    {

      Serial.write("Person found the path is:");
      for(int i = 0; i < pathLength; i++)//reports path user took to person
      {
        Serial.write(path[i]);
      }
      stop();
    }
  }
  else
  {
    if (senseDir == RIGHT)
    {
      turnRight();
    }
    else
    {
      turnLeft();
    }
  }
}

void mode2(){
  unsigned int sensors[3];
  lineSensors.read(sensors);
  if(sensors[1] > 300){
    Serial.write("Zumo: Wall encountered switching to manual contorl");//report to user
    motors.setSpeeds(0, 0);
    mode = 1;
  }
  else if (sensors[0] > 1200) {

    motors.setSpeeds(100, -100);
  }
  // Check right sensor for black line
  else if (sensors[2] > 1800) {

    motors.setSpeeds(-100, 100);
  }
  // No black line detected, move forward
  else {
    motors.setSpeeds(100, 100);
  }
  unsigned long currentTime = millis();

  if (currentTime - lastCheckTime >= checkInterval && currentTime - lastCheckTimeForFatigue >= checkFatigueInterval)
    {
    lastCheckTimeForFatigue = currentTime;
    proxSensors.read();
    uint8_t leftValue = proxSensors.countsLeftWithLeftLeds();
    uint8_t rightValue = proxSensors.countsRightWithRightLeds();
    if (leftValue >= sensorThreshold)
    {
      lastCheckTime = currentTime;
      Serial.println("zumo has found someone on the left!");
    }
    if (rightValue >= sensorThreshold)
    {
      lastCheckTime = currentTime;
      Serial.println("zumo has found someone on the right!");
    }
    }

}


void mode3(){
  
  unsigned int sensors[3];
  lineSensors.read(sensors);
  int position = (int)sensors[0] - 700;

  int speedDifference = position / 10;//small corrections
  int leftSpeed = lineSpeed - speedDifference;
  int rightSpeed = lineSpeed + speedDifference;

  if (position < -50) {//big corrections
    leftSpeed = 0;
    rightSpeed = lineSpeed;
  }

  else if (position > 50) {//big corrections
    leftSpeed = lineSpeed;
    rightSpeed = 0;
  }
  

  if (sensors[1] > cornerThreshold) {//if it hits a wall
    leftSpeed = 100;
    rightSpeed = -100;

    motors.setSpeeds(leftSpeed, rightSpeed);

    delay(800);
  }


  unsigned long currentTime = millis();

    if (currentTime - lastCheckTime >= checkInterval && currentTime - lastCheckTimeForFatigue >= checkFatigueInterval)
    {
    lastCheckTimeForFatigue = currentTime;
    proxSensors.read();
    uint8_t leftValue = proxSensors.countsFrontWithLeftLeds();
    uint8_t rightValue = proxSensors.countsFrontWithRightLeds();


    if (leftValue >4 && rightValue>4)
    {
      lastCheckTime = currentTime;
      Serial.println("zumo has found someone!");
    }
    }


    

  motors.setSpeeds(leftSpeed, rightSpeed );
}





void loop() {//main loop
  ledYellow(messageActive);//is a command being run? if yes the led is on
  
  processEvents();
  if(mode == 1){//mode 1 manual
    if (looking){//if the command look for person has been triggered than it runs the look for person command until either a a person has been found or the command has been timmed out
      look();
    }
    if (messageActive && (uint16_t)(millis() - lastMessageTimeMs) > messageTimeoutMs)//if a command is running but has timmed out
    {
      Serial.write("Zumo: stopped moving");//report to user
      messageActive = false;//no command is being run
      looking = false;//command has timed out so no more looking for people
      stop();//stop motors
    }
  }
  else if(mode == 2){//mode 2 semi-auto
    mode2();
    
  }
  else if(mode == 3){//mode 3 full auto

    mode3();

    
  }
  
}
