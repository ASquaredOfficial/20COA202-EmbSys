#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

typedef enum floor_e {ground, first, outside} floor_t;
typedef enum room0_e {garage, garden} room0_t;
typedef enum room1_e {exitError, hall, livingRoom, kitchen} room1_t;
typedef enum room2_e {bedroom1, bedroom2, bathroom} room2_t;
typedef enum typeAction_e {defaultType, heat, heat_on, heat_off, light, light_on, light_off, 
                            lamp, lamp_on, lamp_off, water, water_on, water_off} typeAction_t;
typedef enum changeLevel_e {stationary, decrease, increase} changeLevel_t;

//Check if a time value is below 10
void resetDevice();
void ifzero_MM(int number);
void ifzero_HH(int number);
void printCase(int number);
int increase_time_MM(int HH, int MM);
int increase_time_HH(int HH, int MM);
int decrease_time_MM(int HH, int MM);
int decrease_time_HH(int HH, int MM);
int freeMemory();

//Light, Heat, Water and Lamp Levels
int bright0 = 50;        int temp0 = 50;       int bulb0 = 50;   //Hall
int bright1 = 50;        int temp1 = 50;       int bulb1 = 50;   //Living Room
int bright2 = 50;        int temp2 = 50;       int bulb2 = 50;   //Kitchen
int bright3 = 50;        int temp3 = 50;       int bulb3 = 50;   //Bedroom 1 
int bright4 = 50;        int temp4 = 50;       int bulb4 = 50;   //Bedroom 2 
int bright5 = 50;        int temp5 = 50;       int bulb5 = 50;   //Bathroom
int bright6 = 50;        //Garage Light
int power7 = 50;         //Garden Hose

//On and Off Times
int heat_on_HH[6] = {18, 18, 18, 21, 21, 22};         int heat_on_MM[6] = {15, 30, 30, 45, 45, 00};
int heat_off_HH[6] = {22, 21, 19, 22, 22, 22};        int heat_off_MM[6] = {00, 00, 45, 30, 30, 15};
int light_on_HH[7] = {18, 18, 18, 21, 21, 22, 19};    int light_on_MM[7] = {15, 30, 30, 45, 45, 00, 00};
int light_off_HH[7] = {22, 22, 19, 22, 22, 22, 20};   int light_off_MM[7] = {00, 00, 45, 30, 30, 15, 00};
int water_on_HH = 9;                                  int water_on_MM = 00;
int water_off_HH = 10;                                int water_off_MM = 30;
int lamp_on_HH[6] = {00, 21, 00, 22, 22, 00};         int lamp_on_MM[6] = {00, 00, 00, 30, 30, 00};
int lamp_off_HH[6] = {00, 22, 00, 23, 23, 00};        int lamp_off_MM[6] = {00, 00, 00, 00, 00, 00};

floor_t floors = ground;
typeAction_t type = defaultType;
room1_t room1 = hall;
room2_t room2 = bedroom1;
room0_t room0 = garage;
changeLevel_t level = stationary;
  
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("ENHANCED");
  Serial.println("\nFloor/Room/Type/Name/Action:VAL");
  Serial.println("-------------------------------------------");
  printCase(888);
  printCase(00);
  lcd.begin(16, 2);
  lcd.print("BEGIN");
  lcd.setBacklight(7);
}

