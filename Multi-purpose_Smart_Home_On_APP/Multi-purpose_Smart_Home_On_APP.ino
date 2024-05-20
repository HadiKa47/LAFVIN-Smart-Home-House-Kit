
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

Servo servo_9;
int Button_Red;
int Button_Green;
int Red_num_time;
String password;
LiquidCrystal_I2C mylcd(0x27,16,2);
int water_level;
int carch;
int pass;
int Environmental_sound;
int Brightness;
int Soil_moisture;
int temperature;
int humidity;
int flame;
int door_flag;
String BLE_value;
int relay_close_led_flag;
int relay_close_led_count;
boolean key_door_flag;
int last_soil;
int soil_low_count;
int BLE_door_flag;
int BLE_car_flag;
int re_string_len;
String app_password;
String sensor_data;
int BLE_LED_flag;
int BLE_window_flag;
int BLE_fan_flag;
Servo servo_10;
Servo servo_11;
DHT dht12(12, 11);

void open_door() {
  Button_Green = digitalRead(4);
  Button_Red = digitalRead(5);
  if (0 != Button_Green && 0 == Button_Red) {
    delay(100);
    Button_Green = digitalRead(4);
    while (0 != Button_Green && 0 == Button_Red) {
      Button_Red = digitalRead(5);
      Red_num_time = Red_num_time + 1;
      delay(100);
    }

  }
  if (1 < Red_num_time && 5 > Red_num_time) {
    key_voice();
    password = String(password) + String(".");
    mylcd.clear();
    mylcd.setCursor(1-1, 1-1);
    mylcd.print("   Smart Home");
    mylcd.setCursor(1-1, 2-1);
    mylcd.print("Password:");
    mylcd.setCursor(11-1, 2-1);
    mylcd.print(password);

  }
  if (5 < Red_num_time) {
    key_voice();
    password = String(password) + String("-");
    mylcd.clear();
    mylcd.setCursor(1-1, 1-1); //note
    mylcd.print("   Smart Home");
    mylcd.setCursor(1-1, 2-1);
    mylcd.print("Password:");
    mylcd.setCursor(11-1, 2-1);
    mylcd.print(password);

  }
  if (0 == Button_Green && 0 != Button_Red) {
    delay(100);
    Button_Red = digitalRead(5);
    if (0 == Button_Green && 0 != Button_Red) {
      if (false == key_door_flag) {
        if (password == ".--.") {
          mylcd.clear();
          mylcd.setCursor(1-1, 1-1);
          mylcd.print("   Smart Home");
          mylcd.setCursor(1-1, 2-1);
          mylcd.print("Password:");
          mylcd.setCursor(11-1, 2-1);
          mylcd.print("Right");
          servo_9.write(100); //edit it as 180
          delay(0);
          door_flag = 0;
          key_door_flag = true;

        } else {
          mylcd.clear();
          mylcd.setCursor(1-1, 1-1);
          mylcd.print("   Smart Home");
          mylcd.setCursor(1-1, 2-1);
          mylcd.print("Password:");
          mylcd.setCursor(11-1, 2-1);
          mylcd.print("Error");
          tone(3,165);
          delay(125);
          delay(500);
          noTone(3);
          delay(200);
          mylcd.clear();
          mylcd.setCursor(1-1, 1-1);
          mylcd.print("   Smart Home");
          mylcd.setCursor(11-1, 2-1);
          mylcd.print("Again ");
          mylcd.setCursor(1-1, 2-1);
          mylcd.print("Password:");
          key_voice();

        }

      } else if (true == key_door_flag) {
        key_door_flag = false;
        mylcd.clear();
        mylcd.setCursor(1-1, 1-1);
        mylcd.print("   Smart Home");
        servo_9.write(0); //edit it was 90
        delay(0);
      }
      password = "";

    }

  }
  Red_num_time = 0;
}

void fan_motor_pwm(int speedpin, int speed)
{
  if (speed <= 0)
  {
    analogWrite(speedpin, 0);
  }
  else if (speed > 255)
  {
    analogWrite(speedpin, 255);
  }
  else
  {
    analogWrite(speedpin,speed);
  }
}

void auto_process() {
  water_level = analogRead(A2);
  if (100 < water_level) {
    servo_10.write(90);
    delay(0);

  } else if (BLE_window_flag != 1) {
    servo_10.write(0);
    delay(0);
  }
  pass = digitalRead(A0);
  if (pass == 0) {
    servo_11.write(180);
    delay(0);

  } else if (BLE_car_flag != 1 && pass == 1) {
    servo_11.write(85);
    delay(0);
  }
  Brightness = analogRead(A1);
  Environmental_sound = digitalRead(2);
  if (Brightness > 500) {
    if (Environmental_sound == 1) {
      digitalWrite(7,HIGH);
      relay_close_led_flag = 1;

    } else {
      if (relay_close_led_flag == 1) {
        relay_close_led_count = relay_close_led_count + 1;
        if (relay_close_led_count >= 5000) {
          digitalWrite(7,LOW);
          relay_close_led_count = 0;
          relay_close_led_flag = 0;

        }

      }

    }

  } else {
    if (BLE_LED_flag != 1) {
      digitalWrite(7,LOW);

    }

  }
  Soil_moisture = analogRead(A3);
  if (50 > Soil_moisture) {
    if (50 < last_soil) {
      Watering_warning();

    } else if (12000 <= soil_low_count) {
      Watering_warning();
      soil_low_count = 0;
    } else {
      soil_low_count = soil_low_count + 1;

    }
    last_soil = Soil_moisture;

  } else {
    soil_low_count = 0;
    last_soil = Soil_moisture;

  }
  temperature = dht12.readTemperature();
  humidity = dht12.readHumidity();
  flame = digitalRead(8);
  if (1 == flame || 31 < temperature) {
    fan_motor_pwm(6, 120);

  } else {
    if (BLE_fan_flag != 1) {
      fan_motor_pwm(6, 0);

    }

  }
  open_door();
}

