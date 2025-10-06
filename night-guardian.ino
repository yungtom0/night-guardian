#include <LiquidCrystal.h>  //lcd display library
#include <Servo.h>  //servo operations library

//arduino pins assigned to components
#define bluePin 19
#define redPin 18
#define switchPin 10
#define buzzerPin 8
#define servoPin 9
#define trigPin 12
#define echoPin 11

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);  //lcd to arduino connections
Servo alarm_marker; //servo object declaration

const int day_threshold = 220;  //const between day and night mode
const int night_threshold = 180;  //const between day and night mode
const int distance_threshold = 40;  //const to detect movement
int actual_brightness = 0;  //variable for actual brightness
bool infoMode = false;  //to switch between info and alarm mode


void setup() {
  lcd.begin(16, 2); //lcd type declaration

  alarm_marker.attach(servoPin); //servo to arduino connection

  //defined arduino pins
  pinMode(switchPin, INPUT_PULLUP); //tact switch pin
  pinMode(redPin, OUTPUT);  //red diode pin
  pinMode(bluePin, OUTPUT); //blue diode pin
  pinMode(buzzerPin, OUTPUT); //buzzer pin
  pinMode(trigPin, OUTPUT); //trigger distance sensor pin
  pinMode(echoPin, INPUT);  //echo distance sensor pin
}

void loop() {
  actual_brightness = analogRead(A0); //actual read from photoresistor

  //switch between day and night mode
  if (actual_brightness > day_threshold) { //day mode
    day_mode();
  }
  else if (actual_brightness < night_threshold) {  //night mode
    night_mode();
  }
}

void day_mode() {
  digitalWrite(bluePin, LOW); //blue diode off
  digitalWrite(redPin, LOW);  //red diode off
  digitalWrite(buzzerPin, LOW);  //buzzer off

  alarm_marker.write(0);  //move servo to sleep position

  int distance = distance_measurement();  //assign actual distance to variable

  //upper line message
  lcd.setCursor(0, 0);
  lcd.print("Day mode        ");

  //lower line message
  lcd.setCursor(0, 1);
  lcd.print("Br:");
  lcd.print(actual_brightness); //display actual brightness
  lcd.print(" Dst:");
  lcd.print(distance);  //display actual distance
  lcd.print("cm  ");

  delay(300);
}

void night_mode() {
  int distance = distance_measurement();  //assign actual distance to variable

  if (digitalRead(switchPin) == LOW) infoMode = !infoMode;  //check which night mode should be displayed

  if (infoMode) {
    //night info mode
    digitalWrite(bluePin, LOW); //blue diode off
    digitalWrite(redPin, LOW);  //red diode off
    digitalWrite(buzzerPin, LOW);  //buzzer off

    alarm_marker.write(0);  //move servo to sleep position

    lcd.setCursor(0, 0);  //upper line message
    lcd.print("Night mode        ");
    lcd.setCursor(0, 1);  //lower line message
    lcd.print("Br:");
    lcd.print(actual_brightness); //display actual brightness
    lcd.print(" Dst:");
    lcd.print(distance);  //display actual distance
    lcd.print("cm  ");
  }
  else {
    //night alarm mode
    alarm_marker.write(90); //move servo to standby position

    if (distance <= distance_threshold) { //when movement is detected
      digitalWrite(bluePin, LOW); //blue diode off

      digitalWrite(buzzerPin, HIGH);  //buzzer on
      digitalWrite(redPin, HIGH); //red diode on
      delay(200);
      digitalWrite(buzzerPin, LOW);  //buzzer off
      digitalWrite(redPin, LOW);

      lcd.setCursor(0, 0);  //upper line message
      lcd.print("WARNING!        ");
      lcd.setCursor(0, 1);  //lower line message
      lcd.print("Move detected   ");
    }
    else {  //no movement
      digitalWrite(bluePin, HIGH); //blue diode on
      digitalWrite(redPin, LOW);  //red diode off
      digitalWrite(buzzerPin, LOW);  //buzzer off
      lcd.setCursor(0, 0);  //upper line message
      lcd.print("Night mode      ");
      lcd.setCursor(0, 1);  //lower line message
      lcd.print("No move detected");
    }
  }

  delay(300);
}

int distance_measurement() {
  //distance sensor measurement
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  int distance = pulseIn(echoPin, HIGH) / 58;  //distance converted to cm
  return distance;
}