void loop() {
  uint8_t buttons = lcd.readButtons();
  if (Serial.available()>0){
    char x = Serial.read();
    if (x == 'Q'){
      Serial.print('\n');
      lcd.setBacklight(8);
      printCase(888);
      lcd.setBacklight(7);
    } 
    if (x == 'M'){
      Serial.print("Memory Left : ");
      int y = freeMemory();
      Serial.println(y);
      lcd.clear();
    }
  }
  
  int tempMM1;
  int tempMM2;
  int count = 0;

  switch (floors) {
    case ground:
      switch (room1) {
        case exitError:
          if (count == 0) {
            room1 = hall;
            delay(150);
            count++;
          }
          break;
        case hall:
          switch (type) {
            case defaultType:
            //case 00
              if (buttons & BUTTON_RIGHT) {
                room1 = livingRoom;
                printCase(10);
              } if (buttons & BUTTON_LEFT) {
                room1 = exitError;
                Serial.println("Invalid input!! \nGround/Hall");
                count = 0;
              } if (buttons & BUTTON_UP) {
                floors = first;
                room2 = bedroom1;
                printCase(30);
                count = 0;
              } if (buttons & BUTTON_DOWN) {
                floors = outside;
                room0 = garage;
                printCase(60);
              } if (buttons & BUTTON_SELECT) {
                type = heat;
                printCase(01);
              }
              delay(150);
              break;
            case heat:
            //case 01
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_RIGHT) {
                    type = heat_on;
                    printCase(02);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(00);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case (increase):
                  if ((temp0 < 100) && (temp0 >= 0)) {
                    temp0 = temp0 + 5;
                    printCase(01);
                  }
                  level = stationary;
                  break;
                case (decrease):
                  if ((temp0 <= 100) && (temp0 > 0)) {
                    temp0 = temp0 - 5;
                    printCase(01);
                  }
                  level = stationary;
                  break;
              }
              break;
            case heat_on:
            //case 02
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = heat;
                    printCase(01);
                  } if (buttons & BUTTON_RIGHT) {
                    type = heat_off;
                    printCase(03);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(00);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = heat_on_MM[0];
                  heat_on_MM[0] = increase_time_MM(heat_on_HH[0], heat_on_MM[0]);
                  heat_on_HH[0] = increase_time_HH(heat_on_HH[0], tempMM1);
                  printCase(02);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = heat_on_MM[0];
                  heat_on_MM[0] = decrease_time_MM(heat_on_HH[0], heat_on_MM[0]);
                  heat_on_HH[0] = decrease_time_HH(heat_on_HH[0], tempMM2);
                  printCase(02);
                  level = stationary;
                  break;
              }                  
              break;
            case heat_off:
            //case 03
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = heat_on;
                    printCase(02);
                  } if (buttons & BUTTON_RIGHT) {
                    type = light;
                    printCase(04);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(00);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = heat_off_MM[0];
                  heat_off_MM[0] = increase_time_MM(heat_off_HH[0], heat_off_MM[0]);
                  heat_off_HH[0] = increase_time_HH(heat_off_HH[0], tempMM1);
                  printCase(03);                  
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = heat_off_MM[0];
                  heat_off_MM[0] = decrease_time_MM(heat_off_HH[0], heat_off_MM[0]);
                  heat_off_HH[0] = decrease_time_HH(heat_off_HH[0], tempMM2);
                  printCase(03);
                  level = stationary;
                  break;
              }
              break;
            case light:
            //case 04
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = heat_off;
                    printCase(03);
                  } if (buttons & BUTTON_RIGHT) {
                    type = light_on;
                    printCase(05);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(00);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case (increase):
                  if ((bright0 < 100) && (bright0 >= 0)) {
                    bright0 = bright0 + 5;
                    printCase(04);
                  }
                  level = stationary;
                  break;
                case (decrease):
                  if ((temp0 <= 100) && (temp0 > 0)) {
                    bright0 = bright0 - 5;
                    printCase(04);
                  }
                  level = stationary;
                  break;
              }
              break;
            case light_on:
            //case 05
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = light;
                    printCase(04);
                  } if (buttons & BUTTON_RIGHT) {
                    type = light_off;
                    printCase(06);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(00);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = light_on_MM[0];
                  light_on_MM[0] = increase_time_MM(light_on_HH[0], light_on_MM[0]);
                  light_on_HH[0] = increase_time_HH(light_on_HH[0], tempMM1);
                  printCase(05);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = light_on_MM[0];
                  light_on_MM[0] = decrease_time_MM(light_on_HH[0], light_on_MM[0]);
                  light_on_HH[0] = decrease_time_HH(light_on_HH[0], tempMM2);
                  printCase(05);
                  level = stationary;
                  break;
              }
              break;
            case light_off:
            //case 06
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = light_on;
                    printCase(05);
                  } if (buttons & BUTTON_RIGHT) {
                    type = lamp;
                    printCase(07);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(00);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = light_off_MM[0];
                  light_off_MM[0] = increase_time_MM(light_off_HH[0], light_off_MM[0]);
                  light_off_HH[0] = increase_time_HH(light_off_HH[0], tempMM1);
                  printCase(06);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = light_off_MM[0];
                  light_off_MM[0] = decrease_time_MM(light_off_HH[0], light_off_MM[0]);
                  light_off_HH[0] = decrease_time_HH(light_off_HH[0], tempMM2);
                  printCase(06);
                  level = stationary;
                  break;
              }
              break;
            case lamp:
            //case 07
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = light_off;
                    printCase(06);
                  } if (buttons & BUTTON_RIGHT) {
                    type = lamp_on;
                    printCase(8);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(00);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case (increase):
                  if ((bulb0 < 100) && (bulb0 >= 0)) {
                    bulb0 = bulb0 + 5;
                    printCase(7);
                  }
                  level = stationary;
                  break;
                case (decrease):
                  if ((bulb0 <= 100) && (bulb0 > 0)) {
                    bulb0 = bulb0 - 5;
                    printCase(7);
                  }
                  level = stationary;
                  break;
              }
              break;
            case lamp_on:
            //case 08
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = lamp;
                    printCase(7);
                  } if (buttons & BUTTON_RIGHT) {
                    type = lamp_off;
                    printCase(9);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(00);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = lamp_on_MM[0];
                  lamp_on_MM[0] = increase_time_MM(lamp_on_HH[0], lamp_on_MM[0]);
                  lamp_on_HH[0] = increase_time_HH(lamp_on_HH[0], tempMM1);
                  printCase(8);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = lamp_on_MM[0];
                  lamp_on_MM[0] = decrease_time_MM(lamp_on_HH[0], lamp_on_MM[0]);
                  lamp_on_HH[0] = decrease_time_HH(lamp_on_HH[0], tempMM2);
                  printCase(8);
                  level = stationary;
                  break;
              }
              break;
            case lamp_off:
            //case 09
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = lamp_on;
                    printCase(8);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(00);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = lamp_off_MM[0];
                  lamp_off_MM[0] = increase_time_MM(lamp_off_HH[0], lamp_off_MM[0]);
                  lamp_off_HH[0] = increase_time_HH(lamp_off_HH[0], tempMM1);
                  printCase(9);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = lamp_off_MM[0];
                  lamp_off_MM[0] = decrease_time_MM(lamp_off_HH[0], lamp_off_MM[0]);
                  lamp_off_HH[0] = decrease_time_HH(lamp_off_HH[0], tempMM2);
                  printCase(9);
                  level = stationary;
                  break;
              }
              break;
          }
          break;
        case (livingRoom):
          switch (type) {    
            case defaultType:
            //case 10
              if (buttons & BUTTON_RIGHT) {
                room1 = kitchen;
                printCase(20);
              } if (buttons & BUTTON_LEFT) {
                room1 = hall;
                printCase(00);
              } if (buttons & BUTTON_UP) {
                floors = first;
                room2 = bedroom1;
                printCase(30);
              } if (buttons & BUTTON_DOWN) {
                floors = outside;
                room0 = garage;
                printCase(60);
              } if (buttons & BUTTON_SELECT) {
                type = heat;
                printCase(11);
              }
              delay(150);
              break;
            case (heat):
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_RIGHT) {
                    type = heat_on;
                    printCase(12);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(10);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;                    
                  }
                  delay(150);
                  break;
                case (increase):
                  if ((temp1 < 100) && (temp1 >= 0)) {
                    temp1 = temp1 + 5;
                    printCase(11);
                  }
                  level = stationary;
                  break;
                case (decrease):
                  if ((temp1 <= 100) && (temp1 > 0)) {
                    temp1 = temp1 - 5;
                    printCase(11);
                  }
                  level = stationary;
                  break;
              }
              break;
            case (heat_on):
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = heat;
                    printCase(11);
                  } if (buttons & BUTTON_RIGHT) {
                    type = heat_off;
                    printCase(13);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(10);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = heat_on_MM[1];
                  heat_on_MM[1] = increase_time_MM(heat_on_HH[1], heat_on_MM[1]);
                  heat_on_HH[1] = increase_time_HH(heat_on_HH[1], tempMM1);
                  printCase(12);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = heat_on_MM[1];
                  heat_on_MM[1] = decrease_time_MM(heat_on_HH[1], heat_on_MM[1]);
                  heat_on_HH[1] = decrease_time_HH(heat_on_HH[1], tempMM2);
                  printCase(12);
                  level = stationary;
                  break;
              }         
              break;
            case (heat_off):
            //case 13
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = heat_on;
                    printCase(12);
                  } if (buttons & BUTTON_RIGHT) {
                    type = light;
                    printCase(14);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(10);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = heat_off_MM[1];
                  heat_off_MM[1] = increase_time_MM(heat_off_HH[1], heat_off_MM[1]);
                  heat_off_HH[1] = increase_time_HH(heat_off_HH[1], tempMM1);
                  printCase(13);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = heat_off_MM[1];
                  heat_off_MM[1] = decrease_time_MM(heat_off_HH[1], heat_off_MM[1]);
                  heat_off_HH[1] = decrease_time_HH(heat_off_HH[1], tempMM2);
                  printCase(13);
                  level = stationary;
                  break;
              }
              break;
            case (light):
            //case 14
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = heat_off;
                    printCase(13);
                  } if (buttons & BUTTON_RIGHT) {
                    type = light_on;
                    printCase(15);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(10);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case (increase):
                  if ((bright1 < 100) && (bright1 >= 0)) {
                    bright1 = bright1 + 5;
                    printCase(14);
                  }
                  level = stationary;
                  break;
                case (decrease):
                  if ((bright1 <= 100) && (bright1 > 0)) {
                    bright1 = bright1 - 5;
                    printCase(14);
                  }
                  level = stationary;
                  break;
              }
              break;
            case (light_on):
            //case 15
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = light;
                    printCase(14);
                  } if (buttons & BUTTON_RIGHT) {
                    type = light_off;
                    printCase(16);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(10);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = light_on_MM[1];
                  light_on_MM[1] = increase_time_MM(light_on_HH[1], light_on_MM[1]);
                  light_on_HH[1] = increase_time_HH(light_on_HH[1], tempMM1);
                  printCase(15);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = light_on_MM[1];
                  light_on_MM[1] = decrease_time_MM(light_on_HH[1], light_on_MM[1]);
                  light_on_HH[1] = decrease_time_HH(light_on_HH[1], tempMM2);
                  printCase(15);
                  level = stationary;
                  break;
              }
              break;
            case (light_off):
            //case 16
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = light_on;
                    printCase(15);
                  } if (buttons & BUTTON_RIGHT) {
                    type = lamp;
                    printCase(17);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(10);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = light_off_MM[1];
                  light_off_MM[1] = increase_time_MM(light_off_HH[1], light_off_MM[1]);
                  light_off_HH[1] = increase_time_HH(light_off_HH[1], tempMM1);
                  printCase(16);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = light_off_MM[1];
                  light_off_MM[1] = decrease_time_MM(light_off_HH[1], light_off_MM[1]);
                  light_off_HH[1] = decrease_time_HH(light_off_HH[1], tempMM2);
                  printCase(16);
                  level = stationary;
                  break;
              }
              break;
            case lamp:
            //case 17
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = light_off;
                    printCase(16);
                  } if (buttons & BUTTON_RIGHT) {
                    type = lamp_on;
                    printCase(18);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(10);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case (increase):
                  if ((bulb1 < 100) && (bulb1 >= 0)) {
                    bulb1 = bulb1 + 5;
                    printCase(17);
                  }
                  level = stationary;
                  break;
                case (decrease):
                  if ((bulb1 <= 100) && (bulb1 > 0)) {
                    bulb1 = bulb1 - 5;
                    printCase(17);
                  }
                  level = stationary;
                  break;
              }
              break;
            case lamp_on:
            //case 18
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = lamp;
                    printCase(17);
                  } if (buttons & BUTTON_RIGHT) {
                    type = lamp_off;
                    printCase(19);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(10);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = lamp_on_MM[1];
                  lamp_on_MM[1] = increase_time_MM(lamp_on_HH[1], lamp_on_MM[1]);
                  lamp_on_HH[1] = increase_time_HH(lamp_on_HH[1], tempMM1);
                  printCase(18);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = lamp_on_MM[1];
                  lamp_on_MM[1] = decrease_time_MM(lamp_on_HH[1], lamp_on_MM[1]);
                  lamp_on_HH[1] = decrease_time_HH(lamp_on_HH[1], tempMM2);
                  printCase(18);
                  level = stationary;
                  break;
              }
              break;
            case lamp_off:
            //case 19
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = lamp_on;
                    printCase(18);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(10);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = lamp_off_MM[1];
                  lamp_off_MM[1] = increase_time_MM(lamp_off_HH[1], lamp_off_MM[1]);
                  lamp_off_HH[1] = increase_time_HH(lamp_off_HH[1], tempMM1);
                  printCase(19);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = lamp_off_MM[1];
                  lamp_off_MM[1] = decrease_time_MM(lamp_off_HH[1], lamp_off_MM[1]);
                  lamp_off_HH[1] = decrease_time_HH(lamp_off_HH[1], tempMM2);
                  printCase(19);
                  level = stationary;
                  break;
              }
              break;
          }
          break;
        case kitchen:
        //case 20
          switch (type) {
            case (defaultType):
              if (buttons & BUTTON_LEFT) {
                room1 = livingRoom;
                printCase(10);
                break;
              } else if (buttons & BUTTON_UP) {
                floors = first;
                room2 = bedroom1;
                printCase(30);
              } else if (buttons & BUTTON_DOWN) {
                floors = outside;
                room0 = garage;
                printCase(60);          
              } else if (buttons & BUTTON_SELECT) {
                type = heat;
                printCase(21);
              }
              delay(150);
              break;
            case (heat):
            //case 21
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_RIGHT) {
                    type = heat_on;
                    printCase(22);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(20);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case (increase):
                  if ((temp2 < 100) && (temp2 >= 0)) {
                    temp2 = temp2 + 5;
                    printCase(21);
                  }
                  level = stationary;
                  break;
                case (decrease):
                  if ((temp2 <= 100) && (temp2 > 0)) {
                    temp2 = temp2 - 5;
                    printCase(21);
                  }
                  level = stationary;
                  break;
              }
              break;
            case (heat_on):
            //case 22
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = heat;
                    printCase(21);
                  } if (buttons & BUTTON_RIGHT) {
                    type = heat_off;
                    printCase(23);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(20);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = heat_on_MM[2];
                  heat_on_MM[2] = increase_time_MM(heat_on_HH[2], heat_on_MM[2]);
                  heat_on_HH[2] = increase_time_HH(heat_on_HH[2], tempMM1);
                  printCase(22);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = heat_on_MM[2];
                  heat_on_MM[2] = decrease_time_MM(heat_on_HH[2], heat_on_MM[2]);
                  heat_on_HH[2] = decrease_time_HH(heat_on_HH[2], tempMM2);
                  printCase(22);
                  level = stationary;
                  break;
              } 
              break;
            case (heat_off):
            //case 23
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = heat_on;
                    printCase(22);
                  } if (buttons & BUTTON_RIGHT) {
                    type = light;
                    printCase(24);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(20);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = heat_off_MM[2];
                  heat_off_MM[2] = increase_time_MM(heat_off_HH[2], heat_off_MM[2]);
                  heat_off_HH[2] = increase_time_HH(heat_off_HH[2], tempMM1);
                  printCase(23);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = heat_off_MM[2];
                  heat_off_MM[2] = decrease_time_MM(heat_off_HH[2], heat_off_MM[2]);
                  heat_off_HH[2] = decrease_time_HH(heat_off_HH[2], tempMM2);
                  printCase(23);
                  level = stationary;
                  break;
              }
              break;
            case (light):
            //case 24
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = heat_off;
                    printCase(23);
                  } if (buttons & BUTTON_RIGHT) {
                    type = light_on;
                    printCase(25);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(20);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case (increase):
                  if ((bright2 < 100) && (bright2 >= 0)) {
                    bright2 = bright2 + 5;
                    printCase(24);
                  }
                  level = stationary;
                  break;
                case (decrease):
                  if ((bright2 <= 100) && (bright2 > 0)) {
                    bright2 = bright2 - 5;
                    printCase(24);
                  }
                  level = stationary;
                  break;
              }
              break;
            case (light_on):
            //case 25
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = light;
                    printCase(24);
                  } if (buttons & BUTTON_RIGHT) {
                    type = light_off;
                    printCase(26);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(20);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = light_on_MM[2];
                  light_on_MM[2] = increase_time_MM(light_on_HH[2], light_on_MM[2]);
                  light_on_HH[2] = increase_time_HH(light_on_HH[2], tempMM1);
                  printCase(25);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = light_on_MM[0];
                  light_on_MM[2] = decrease_time_MM(light_on_HH[2], light_on_MM[2]);
                  light_on_HH[2] = decrease_time_HH(light_on_HH[2], tempMM2);
                  printCase(25);
                  level = stationary;
                  break;
              }
              break;
            case (light_off):
            //case 26
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = light_on;
                    printCase(25);
                  } if (buttons & BUTTON_RIGHT) {
                    type = lamp;
                    printCase(27);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(20);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = light_off_MM[2];
                  light_off_MM[2] = increase_time_MM(light_off_HH[2], light_off_MM[2]);
                  light_off_HH[2] = increase_time_HH(light_off_HH[2], tempMM1);
                  printCase(26);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = light_off_MM[2];
                  light_off_MM[2] = decrease_time_MM(light_off_HH[2], light_off_MM[2]);
                  light_off_HH[2] = decrease_time_HH(light_off_HH[2], tempMM2);
                  printCase(26);
                  level = stationary;
                  break;
              }
              break;
            case lamp:
            //case 27
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = light_off;
                    printCase(26);
                  } if (buttons & BUTTON_RIGHT) {
                    type = lamp_on;
                    printCase(28);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(20);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case (increase):
                  if ((bulb2 < 100) && (bulb2 >= 0)) {
                    bulb2 = bulb2 + 5;
                    printCase(27);
                  }
                  level = stationary;
                  break;
                case (decrease):
                  if ((bulb2 <= 100) && (bulb2 > 0)) {
                    bulb2 = bulb2 - 5;
                    printCase(27);
                  }
                  level = stationary;
                  break;
              }
              break;
            case lamp_on:
            //case 28
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = lamp;
                    printCase(27);
                  } if (buttons & BUTTON_RIGHT) {
                    type = lamp_off;
                    printCase(29);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(20);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = lamp_on_MM[2];
                  lamp_on_MM[2] = increase_time_MM(lamp_on_HH[2], lamp_on_MM[2]);
                  lamp_on_HH[2] = increase_time_HH(lamp_on_HH[2], tempMM1);
                  printCase(28);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = lamp_on_MM[2];
                  lamp_on_MM[2] = decrease_time_MM(lamp_on_HH[2], lamp_on_MM[2]);
                  lamp_on_HH[2] = decrease_time_HH(lamp_on_HH[2], tempMM1);
                  printCase(28);
                  level = stationary;
                  break;
              }
              break;
            case lamp_off:
            //case 29
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = lamp_on;
                    printCase(28);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(20);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = lamp_off_MM[2];
                  lamp_off_MM[2] = increase_time_MM(lamp_off_HH[2], lamp_off_MM[2]);
                  lamp_off_HH[2] = increase_time_HH(lamp_off_HH[2], tempMM1);
                  printCase(29);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = lamp_off_MM[2];
                  lamp_off_MM[2] = decrease_time_MM(lamp_off_HH[2], lamp_off_MM[2]);
                  lamp_off_HH[2] = decrease_time_HH(lamp_off_HH[2], tempMM2);
                  printCase(29);
                  level = stationary;
                  break;
              }
              break;
          }
          break;
      }
      break;
    case (first):
      switch (room2) {
        case bedroom1:
        //case 30
          switch (type) {
            case (defaultType):
              if (buttons & BUTTON_RIGHT) {
                room2 = bedroom2;
                printCase(40);
              } if (buttons & BUTTON_DOWN) {
                floors = ground;
                room1 = hall;
                printCase(00);
              } if (buttons & BUTTON_SELECT) {
                type = heat;
                printCase(31);
              }
              delay(150);
              break;
            case (heat):
            //case 31
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_RIGHT) {
                    type = heat_on;
                    printCase(32);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(30);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case (increase):
                  if ((temp3 < 100) && (temp3 >= 0)) {
                    temp3 = temp3 + 5;
                    printCase(31);
                  }
                  level = stationary;
                  break;
                case (decrease):
                  if ((temp3 <= 100) && (temp3 > 0)) {
                    temp3 = temp3 - 5;
                    printCase(31);
                  }
                  level = stationary;
                  break;
              }
              break;
            case (heat_on):
            //case 32
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = heat;
                    printCase(31);
                  } if (buttons & BUTTON_RIGHT) {
                    type = heat_off;
                    printCase(33);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(30);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = heat_on_MM[3];
                  heat_on_MM[3] = increase_time_MM(heat_on_HH[3], heat_on_MM[3]);
                  heat_on_HH[3] = increase_time_HH(heat_on_HH[3], tempMM1);
                  printCase(32);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = heat_on_MM[3];
                  heat_on_MM[3] = decrease_time_MM(heat_on_HH[3], heat_on_MM[3]);
                  heat_on_HH[3] = decrease_time_HH(heat_on_HH[3], tempMM2);
                  printCase(32);
                  level = stationary;
                  break;
              }  
              break;
            case (heat_off):
            //case 33
              switch (level) {
                case (stationary):
                 if (buttons & BUTTON_LEFT) {
                    type = heat_on;
                    printCase(32);
                  } if (buttons & BUTTON_RIGHT) {
                    type = light;
                    printCase(34);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(30);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = heat_off_MM[3];
                  heat_off_MM[3] = increase_time_MM(heat_off_HH[3], heat_off_MM[3]);
                  heat_off_HH[3] = increase_time_HH(heat_off_HH[3], tempMM1);
                  printCase(33);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = heat_off_MM[3];
                  heat_off_MM[3] = decrease_time_MM(heat_off_HH[3], heat_off_MM[3]);
                  heat_off_HH[3] = decrease_time_HH(heat_off_HH[3], tempMM2);
                  printCase(33);
                  level = stationary;
                  break;
              }
              break;
            case (light):
            //case 34
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = heat_off;
                    printCase(33);
                  } if (buttons & BUTTON_RIGHT) {
                    type = light_on;
                    printCase(35);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(30);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case (increase):
                  if ((bright3 < 100) && (bright3 >= 0)) {
                    bright3 = bright3 + 5;
                    printCase(34);
                  }
                  level = stationary;
                  break;
                case (decrease):
                  if ((bright3 <= 100) && (bright3 > 0)) {
                    bright3 = bright3 - 5;
                    printCase(34);
                  }
                  level = stationary;
                  break;
              }
              break;
            case (light_on):
            //case 35
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = light;
                    printCase(34);
                  } if (buttons & BUTTON_RIGHT) {
                    type = light_off;
                    printCase(36);
                  } if (buttons & BUTTON_SELECT) {
                    printCase(30);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = light_on_MM[3];
                  light_on_MM[3] = increase_time_MM(light_on_HH[3], light_on_MM[3]);
                  light_on_HH[3] = increase_time_HH(light_on_HH[3], tempMM1);
                  printCase(35);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = light_on_MM[3];
                  light_on_MM[3] = decrease_time_MM(light_on_HH[3], light_on_MM[3]);
                  light_on_HH[3] = decrease_time_HH(light_on_HH[3], tempMM2);
                  printCase(35);
                  level = stationary;
                  break;
              }
              break;
            case (light_off):
            //case 36
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = light_on;
                    printCase(35);
                  } if (buttons & BUTTON_RIGHT) {
                    type = lamp;
                    printCase(37);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(30);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = light_off_MM[3];
                  light_off_MM[3] = increase_time_MM(light_off_HH[3], light_off_MM[3]);
                  light_off_HH[3] = increase_time_HH(light_off_HH[3], tempMM1);
                  printCase(36);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = light_off_MM[3];
                  light_off_MM[3] = decrease_time_MM(light_off_HH[3], light_off_MM[3]);
                  light_off_HH[3] = decrease_time_HH(light_off_HH[3], tempMM2);
                  printCase(36);
                  level = stationary;
                  break;
              }
              break;
            case lamp:
            //case 37
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = light_off;
                    printCase(36);
                  } if (buttons & BUTTON_RIGHT) {
                    type = lamp_on;
                    printCase(38);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(30);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case (increase):
                  if ((bulb3 < 100) && (bulb3 >= 0)) {
                    bulb3 = bulb3 + 5;
                    printCase(37);
                  }
                  level = stationary;
                  break;
                case (decrease):
                  if ((bulb3 <= 100) && (bulb3 > 0)) {
                    bulb3 = bulb3 - 5;
                    printCase(37);
                  }
                  level = stationary;
                  break;
              }
              break;
            case lamp_on:
            //case 38
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = lamp;
                    printCase(37);
                  } if (buttons & BUTTON_RIGHT) {
                    type = lamp_off;
                    printCase(39);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(30);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = lamp_on_MM[3];
                  lamp_on_MM[3] = increase_time_MM(lamp_on_HH[3], lamp_on_MM[3]);
                  lamp_on_HH[3] = increase_time_HH(lamp_on_HH[3], tempMM1);
                  printCase(38);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = lamp_on_MM[3];
                  lamp_on_MM[3] = decrease_time_MM(lamp_on_HH[3], lamp_on_MM[3]);
                  lamp_on_HH[3] = decrease_time_HH(lamp_on_HH[3], tempMM2);
                  printCase(38);
                  level = stationary;
                  break;
              }
              break;
            case lamp_off:
            //case 39
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = lamp_on;
                    printCase(38);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(30);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = lamp_off_MM[3];
                  lamp_off_MM[3] = increase_time_MM(lamp_off_HH[3], lamp_off_MM[3]);
                  lamp_off_HH[3] = increase_time_HH(lamp_off_HH[3], tempMM1);
                  printCase(39);
                  level = stationary; 
                  
                  break;
                case decrease:
                  tempMM2 = lamp_off_MM[3];
                  lamp_off_MM[3] = decrease_time_MM(lamp_off_HH[3], lamp_off_MM[3]);
                  lamp_off_HH[3] = decrease_time_HH(lamp_off_HH[3], tempMM2);
                  printCase(39);
                  level = stationary;
                  break;
              }
              break;
          }
          break;
        case bedroom2:
        //case 40
          switch (type) {
            case (defaultType):
              if (buttons & BUTTON_RIGHT) {
                room2 = bathroom;
                printCase(50);
              } if (buttons & BUTTON_LEFT) {
                room2 = bedroom1;
                printCase(30);
              } if (buttons & BUTTON_DOWN) {
                floors = ground;
                room1 = hall;
                printCase(00);
              } if (buttons & BUTTON_SELECT) {
                type = heat;
                printCase(41);
              }
              delay(150);
              break;
            case (heat):
            //case 41
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_RIGHT) {
                    type = heat_on;
                    printCase(42);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(40);
                    break;
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case (increase):
                  if ((temp4 < 100) && (temp4 >= 0)) {
                    temp4 = temp4 + 5;
                    printCase(41);
                  }
                  level = stationary;
                  break;
                case (decrease):
                  if ((temp4 <= 100) && (temp4 > 0)) {
                    temp4 = temp4 - 5;
                    printCase(41);
                  }
                  level = stationary;
                  break;
              }
              break;
            case (heat_on):
            //case 42
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = heat;
                    printCase(41);
                  } if (buttons & BUTTON_RIGHT) {
                    type = heat_off;
                    printCase(43);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(40);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = heat_on_MM[4];
                  heat_on_MM[4] = increase_time_MM(heat_on_HH[4], heat_on_MM[4]);
                  heat_on_HH[4] = increase_time_HH(heat_on_HH[4], tempMM1);
                  printCase(42);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = heat_on_MM[4];
                  heat_on_MM[4] = decrease_time_MM(heat_on_HH[4], heat_on_MM[4]);
                  heat_on_HH[4] = decrease_time_HH(heat_on_HH[4], tempMM2);
                  printCase(42);
                  level = stationary;
                  break;
              }
              break;
            case (heat_off):
            //case 43
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = heat_on;
                    printCase(42);
                  } if (buttons & BUTTON_RIGHT) {
                    type = light;
                    printCase(44);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(40);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = heat_off_MM[4];
                  heat_off_MM[4] = increase_time_MM(heat_off_HH[4], heat_off_MM[4]);
                  heat_off_HH[4] = increase_time_HH(heat_off_HH[4], tempMM1);
                  printCase(43);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = heat_off_MM[4];
                  heat_off_MM[4] = decrease_time_MM(heat_off_HH[4], heat_off_MM[4]);
                  heat_off_HH[4] = decrease_time_HH(heat_off_HH[4], tempMM2);
                  printCase(43);
                  level = stationary;
                  break;
              }
              break;
            case (light):
            //case 44
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = heat_off;
                    printCase(43);
                  } if (buttons & BUTTON_RIGHT) {
                    type = light_on;
                    printCase(45);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(40);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case (increase):
                  if ((bright4 < 100) && (bright4 >= 0)) {
                    bright4 = bright4 + 5;
                    printCase(44);
                  }
                  level = stationary;
                  break;
                case (decrease):
                  if ((bright4 <= 100) && (bright4 > 0)) {
                    bright4 = bright4 - 5;
                    printCase(44);
                  }
                  level = stationary;
                  break;
              }
              break;
            case (light_on):
            //case 45
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = light;
                    printCase(44);
                  } if (buttons & BUTTON_RIGHT) {
                    type = light_off;
                    printCase(46);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(40);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = light_on_MM[4];
                  light_on_MM[4] = increase_time_MM(light_on_HH[4], light_on_MM[4]);
                  light_on_HH[4] = increase_time_HH(light_on_HH[4], tempMM1);
                  printCase(45);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = light_on_MM[4];
                  light_on_MM[4] = decrease_time_MM(light_on_HH[4], light_on_MM[4]);
                  light_on_HH[4] = decrease_time_HH(light_on_HH[4], tempMM2);
                  printCase(45);
                  level = stationary;
                  break;
              }
              break;
            case (light_off):
            //case 46
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = light_on;
                    printCase(45);
                  } if (buttons & BUTTON_RIGHT) {
                    type = lamp;
                    printCase(47);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(40);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = light_off_MM[4];
                  light_off_MM[4] = increase_time_MM(light_off_HH[4], light_off_MM[4]);
                  light_off_HH[4] = increase_time_HH(light_off_HH[4], tempMM1);
                  printCase(46);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = light_off_MM[4];
                  light_off_MM[4] = decrease_time_MM(light_off_HH[4], light_off_MM[4]);
                  light_off_HH[4] = decrease_time_HH(light_off_HH[4], tempMM2);
                  printCase(46);
                  level = stationary;
                  break;
              }
              break;
            case lamp:
            //case 47
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = light_off;
                    printCase(46);
                  } if (buttons & BUTTON_RIGHT) {
                    type = lamp_on;
                    printCase(48);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(40);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case (increase):
                  if ((bulb4 < 100) && (bulb4 >= 0)) {
                    bulb4 = bulb4 + 5;
                    printCase(47);
                  }
                  level = stationary;
                  break;
                case (decrease):
                  if ((bulb4 <= 100) && (bulb4 > 0)) {
                    bulb4 = bulb4 - 5;
                    printCase(47);
                  }
                  level = stationary;
                  break;
              }
              break;
            case lamp_on:
            //case 48
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = lamp;
                    printCase(47);
                  } if (buttons & BUTTON_RIGHT) {
                    type = lamp_off;
                    printCase(49);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(40);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = lamp_on_MM[4];
                  lamp_on_MM[4] = increase_time_MM(lamp_on_HH[4], lamp_on_MM[4]);
                  lamp_on_HH[4] = increase_time_HH(lamp_on_HH[4], tempMM1);
                  printCase(48);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = lamp_on_MM[4];
                  lamp_on_MM[4] = decrease_time_MM(lamp_on_HH[4], lamp_on_MM[4]);
                  lamp_on_HH[4] = decrease_time_HH(lamp_on_HH[4], tempMM2);
                  printCase(48);
                  level = stationary;
                  break;
              }
              break;
            case lamp_off:
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = lamp_on;
                    printCase(48);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(40);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = lamp_off_MM[4];
                  lamp_off_MM[4] = increase_time_MM(lamp_off_HH[4], lamp_off_MM[4]);
                  lamp_off_HH[4] = increase_time_HH(lamp_off_HH[4], tempMM1);
                  printCase(49);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = lamp_off_MM[4];
                  lamp_off_MM[4] = decrease_time_MM(lamp_off_HH[4], lamp_off_MM[4]);
                  lamp_off_HH[4] = decrease_time_HH(lamp_off_HH[4], tempMM2);
                  printCase(49);
                  level = stationary;
                  break;
              }
              break;
          }
          break;
        case bathroom:
        //case 50
          switch (type) {
            case (defaultType):
              if (buttons & BUTTON_LEFT) {
                room2 = bedroom2;
                printCase(40);
              } if (buttons & BUTTON_DOWN) {
                floors = ground;
                room1 = hall;
                printCase(00);
              } if (buttons & BUTTON_SELECT) {
                type = heat;
                printCase(51);
              }
              delay(150);
              break;
            case (heat):
            //case 51
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_RIGHT) {
                    type = heat_on;
                    printCase(52);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(50);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case (increase):
                  if ((temp5 < 100) && (temp5 >= 0)) {
                    temp5 = temp5 + 5;
                    printCase(51);
                  }
                  level = stationary;
                  break;
                case (decrease):
                  if ((temp5 <= 100) && (temp5 > 0)) {
                    temp5 = temp5 - 5;
                    printCase(51);
                  }
                  level = stationary;
                  break;
              }
              break;
            case (heat_on):
            //case 52
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = heat;
                    printCase(51);
                  } if (buttons & BUTTON_RIGHT) {
                    type = heat_off;
                    printCase(53);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(50);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = heat_on_MM[5];
                  heat_on_MM[5] = increase_time_MM(heat_on_HH[5], heat_on_MM[5]);
                  heat_on_HH[5] = increase_time_HH(heat_on_HH[5], tempMM1);
                  printCase(52);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = heat_on_MM[5];
                  heat_on_MM[5] = decrease_time_MM(heat_on_HH[5], heat_on_MM[5]);
                  heat_on_HH[5] = decrease_time_HH(heat_on_HH[5], tempMM2);
                  printCase(52);
                  level = stationary;
                  break;
              } 
              break;
            case (heat_off):
            //case 53
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = heat_on;
                    printCase(52);
                  } if (buttons & BUTTON_RIGHT) {
                    type = light;
                    printCase(54);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(50);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = heat_off_MM[5];
                  heat_off_MM[5] = increase_time_MM(heat_off_HH[5], heat_off_MM[5]);
                  heat_off_HH[5] = increase_time_HH(heat_off_HH[5], tempMM1);
                  printCase(53);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = heat_off_MM[5];
                  heat_off_MM[5] = decrease_time_MM(heat_off_HH[5], heat_off_MM[5]);
                  heat_off_HH[5] = decrease_time_HH(heat_off_HH[5], tempMM2);
                  printCase(53);
                  level = stationary;
                  break;
              }
              break;
            case (light):
            //case 54
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = heat_off;
                    printCase(53);
                  } if (buttons & BUTTON_RIGHT) {
                    type = light_on;
                    printCase(55);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(50);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;                    
                  }
                  delay(150);
                  break;
                case (increase):
                  if ((bright5 < 100) && (bright5 >= 0)) {
                    bright5 = bright5 + 5;
                    printCase(54);
                  }
                  level = stationary;
                  break;
                case (decrease):
                  if ((bright5 <= 100) && (bright5 > 0)) {
                    bright5 = bright5 - 5;
                    printCase(54);
                  }
                  level = stationary;
                  break;
              }
              break;
            case (light_on):
            //case 55
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = light;
                    printCase(54);
                  } if (buttons & BUTTON_RIGHT) {
                    type = light_off;
                    printCase(56);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(50);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = light_on_MM[5];
                  light_on_MM[5] = increase_time_MM(light_on_HH[5], light_on_MM[5]);
                  light_on_HH[5] = increase_time_HH(light_on_HH[5], tempMM1);
                  printCase(55);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = light_on_MM[4];
                  light_on_MM[5] = decrease_time_MM(light_on_HH[5], light_on_MM[5]);
                  light_on_HH[5] = decrease_time_HH(light_on_HH[5], tempMM2);
                  printCase(55);
                  level = stationary;
                  break;
              }
              break;
            case (light_off):
            //case 56
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = light_on;
                    printCase(55);
                  } if (buttons & BUTTON_RIGHT) {
                    type = lamp;
                    printCase(57);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(50);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = light_off_MM[5];
                  light_off_MM[5] = increase_time_MM(light_off_HH[5], light_off_MM[5]);
                  light_off_HH[5] = increase_time_HH(light_off_HH[5], tempMM1);
                  printCase(56);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = light_off_MM[5];
                  light_off_MM[5] = decrease_time_MM(light_off_HH[5], light_off_MM[5]);
                  light_off_HH[5] = decrease_time_HH(light_off_HH[5], tempMM2);
                  printCase(56);
                  level = stationary;
                  break;
              } 
              break;
            case lamp:
            //case 57
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = light_off;
                    printCase(56);
                  } if (buttons & BUTTON_RIGHT) {
                    type = lamp_on;
                    printCase(58);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(50);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case (increase):
                  if ((bulb5 < 100) && (bulb5 >= 0)) {
                    bulb5 = bulb5 + 5;
                    printCase(57);
                  }
                  level = stationary;
                  break;
                case (decrease):
                  if ((bulb5 <= 100) && (bulb5 > 0)) {
                    bulb5 = bulb5 - 5;
                    printCase(57);
                  }
                  level = stationary;
                  break;
              }
              break;
            case lamp_on:
            //case 58
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = lamp;
                    printCase(57);
                  } if (buttons & BUTTON_RIGHT) {
                    type = lamp_off;
                    printCase(59);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(50);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = lamp_on_MM[5];
                  lamp_on_MM[5] = increase_time_MM(lamp_on_HH[5], lamp_on_MM[5]);
                  lamp_on_HH[5] = increase_time_HH(lamp_on_HH[5], tempMM1);
                  printCase(58);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = lamp_on_MM[5];
                  lamp_on_MM[5] = decrease_time_MM(lamp_on_HH[5], lamp_on_MM[5]);
                  lamp_on_HH[5] = decrease_time_HH(lamp_on_HH[5], tempMM1);
                  printCase(58);
                  level = stationary;
                  break;
              }
              break;
            case lamp_off:
            //case 59
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = lamp_on;
                    printCase(58);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(50);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = lamp_off_MM[5];
                  lamp_off_MM[5] = increase_time_MM(lamp_off_HH[5], lamp_off_MM[5]);
                  lamp_off_HH[5] = increase_time_HH(lamp_off_HH[5], tempMM1);
                  printCase(59);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = lamp_off_MM[5];
                  lamp_off_MM[5] = decrease_time_MM(lamp_off_HH[5], lamp_off_MM[5]);
                  lamp_off_HH[5] = decrease_time_HH(lamp_off_HH[5], tempMM2);
                  printCase(59);
                  level = stationary;
                  break;
              }
              break;
          }
          break;
      }
      break;
    case outside:
      switch (room0){
        case garage:
        //case 60
          switch (type){
            case (defaultType):
              if (buttons & BUTTON_RIGHT) {
                room0 = garden;
                printCase(70);
              } if (buttons & BUTTON_UP) {
                floors = ground;
                room1 = hall;
                printCase(00);
              } if (buttons & BUTTON_SELECT) {
                type = light;
                printCase(61);
              }
              delay(150);
              break;
            case (light):
            //case 61
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_RIGHT) {
                    type = light_on;
                    printCase(62);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(60);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case (increase):
                  if ((bright6 < 100) && (bright6 >= 0)) {
                    bright6 = bright6 + 5;
                    printCase(61);
                  }
                  level = stationary;
                  break;
                case (decrease):
                  if ((bright6 <= 100) && (bright6 > 0)) {
                    bright6 = bright6 - 5;
                    printCase(61);
                  }
                  level = stationary;
                  break;
              }
              break;
            case (light_on):
            //case 62
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = light;
                    printCase(61);
                  } if (buttons & BUTTON_RIGHT) {
                    type = light_off;
                    printCase(63);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(60);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = light_on_MM[6];
                  light_on_MM[6] = increase_time_MM(light_on_HH[6], light_on_MM[6]);
                  light_on_HH[6] = increase_time_HH(light_on_HH[6], tempMM1);
                  printCase(62);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = light_on_MM[6];
                  light_on_MM[6] = decrease_time_MM(light_on_HH[6], light_on_MM[6]);
                  light_on_HH[6] = decrease_time_HH(light_on_HH[6], tempMM2);
                  printCase(62);
                  level = stationary;
                  break;
              }
              break;
            case (light_off):
            //case 63
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = light_on;
                    printCase(62);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(60);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = light_off_MM[6];
                  light_off_MM[6] = increase_time_MM(light_off_HH[6], light_off_MM[6]);
                  light_off_HH[6] = increase_time_HH(light_off_HH[6], tempMM1);
                  printCase(63);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = light_off_MM[6];
                  light_off_MM[6] = decrease_time_MM(light_off_HH[6], light_off_MM[6]);
                  light_off_HH[6] = decrease_time_HH(light_off_HH[6], tempMM2);
                  printCase(63);
                  level = stationary;
                  break;
              }
              break;
          }
          break;
        case garden:
        //case 70
          switch (type) {
            case (defaultType):
              if (buttons & BUTTON_LEFT) {
                room0 = garage;
                printCase(60);
              } if (buttons & BUTTON_UP) {
                floors = ground;
                room1 = hall;
                printCase(00);
              } if (buttons & BUTTON_SELECT) {
                type = water;
                printCase(71);
              }
              delay(150);
              break;
            case (water):
            //case 71
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_RIGHT) {
                    type = water_on;
                    printCase(72);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(70);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case (increase):
                  if ((power7 < 100) && (power7 >= 0)) {
                    power7 = power7 + 5;
                    printCase(71);
                  }
                  level = stationary;
                  break;
                case (decrease):
                  if ((power7 <= 100) && (power7 > 0)) {
                    power7 = power7 - 5;
                    printCase(71);
                  }
                  level = stationary;
                  break;
              }
              break;
            case (water_on):
            //case 72
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = heat;
                    printCase(71);
                  } if (buttons & BUTTON_RIGHT) {
                    type = water_off;
                    printCase(73);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(70);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = water_on_MM;
                  water_on_MM = increase_time_MM(water_on_HH, water_on_MM);
                  water_on_HH = increase_time_HH(water_on_HH, tempMM1);
                  printCase(72);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = water_on_MM;
                  water_on_MM = decrease_time_MM(water_on_HH, water_on_MM);
                  water_on_HH = decrease_time_HH(water_on_HH, tempMM2);
                  printCase(72);
                  level = stationary;
                  break;
              } 
              break;
            case (water_off):
            //case 73
              switch (level) {
                case (stationary):
                  if (buttons & BUTTON_LEFT) {
                    type = water_on;
                    printCase(72);
                  } if (buttons & BUTTON_SELECT) {
                    type = defaultType;
                    printCase(70);
                  } if (buttons & BUTTON_UP) {
                    level = increase;
                  } if (buttons & BUTTON_DOWN) {
                    level = decrease;
                  }
                  delay(150);
                  break;
                case increase:
                  tempMM1 = water_off_MM;
                  water_off_MM = increase_time_MM(water_off_HH, water_off_MM);
                  water_off_HH = increase_time_HH(water_off_HH, tempMM1);
                  printCase(73);
                  level = stationary; 
                  break;
                case decrease:
                  tempMM2 = water_on_MM;
                  water_off_MM = decrease_time_MM(water_off_HH, water_off_MM);
                  water_off_HH = decrease_time_HH(water_off_HH, tempMM2);
                  printCase(73);
                  level = stationary;
                  break;
              } 
              break;
          }
          break;
      }
      break;
  }
}

