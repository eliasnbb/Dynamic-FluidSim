#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;

const int OLED_RESET = -1;
const int maksAntall = 800; // 190 er maks på Arduino Uno R3 per 05.04.2024.

const int SCREEN_ADDRESS = 0x3c;
Adafruit_SH1106G skjerm = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_MPU6050 mpu;

const int referanseForhold = 50;
const int skjermHZ = 40;

static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000 };

int xPosArray[maksAntall];
int yPosArray[maksAntall];
int xVelArray[maksAntall];
int yVelArray[maksAntall];

unsigned long deltaTid, forrigeDeltaTid, intervall = 200;

void setup() {

  skjerm.begin(SCREEN_ADDRESS, true);
  Serial.begin(115200);

  mpu.begin();
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);

  skjerm.display();
  delay(2000);
  skjerm.clearDisplay();

  for (int i = 0; i < maksAntall; i++){
  int xRand = random(0,((SCREEN_WIDTH-1)*referanseForhold));
  int yRand = random(0, ((SCREEN_HEIGHT-1)*referanseForhold));
  xPosArray[i] = xRand;
  yPosArray[i] = yRand;
  }
}

const int gravitet = -400;                          // maks fart (y)
const int aks = 20;                                 // akselerasjon
const int deAks = 4;
float mpuXYZ[2];                              // X-aksen er 0 når den ligger flatt.

void loop() {


  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  mpuXYZ[0] = (-(a.gyro.y));
  mpuXYZ[1] = a.gyro.x;

  for (int i = 0; i < 2; i++){
    if (mpuXYZ[i] > deAks){
      mpuXYZ[i] = deAks;
    } else if (mpuXYZ[i] < -deAks){
      mpuXYZ[i] = -deAks;
    }
  }
  
  deltaTid = millis();
    for (int i = 0; i < maksAntall; i++){
      yVelArray[i] += (aks * mpuXYZ[1]);
      xVelArray[i] += (aks * mpuXYZ[0]);
      if (yPosArray[i] > ((SCREEN_HEIGHT-1)*referanseForhold)){    // Sørger for at partikler holder seg innen skjermen.
        yVelArray[i] = -30;
        yPosArray[i] = ((SCREEN_HEIGHT-1)*referanseForhold);
      }
      if (yPosArray[i] <= 0){
        yVelArray[i] = 30;
        yPosArray[i] = 1;
      }
      if (xPosArray[i] > ((SCREEN_WIDTH-2)*referanseForhold)){    // Sørger for at partikler holder seg innen skjermen.
        xVelArray[i] = -30;
        xPosArray[i] = ((SCREEN_WIDTH-2)*referanseForhold);
      }
      if (xPosArray[i] <= 10){
        xVelArray[i] = 30;
        xPosArray[i] = 10;
      }
      if (xVelArray[i] > -gravitet){            // Sørger for at ingen partikler beveger seg for fort
        xVelArray[i] = -gravitet;
        }   
      if (xVelArray[i] < gravitet){           // ~~~||~~~
        xVelArray[i] = gravitet;
      }
      if (yVelArray[i] > -gravitet){            // Sørger for at ingen partikler beveger seg for fort
        yVelArray[i] = -gravitet;
        }   
      if (yVelArray[i] < gravitet){           // ~~~||~~~
        yVelArray[i] = gravitet;
      }
      if (xVelArray[i] > 0){                  // Horisontal fart minker gradvis.
        xVelArray[i] -= 2;
      } else if (xVelArray[i] < 0){
        xVelArray[i] += 2;
      }
      if (skjerm.getPixel((xPosArray[i] + xVelArray[i]) / referanseForhold, (yPosArray[i] + yVelArray[i]) / referanseForhold) == true){
        yPosArray[i] -= referanseForhold;
        yVelArray[i] = 0;
        xVelArray[i] += random(-30, 30);
      }

      if(deltaTid - forrigeDeltaTid >= intervall){
        Serial.print("\n\n\n\n\n");
        Serial.print("X: ");
        Serial.println(mpuXYZ[0]);
        Serial.print("Y: ");
        Serial.println(mpuXYZ[1]);
        forrigeDeltaTid = deltaTid;
      }

      xPosArray[i] += xVelArray[i];
      yPosArray[i] += yVelArray[i];
      
    skjerm.drawPixel((xPosArray[i]/referanseForhold), (yPosArray[i]/referanseForhold), SH110X_WHITE);
 
    }

  skjerm.display();
  delay(1000/skjermHZ);
  skjerm.clearDisplay();
}

