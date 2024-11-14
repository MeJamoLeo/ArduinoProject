#include <Adafruit_MotorShield.h>

// ======================================================
// Global Variable
// ======================================================
//create motor shield object and 4 motors
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *Motor1 = AFMS.getMotor(1);
Adafruit_DCMotor *Motor2 = AFMS.getMotor(2);
Adafruit_DCMotor *Motor3 = AFMS.getMotor(3);
Adafruit_DCMotor *Motor4 = AFMS.getMotor(4);
int motor_speed = 180;

// Ultrasonic Distance Sensor
const int trigPin = 9;
const int echoPin = 10;
long duration;
int distance;
long fillterArray[20];

// Servo
int pos = 90;
bool servo_cc = true;  // Variable to store the servo position. Allowed range is 0 to 180
int wait_time = 100;  // Number of milliseconds to allow servo to reach position
Servo myservo;  // create servo object called 'myservo' to control a servo motor
// ======================================================
void setup_speed(int);
void forward(int);
void backward(int);
void rotateCCW(int);
void rotateCW(int);
void stop(int);
void go_forward_2m();
void go_forward1m_and_rotateCCW();
long ultrasonicMeasure();
// ======================================================

void setup()
{
  Serial.begin(9600);
  Serial.println("Riverbot DC Motor test!");

  // check motor shield
  if (!AFMS.begin()) {
    Serial.println("Could not find Motor Shield. Check wiring");
    while (1); // infinite loop
  }
  Serial.println("Motor Shield found");

  // Ultrasonic Distance Sensor
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  digitalWrite(trigPin, LOW);

  // Motor Speed
  setup_speed(motor_speed);
}

void loop()
{
  forward(10);
  if !(distance <= 25 && distance != 0) return;
  // stop
  Motor1->setSpeed(0);
  Motor2->setSpeed(0);
  Motor3->setSpeed(0);
  Motor4->setSpeed(0);
  Motor1->run(BRAKE);
  Motor2->run(BRAKE);
  Motor3->run(BRAKE);
  Motor4->run(BRAKE);
  // check surround
  pos = 30;
  myservo.write(pos);
  delay(wait_time);

  distance = filtterMesure();

  if (servo_cc == true) {
      pos += 30;
      myservo.write(pos);
      delay(wait_time);
      if (pos >= 180)
	  servo_cc = false;
  } else {
      pos -= 30;
      myservo.write(pos);
      delay(wait_time);
      if (pos <= 0)
	  servo_cc = true;
  }

  // decide direction
  // turn that direction 
  // set_speed
}

long filtterMesure() {
  // 1. Tracking Multiple Measurements and store in an array
  for (int sample = 0; sample < 20; sample++) {
      fillterArray[sample] = ultrasonicMeasure();
      delay(30);
  }

  // 2. Sort
  for (int i = 0; i < 19; i++) {
      for (int j = i + 1; j < 20; j++) {
	  if (filterArray[i] > filterArray[j]) {
	      long swap = filterArray[i];
	      filterArray[i] = filterArray[j];
	      filterArray[j] = swap;
	  }
      }
  }
  // 3. Filtering
  // + the five smallest samples are considered as noise -> ignore it
  // + the five biggest  samples are considered as noise -> ignore it
  // ----------------------------------------------------------------
  // => get average of the 10 middle samples (from 5th to 14th)
  double sum = 0;
  for (int sample = 5; sample < 15; sample++) {
    sum += filterArray[sample];
  }
  long res = sum / 10;

  return res;
}

long ultrasonicMeasure() {
  // trigPin
  // delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); // Sets the trigPin on HIGH state for 10 micro seconds
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH); // echoPin
  distance = duration * 0.034 / 2; // Calculating the distance

  // Prints the distance on the Serial Monitor
  Serial.print("Distance\(cm\): ");
  Serial.println(distance);
  return distance;
}

void setup_speed(int m1)
{
  //Serial.println("Set motor speed to 100 (max=255)");
  Motor1->setSpeed(m1 + 11); //This is fixed 200
  Motor2->setSpeed(180); // 146/177 = 0.825, 200 * 0.825 = 165
  Motor3->setSpeed(151); // 200/123*146 = 237
  Motor4->setSpeed(200 + 3 - 2 - 2 - 5); // 200/169*146
}

void forward(int ms)
{
  //Serial.println("Go forward");
  Motor1->run(FORWARD);
  Motor2->run(BACKWARD); // This is wrong direction, so I fixed with code
  Motor3->run(FORWARD);
  Motor4->run(FORWARD);
  delay(ms);
}

void backward(int ms)
{
  //Serial.println("Go backward");
  Motor1->run(BACKWARD);
  Motor2->run(FORWARD);
  Motor3->run(BACKWARD);
  Motor4->run(BACKWARD);
  delay(ms);
}

void rotateCCW(int ms)
{
  //Serial.println("Rotate CCW");
  Motor1->run(BACKWARD);
  Motor2->run(BACKWARD);
  Motor3->run(FORWARD);
  Motor4->run(BACKWARD);
  delay(ms);
}

void rotateCW(int ms)
{
  //Serial.println("Rotate CW");
  Motor1->run(FORWARD);
  Motor2->run(FORWARD);
  Motor3->run(BACKWARD);
  Motor4->run(FORWARD);
  delay(ms);
}

void stop(int ms)
{
  Motor1->setSpeed(0);
  Motor2->setSpeed(0);
  Motor3->setSpeed(0);
  Motor4->setSpeed(0);
  Motor1->run(BRAKE);
  Motor2->run(BRAKE);
  Motor3->run(BRAKE);
  Motor4->run(BRAKE);
  delay(ms);
  setup_speed(motor_speed);
}

void go_forward_2m()
{
  //TODO: change ms below
  forward(3550);
  //backward(3000);
  //rotateCCW(3000);
  //rotateCW(3000);
  stop(5000);
}

void go_forward1m_and_rotateCCW()
{
  forward(3636 / 2);
  rotateCCW(385);
  forward(3636 / 2);
  stop(5000);
}