//Check if a time value is below 10
void ifzero_MM(int number) {
  if (number < 10) {
    Serial.print(0); Serial.println(number);
  } else {
    Serial.println(number);
  }
}
void ifzero_HH(int number) {
  if (number < 10) {
    Serial.print(0); Serial.print(number);
  } else {
    Serial.print(number);
  }
}

//Change Edit Time Functions
int increase_time_MM(int HH, int MM) {
  if (HH==23 && MM==45){
    MM = 0;
    HH = 0;
    delay(150);
  } else if (MM == 45){
    HH = HH + 1;
    MM = 0;
    delay(150);
  } else if(MM<45){
    MM = MM + 15;
    delay(150);
  }
  return MM;
} int increase_time_HH(int HH, int MM) {
  if (HH==23 && MM==45){
    MM = 0;
    HH = 0;
    delay(150);
  } else if (MM == 45){
    HH = HH + 1;
    MM = 0;
    delay(150);
  } else if(MM<45){
    MM = MM + 15;
    delay(150);
  }
  return HH;
} int decrease_time_MM(int HH, int MM) {
  if (HH==00 && MM==00){
    HH = 23;
    MM = 45;
    delay(150);
  } else if (MM == 00){
    HH = HH - 1; 
    MM = 45;
    delay(150);
  } else if(MM<=45){
    MM = MM - 15;
    delay(150);
  } 
  return MM;
} int decrease_time_HH(int HH, int MM) {
  if (HH==00 && MM==00){
    HH = 23;
    MM = 45;
    delay(150);
  } else if (MM == 00){
    HH = HH - 1; 
    MM = 45;
    delay(150);
  } else if(MM<=45){
    MM = MM - 15;
    delay(150);
  } 
  return HH;
}
void resetDevice(){
  changeLevel_t level = stationary;
  typeAction_t type = defaultType;
  room1_t room1 = hall; 
  floor_t floors = ground;  
}
void printCase(int number){
  lcd.clear();
  if (number == 00){
    Serial.println("Ground/Hall");
    lcd.print("Ground/Hall");
  } if (number == 1){
    Serial.print(F("Ground/Hall/Heat/Main/Level:")); Serial.println(temp0);
  } if (number == 2){
    Serial.print(F("Ground/Hall/Heat/Main/On:")); ifzero_HH(heat_on_HH[0]); Serial.print('.'); ifzero_MM(heat_on_MM[0]);    
  } if (number == 3){
    Serial.print(F("Ground/Hall/Heat/Main/Off:")); ifzero_HH(heat_off_HH[0]); Serial.print('.'); ifzero_MM(heat_off_MM[0]);
  } if (number == 4){
    Serial.print(F("Ground/Hall/Light/Main/Level:")); Serial.println(bright0);
  } if (number == 5){
    Serial.print(F("Ground/Hall/Light/Main/On:")); ifzero_HH(light_on_HH[0]); Serial.print('.'); ifzero_MM(light_on_MM[0]);
  } if (number == 6){
    Serial.print(F("Ground/Hall/Light/Main/Off:")); ifzero_HH(light_off_HH[0]); Serial.print('.'); ifzero_MM(light_off_MM[0]);
  } if (number == 7){
    Serial.print(F("Ground/Hall/Lamp/Wall/Level:")); Serial.println(bulb0);
  } if (number == 8){
    Serial.print(F("Ground/Hall/Lamp/Wall/On:")); ifzero_HH(lamp_on_HH[0]); Serial.print('.'); ifzero_MM(lamp_on_MM[0]);
  } if (number == 9){
    Serial.print(F("Ground/Hall/Lamp/Wall/Off:")); ifzero_HH(lamp_off_HH[0]); Serial.print('.'); ifzero_MM(lamp_off_MM[0]);
  }
  if (number == 10){
    Serial.println("Ground/Living Room");
    lcd.print("Ground/Living Rm");
  } if (number == 11){
    Serial.print(F("Ground/Living Room/Heat/Main/Level:")); Serial.println(temp1);
  } if (number == 12){
    Serial.print(F("Ground/Living Room/Heat/Main/On:")); ifzero_HH(heat_on_HH[1]); Serial.print('.'); ifzero_MM(heat_on_MM[1]);
  } if (number == 13){
    Serial.print(F("Ground/Living Room/Heat/Main/Off:")); ifzero_HH(heat_off_HH[1]); Serial.print('.'); ifzero_MM(heat_off_MM[1]);
  } if (number == 14){
    Serial.print(F("Ground/Living Room/Light/Main/Level:")); Serial.println(bright1);
  } if (number == 15){
    Serial.print(F("Ground/Living Room/Light/Main/On:")); ifzero_HH(light_on_HH[1]); Serial.print('.'); ifzero_MM(light_on_MM[1]);
  } if (number == 16){
    Serial.print(F("Ground/Living Room/Light/Main/Off:")); ifzero_HH(light_off_HH[1]); Serial.print('.'); ifzero_MM(light_off_MM[1]);
  } if (number == 17){
    Serial.print(F("Ground/Living Room/Lamp/Wall/Level:")); Serial.println(bulb1);
  } if (number == 18){
    Serial.print(F("Ground/Living Room/Lamp/Wall/On:")); ifzero_HH(lamp_on_HH[1]); Serial.print('.'); ifzero_MM(lamp_on_MM[1]);
  } if (number == 19){
    Serial.print(F("Ground/Living Room/Lamp/Wall/Off:")); ifzero_HH(lamp_off_HH[1]); Serial.print('.'); ifzero_MM(lamp_off_MM[1]);
  }
  if (number == 20){
    Serial.println("Ground/Kitchen");
    lcd.print("Ground/Kitchen");
  } if (number == 21){
    Serial.print(F("Ground/Kitchen/Heat/Main/Level:")); Serial.println(temp2);
  } if (number == 22){
    Serial.print(F("Ground/Kitchen/Heat/Main/On:")); ifzero_HH(heat_on_HH[2]); Serial.print('.'); ifzero_MM(heat_on_MM[2]);
  } if (number == 23){
    Serial.print(F("Ground/Kitchen/Heat/Main/Off:")); ifzero_HH(heat_off_HH[2]); Serial.print('.'); ifzero_MM(heat_off_MM[2]);
  } if (number == 24){
    Serial.print(F("Ground/Kitchen/Light/Main/Level:")); Serial.println(bright2);
  } if (number == 25){
    Serial.print(F("Ground/Kitchen/Light/Main/On:")); ifzero_HH(light_on_HH[2]); Serial.print('.'); ifzero_MM(light_on_MM[2]);
  } if (number == 26){
    Serial.print(F("Ground/Kitchen/Light/Main/Off:")); ifzero_HH(light_off_HH[2]); Serial.print('.'); ifzero_MM(light_off_MM[2]);
  } if (number == 27){
    Serial.print(F("Ground/Kitchen/Lamp/Desk/Level:")); Serial.println(bulb2);
  } if (number == 28){
    Serial.print(F("Ground/Kitchen/Lamp/Desk/On:")); ifzero_HH(lamp_on_HH[2]); Serial.print('.'); ifzero_MM(lamp_on_MM[2]);
  } if (number == 29){
    Serial.print(F("Ground/Kitchen/Lamp/Desk/Off:")); ifzero_HH(lamp_off_HH[2]); Serial.print('.'); ifzero_MM(lamp_off_MM[2]);
  }
  if (number == 30){
    Serial.println("First/Bedroom 1");
    lcd.print("First/Bedroom 1");
  } if (number == 31){
    Serial.print(F("First/Bedroom 1/Heat/Main/Level:")); Serial.println(temp3);
  } if (number == 32){
    Serial.print(F("First/Bedroom 1/Heat/Main/On:")); ifzero_HH(heat_on_HH[3]); Serial.print('.'); ifzero_MM(heat_on_MM[3]);
  } if (number == 33){
    Serial.print(F("First/Bedroom 1/Heat/Main/Off:")); ifzero_HH(heat_off_HH[3]); Serial.print('.'); ifzero_MM(heat_off_MM[3]);
  } if (number == 34){
    Serial.print(F("First/Bedroom 1/Light/Main/Level:")); Serial.println(bright3);
  } if (number == 35){
    Serial.print(F("First/Bedroom 1/Light/Main/On:")); ifzero_HH(light_on_HH[3]); Serial.print('.'); ifzero_MM(light_on_MM[3]);
  } if (number == 36){
    Serial.print(F("First/Bedroom 1/Light/Main/Off:")); ifzero_HH(light_off_HH[3]); Serial.print('.'); ifzero_MM(light_off_MM[3]);
  } if (number == 37){
    Serial.print(F("First/Bedroom 1/Lamp/Desk/Level:")); Serial.println(bulb3);
  } if (number == 38){
    Serial.print(F("First/Bedroom 1/Lamp/Desk/On:")); ifzero_HH(lamp_on_HH[3]); Serial.print('.'); ifzero_MM(lamp_on_MM[3]);
  } if (number == 39){
    Serial.print(F("First/Bedroom 1/Lamp/Desk/Off:")); ifzero_HH(lamp_off_HH[3]); Serial.print('.'); ifzero_MM(lamp_off_MM[3]);
  }
  if (number == 40){
    Serial.println("First/Bedroom 2");
    lcd.print("First/Bedroom 2");
  } if (number == 41){
    Serial.print(F("First/Bedroom 2/Heat/Main/Level:")); Serial.println(temp4);
  } if (number == 42){
    Serial.print(F("First/Bedroom 2/Heat/Main/On:")); ifzero_HH(heat_on_HH[4]); Serial.print('.'); ifzero_MM(heat_on_MM[4]);
  } if (number == 43){
    Serial.print(F("First/Bedroom 2/Heat/Main/Off:")); ifzero_HH(heat_off_HH[4]); Serial.print('.'); ifzero_MM(heat_off_MM[4]);
  } if (number == 44){
    Serial.print(F("First/Bedroom 2/Light/Main/Level:")); Serial.println(bright4);
  } if (number == 45){
    Serial.print(F("First/Bedroom 2/Light/Main/On:")); ifzero_HH(light_on_HH[4]); Serial.print('.'); ifzero_MM(light_on_MM[4]);
  } if (number == 46){
    Serial.print(F("First/Bedroom 2/Light/Main/Off:")); ifzero_HH(light_off_HH[4]); Serial.print('.'); ifzero_MM(light_off_MM[4]);
  } if (number == 47){
    Serial.print(F("First/Bedroom 2/Lamp/Desk/Level:")); Serial.println(bulb4);
  } if (number == 48){
    Serial.print(F("First/Bedroom 2/Lamp/Desk/On:")); ifzero_HH(lamp_on_HH[4]); Serial.print('.'); ifzero_MM(lamp_on_MM[4]);
  } if (number == 49){
    Serial.print(F("First/Bedroom 2/Lamp/Desk/Off:")); ifzero_HH(lamp_off_HH[4]); Serial.print('.'); ifzero_MM(lamp_off_MM[4]);
  }
  if (number == 50){
    Serial.println("First/Bathroom");
    lcd.print("First/Bathroom");
  } if (number == 51){
    Serial.print(F("First/Bathroom/Heat/Main/Level:")); Serial.println(temp5);
  } if (number == 52){
    Serial.print(F("First/Bathroom/Heat/Main/On:")); ifzero_HH(heat_on_HH[5]); Serial.print('.'); ifzero_MM(heat_on_MM[5]);
  } if (number == 53){
    Serial.print(F("First/Bathroom/Heat/Main/Off:")); ifzero_HH(heat_off_HH[5]); Serial.print('.'); ifzero_MM(heat_off_MM[5]);
  } if (number == 54){
    Serial.print(F("First/Bathroom/Light/Main/Level:")); Serial.println(bright5);
  } if (number == 55){
    Serial.print(F("First/Bathroom/Light/Main/On:")); ifzero_HH(light_on_HH[5]); Serial.print('.'); ifzero_MM(light_on_MM[5]);
  } if (number == 56){
    Serial.print(F("First/Bathroom/Light/Main/Off:")); ifzero_HH(light_off_HH[5]); Serial.print('.'); ifzero_MM(light_off_MM[5]);
  }  if (number == 57){
    Serial.print(F("First/Bathroom/Lamp/Ceiling/Level:")); Serial.println(bulb5);
  } if (number == 58){
    Serial.print(F("First/Bathroom/Lamp/Ceiling/On:")); ifzero_HH(lamp_on_HH[5]); Serial.print('.'); ifzero_MM(lamp_on_MM[5]);
  } if (number == 59){
    Serial.print(F("First/Bathroom/Lamp/Ceiling/Off:")); ifzero_HH(lamp_off_HH[5]); Serial.print('.'); ifzero_MM(lamp_off_MM[5]);
  }
  if (number == 60){
    Serial.println("Outside/Garage");
    lcd.print("Outside/Garage");
  } if (number == 61){
    Serial.print(F("Outside/Garage/Light/Main/Level:")); Serial.println(bright6);
  } if (number == 62){
    Serial.print(F("Outside/Garage/Light/Main/On:")); ifzero_HH(light_on_HH[6]); Serial.print('.'); ifzero_MM(light_on_MM[6]);
  } if (number == 63){
    Serial.print(F("Outside/Garage/Light/Main/Off:")); ifzero_HH(light_off_HH[6]); Serial.print('.'); ifzero_MM(light_off_MM[6]);
  } 
  if (number == 70){
    Serial.println("Outside/Garden");
    lcd.print("Outside/Garden");
  } if (number == 71){
    Serial.print(F("Outside/Garden/Water/Wall/Level:")); Serial.println(power7);
  } if (number == 72){
    Serial.print(F("Outside/Garden/Water/Wall/On:")); ifzero_HH(water_on_HH); Serial.print('.'); ifzero_MM(water_on_MM);
  } if (number == 73){
    Serial.print(F("Outside/Garden/Water/Wall/Off:")); ifzero_HH(water_off_HH); Serial.print('.'); ifzero_MM(water_off_MM);
  }
  if (number == 888){
    //All Strings - infinity
    for (int i = 0; i<=73; i++){
      printCase(i);
    }
    lcd.clear();
    lcd.print("Continue");
    Serial.print('\n');
  }
}

//freeMemory Function to check SRAM
#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else // __ARM__
extern char *__brkval;
#endif // __arm__
int freeMemory() {
  char top;
  #ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
  #elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
  #else // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
  #endif // __arm__
}