void Watering_warning() {
  for (int i = 1; i <= 2; i = i + (1)) {
    digitalWrite(13,HIGH);
    tone(3,532);
    delay(125);
    delay(50);
    digitalWrite(13,LOW);
    noTone(3);
    delay(50);
  }
}

void key_voice() {
  tone(3,349);
  delay(125);
  delay(100);
  noTone(3);
  delay(100);
}

void setup(){
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  servo_9.attach(9);
  pinMode(3, OUTPUT);
  Button_Red = 1;
  Button_Green = 1;
  Red_num_time = 0;
  password = "";
  mylcd.init();
  mylcd.backlight();
  water_level = 0;
  carch = 0;
  pass = 0;
  Environmental_sound = 0;
  Brightness = 0;
  pinMode(7, OUTPUT);
  Soil_moisture = 0;
  temperature = 0;
  humidity = 0;
  flame = 0;
  Serial.begin(9600);
  door_flag = 0;
  BLE_value = "Empty";
  relay_close_led_flag = 0;
  relay_close_led_count = 0;
  key_door_flag = false;
  last_soil = 55;
  soil_low_count = 0;
  BLE_door_flag = 0;
  BLE_car_flag = 0;
  re_string_len = 0;
  app_password = ' ';
  sensor_data = ' ';
  BLE_LED_flag = 0;
  BLE_window_flag = 0;
  BLE_fan_flag = 0;
  servo_10.attach(10);
  servo_11.attach(11);
  mylcd.setCursor(1-1, 1-1);
  mylcd.print("   Smart Home");
  digitalWrite(7,LOW);
  servo_9.write(90);
  delay(0);
  servo_10.write(0);
  delay(0);
  servo_11.write(90);
  delay(0);
  pinMode(A2, INPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(2, INPUT);
  pinMode(A3, INPUT);
   dht12.begin();
  pinMode(8, INPUT);
  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);
  pinMode(13, OUTPUT);
}

void loop(){
  while (true) {
    auto_process();
    if (Serial.available() > 0) {
      BLE_value = Serial.readString();
      if (12 > String(BLE_value).length()) {
        if ('%' == String(BLE_value).charAt(0) && '#' == String(BLE_value).charAt(2)) {
          switch (String(BLE_value).charAt(1)) {
           case 'Q':
            re_string_len = String(BLE_value).length();
            app_password = String(BLE_value).substring(3,(re_string_len - 2));
            if (app_password == "1234") {
              servo_9.write(180);
              delay(0);
              Serial.println("%A#");

            } else {
              Serial.println("%B#");

            }
            break;
           case 'K':
            sensor_data = String("%") + String(String(Brightness) + String(String("%") + String(String(String(temperature)) + String(String("%") + String(String(String(humidity)) + String(String("%") + String(String(String(Soil_moisture)) + String(String("%") + String(String(String(water_level)) + String("%"))))))))));
            Serial.println(sensor_data);
            break;
           case 'A':
            BLE_LED_flag = 1;
            digitalWrite(7,HIGH);
            break;
           case 'B':
            BLE_LED_flag = 0;
            digitalWrite(7,LOW);
            break;
           case 'C':
            BLE_door_flag = 1;
            servo_9.write(100); //Edit to 100 its was 180 
            delay(0);
            break;
           case 'D':
            BLE_door_flag = 0;
            servo_9.write(0); //Edit to 0 its was 90
            delay(0);
            break;
           case 'E':
            BLE_fan_flag = 1;
            fan_motor_pwm(6, 120);
            break;
           case 'F':
            BLE_fan_flag = 0;
            fan_motor_pwm(6, 0);
            break;
           case 'I':
            BLE_car_flag = 1;
            servo_11.write(180);
            delay(0);
            break;
           case 'J':
            BLE_car_flag = 0;
            servo_11.write(90);
            delay(0);
            break;
           case 'L':
            BLE_window_flag = 0;
            servo_10.write(0);
            delay(0);
            break;
           case 'M':
            BLE_window_flag = 1;
            servo_10.write(90);
            delay(0);
            break;
          }
          BLE_value = "Empty";

        } else {
          BLE_value = "Empty";

        }

      }

    }
  }

}
