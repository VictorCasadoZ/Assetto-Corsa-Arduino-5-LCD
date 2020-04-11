#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <RA8875.h>
#include "fonts/akashi_36.c"//minipixel
#include "fonts/aerial_22.c"//minipixel

#define PIN        5
#define NUMPIXELS 16
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define RA8875_CS 10
#define RA8875_RESET 9//any pin or 255 to disable it!

#define AZUL1 0x0258
#define ROJO1 0xF800
#define VERDE1 0x47E0
#define NEGRO 0X0000

#define runEvery(t) for (static uint16_t _lasttime;\
                         (uint16_t)((uint16_t)millis() - _lasttime) >= (t);\
                         _lasttime += (t))
volatile unsigned int cuenta = 0;
bool ESTADO = false;

String maxRPMs = "9000";

String command;

String Gear = "1";
String Speed = "0";
String RPMs = "2000";
String Fuel = "0";
String Boost = "0";
String AirTemp = "0";
String Pit = "0";
String CurrentTime = "0";
String LastLapTime = "0";
String BestLapTime = "0";
String Position = "0";
String CompletedLaps = "0";
String NumberOfLaps = "0";

String last_Gear = "1";
String last_Speed = "0";
String last_RPMs = "0";
String last_Fuel = "0";
String last_Boost = "0";
String last_AirTemp = "0";
String last_Pit = "0";
String last_CurrentTime = "0";
String last_LastLapTime = "0";
String last_LastBestTime = "0";
String last_Position = "0";
String last_CompletedLaps = "0";
String last_NumberOfLaps = "0";

char current_screen = 0; // 0: USB connection pending; 1: Main page data display
char main_screen_state = 0;

bool led_blink = false;

RA8875 tft = RA8875(RA8875_CS, RA8875_RESET);

void setup()
{
  Serial.begin(9600);

  // Init Timer a 32.64ms
  SREG = (SREG & 0b01111111); //Desabilitar interrupciones
  TIMSK2 = TIMSK2 | 0b00000001; //Habilita la interrupcion por desbordamiento
  TCCR2B = 0b00000111; //Configura preescala para que FT2 sea de 7812.5Hz
  SREG = (SREG & 0b01111111) | 0b10000000; //Habilitar interrupciones //Desabilitar interrupciones
  pinMode(13, OUTPUT);

  tft.begin(RA8875_800x480);
  pixels.begin();
  pixels.clear();
  pixels.setBrightness(50);
  tft.setRotation(0);//works at any rotation as well
  //about();
  //main_screen();
  tft.setFont(&aerial_22);
  tft.setFontScale(3);
  tft.setCursor(CENTER, 0);
  tft.setTextColor(RA8875_GREEN);

  start_screen();
  current_screen = 0;
  while (!Serial.available()) {
    tft.fillWindow(NEGRO);
    tft.setFont(&aerial_22);
    tft.setFontScale(1);
    for (int i = 0; i < 10; i++) {
      tft.setCursor(100, 220);
      tft.setTextColor(tft.Color565(i * 25, 0, 0)); //Force white
      tft.println("CONNECT THE APP");
    }
    for (int i = 0; i < 10; i++) {
      tft.setCursor(100, 220);
      tft.setTextColor(tft.Color565(255 - (i * 25), 0, 0)); //Force white
      tft.println("CONNECT THE APP");
    }
    tft.setCursor(100, 220);
    tft.setTextColor(tft.Color565(0, 0, 0)); //Force white
    tft.println("CONNECT THE APP");
  }
  main_screen();
  current_screen = 1;
  while (1) {
    while (Serial.available()) {
      char c = Serial.read();
      if ((c == '\n')) {
        if (command.length() == 47) {
          Gear = command.substring(0, 1);
          Speed = command.substring(1, 4);
          RPMs = command.substring(4, 9);
          Fuel = command.substring(9, 11);
          AirTemp = command.substring(11, 13);
          maxRPMs = command.substring(13, 18);
          Boost = command.substring(18, 22);
          LastLapTime = command.substring(22, 31);
          BestLapTime = command.substring(31, 40);
          Position = command.substring(40, 42);
          CompletedLaps = command.substring(42, 44);
          NumberOfLaps = command.substring(44, 46);
          Pit = command.substring(46, 47);
        }
        command = "";
      } else {
        command += c;
      }
    }
    /*if (Pit != last_Pit) {
      if (Pit == "1") {
        tft.setFontScale(2);
        tft.setCursor(650, 20);
        tft.setTextColor(tft.Color565(255, 255, 0)); //Force white
        tft.println("PIT");
      }
      if (Pit == "0") {
        tft.setFontScale(2);
        tft.setCursor(650, 20);
        tft.setTextColor(tft.Color565(0, 0, 0)); //Force white
        tft.println("PIT");
      }
      }*/

    setLeds(RPMs.toInt() / ((maxRPMs.toInt() - 150) / 16));
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void start_screen() {
  tft.fillWindow(NEGRO);
  tft.setFont(&aerial_22);
  tft.setFontScale(1);

  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    if (j % 10 == 0) {
      tft.setCursor(150, 220);
      tft.setTextColor(tft.Color565(j, j, j)); //Force white
      tft.println("VICTOR CASADO");
    }
    for (i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, Wheel((i + j) & 255));
    }
    pixels.show();
    //delay(wait);
  }


  /*
    for (int i = 0; i < 20; i++) {
    tft.setCursor(150, 220);
    tft.setTextColor(tft.Color565(i * 12, i * 12, i * 12)); //Force white
    tft.println("VICTOR CASADO");
    }
    for (int i = 0; i < 20; i++) {
    tft.setCursor(150, 220);
    tft.setTextColor(tft.Color565(255 - (i * 12), 255 - (i * 12), 255 - (i * 12))); //Force white
    tft.println("VICTOR CASADO");
    }*/
  tft.fillWindow(NEGRO);
}

void main_screen() {
  degradado();
  lineasVdegradadas(125, 0, 3 * tft.height() / 4);
  lineasVdegradadas(335, 0, 3 * tft.height() / 4);
  lineasVdegradadas(635, 0, 105);
  lineasVdegradadas(575, 105, 3 * tft.height() / 4);

  lineasHdegradadas(125, 105, 800);

  tft.setFont(&aerial_22);
  tft.setTextGrandient(0xFFFF, 0x73AE);
  tft.setFontScale(0);
  tft.setCursor(150, 75);
  tft.println("SPEED");
  tft.setTextGrandient(0xFFFF, 0x73AE);
  tft.setCursor(600, 240);
  tft.println("BOOST");
  tft.setTextGrandient(0xFFFF, 0x73AE);
  tft.setCursor(150, 145);
  tft.println("LAST LAP");
  tft.setTextGrandient(0xFFFF, 0x73AE);
  tft.setCursor(150, 200);
  tft.println("BEST LAP");
  tft.setTextGrandient(0xFFFF, 0x73AE);
  tft.setCursor(600, 320);
  tft.println("AIR TEMP");
  tft.setCursor(360, 320);
  tft.setTextGrandient(0xFFFF, 0x73AE);
  tft.println("GEAR");
  tft.setCursor(600, 160);
  tft.setTextGrandient(0xFFFF, 0x73AE);
  tft.println("FUEL");
  tft.setCursor(650, 75);
  tft.setTextGrandient(0xFFFF, 0x73AE);
  tft.println("POSITION");
  tft.setCursor(360, 75);
  tft.setTextGrandient(0xFFFF, 0x73AE);
  tft.println("LAP");
  tft.setCursor(500, 5);
  tft.setTextGrandient(0xFFFF, 0x73AE);
  tft.println("OF");

  tft.setTextColor(0xB800);
  tft.setFontScale(2);
  tft.setCursor(50, 390);
  tft.println("PERFORMANCE");
  tft.setFont(INT);
  tft.setTextColor(NEGRO);
  tft.setFontSize((RA8875tsize)1);
  tft.setCursor(280, 460);
  tft.println("Developed by Victor Casado @ 2020");
}

void about() {
  tft.fillWindow(NEGRO);
  degradado();
  tft.setCursor(280, 450);
  tft.setTextColor(NEGRO);
  tft.setFontSize((RA8875tsize)1);
  tft.println("Developed by Victor Casado @ 2020");
}

void setLeds(int n) {
  if (n < 15) {
    for (int i = 0; i < n + 1; i++) {
      for (int j = 0; j < i; j++) {
        pixels.setPixelColor(i - 1, (j * 16) , (255 - (j * 16)), 0);
      }
    }
    for (int i = n ; i < NUMPIXELS ; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
  } else if (n >= 15) {
    runEvery(100) {
      if (led_blink) {
        for (int i = 0; i < n + 1; i++) {
          for (int j = 0; j < i; j++) {
            pixels.setPixelColor(i - 1, (j * 16) , (255 - (j * 16)), 0);
          }
        }
        for (int i = n ; i < NUMPIXELS ; i++) {
          pixels.setPixelColor(i, pixels.Color(0, 0, 0));
        }
      } else {
        for (int i = 0 ; i < NUMPIXELS ; i++) {
          pixels.setPixelColor(i, pixels.Color(0, 0, 0));
        }
      }
      led_blink = !led_blink;
    }
  }
  pixels.show();
}

inline void displaySpeed(String value, int x, int y, uint16_t background, uint16_t color, int textsize) {
  tft.setCursor(x, y);
  tft.setFont(&aerial_22);
  tft.setTextColor(background);
  tft.setFontScale(textsize);
  tft.println(last_Speed);
  //delay(5);
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setFontScale(textsize);
  tft.println(value);
  last_Speed = value;
}

inline void displayPosition(String value, int x, int y, uint16_t background, uint16_t color, int textsize) {
  tft.setCursor(x, y);
  tft.setFont(&aerial_22);
  tft.setTextColor(background);
  tft.setFontScale(textsize);
  tft.println(last_Position);
  //delay(5);
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setFontScale(textsize);
  tft.println(value);
  last_Position = value;
}

inline void displayPit(String value, int x, int y, uint16_t background, uint16_t color, int textsize) {
  if (value == "1") {
    tft.setCursor(x, y);
    tft.setFont(&aerial_22);
    tft.setTextColor(0xEFE0);
    tft.setFontScale(textsize);
    tft.println("PIT");
    last_Pit = value;
  } else if(value == "0"){
    tft.fillRect(x-10, y-10, x+100, y+100, 0x0000);
    tft.setCursor(x, y);
    tft.setFont(&aerial_22);
    tft.setTextColor(0x10A2);
    tft.setFontScale(textsize);
    tft.println("PIT");
    last_Pit = value;
  }
}

inline void displayNumberOfLaps(String value, int x, int y, uint16_t background, uint16_t color, int textsize) {
  tft.setCursor(x, y);
  tft.setFont(&aerial_22);
  tft.setTextColor(background);
  tft.setFontScale(textsize);
  tft.println(last_NumberOfLaps);
  //delay(5);
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setFontScale(textsize);
  tft.println(value);
  last_NumberOfLaps = value;
}

inline void displayCompletedLaps(String value, int x, int y, uint16_t background, uint16_t color, int textsize) {
  tft.setCursor(x, y);
  tft.setFont(&aerial_22);
  tft.setTextColor(background);
  tft.setFontScale(textsize);
  tft.println(last_CompletedLaps);
  //delay(5);
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setFontScale(textsize);
  tft.println(value);
  last_CompletedLaps = value;
}

inline void displayCurrentTime(String value, int x, int y, uint16_t background, uint16_t color, int textsize) {
  tft.setCursor(x, y);
  tft.setFont(&aerial_22);
  tft.setTextColor(background);
  tft.setFontScale(textsize);
  tft.println(last_CurrentTime);
  //delay(5);
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setFontScale(textsize);
  tft.println(value);
  last_CurrentTime = value;
}

inline void displayLastLapTime(String value, int x, int y, uint16_t background, uint16_t color, int textsize) {
  tft.setCursor(x, y);
  tft.setFont(&aerial_22);
  tft.setTextColor(background);
  tft.setFontScale(textsize);
  tft.println(last_LastLapTime);
  //delay(5);
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setFontScale(textsize);
  tft.println(value);
  last_LastLapTime = value;
}

inline void displayBestLapTime(String value, int x, int y, uint16_t background, uint16_t color, int textsize) {
  tft.setCursor(x, y);
  tft.setFont(&aerial_22);
  tft.setTextColor(background);
  tft.setFontScale(textsize);
  tft.println(last_LastBestTime);
  //delay(5);
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setFontScale(textsize);
  tft.println(value);
  last_LastBestTime = value;
}

inline void displayBoost(String value, int x, int y, uint16_t background, uint16_t color, int textsize) {
  tft.setCursor(x, y);
  tft.setFont(&aerial_22);
  tft.setTextColor(background);
  tft.setFontScale(textsize);
  tft.println(last_Boost);
  //delay(5);
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setFontScale(textsize);
  tft.println(value);
  last_Boost = value;
}

inline void displayFuel(String value, int x, int y, uint16_t background, uint16_t color, int textsize) {
  tft.setCursor(x, y);
  tft.setFont(&aerial_22);
  tft.setTextColor(background);
  tft.setFontScale(textsize);
  tft.println(last_Fuel);
  //delay(5);
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setFontScale(textsize);
  tft.println(value);
  last_Fuel = value;
}

inline void displayAirTemp(String value, int x, int y, uint16_t background, uint16_t color, int textsize) {
  tft.setCursor(x, y);
  tft.setFont(&aerial_22);
  tft.setTextColor(background);
  tft.setFontScale(textsize);
  tft.println(last_AirTemp);
  //delay(5);
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setFontScale(textsize);
  tft.println(value);
  last_AirTemp = value;
}

inline void displayRPMs(String value, int x, int y, uint16_t background, uint16_t color, int textsize) {
  tft.setCursor(x, y);
  tft.setFont(&aerial_22);
  tft.setTextColor(background);
  tft.setFontScale(textsize);
  tft.println(last_RPMs);
  //delay(5);
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setFontScale(textsize);
  tft.println(value);
  last_RPMs = value;
}

inline void displayGear(String value, int x, int y, uint16_t background, uint16_t color, int textsize) {
  tft.setCursor(x, y);
  tft.setFont(&aerial_22);
  tft.setTextColor(background);
  tft.setFontScale(textsize);
  tft.println(last_Gear);
  //delay(5);
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setFontScale(textsize);
  tft.println(value);
  last_Gear = value;
}

void lineasVdegradadas(int x, int y, int altura) {
  for (int i = x; i < x + 3; i++) {
    tft.drawFastVLine(i - 3, y, altura - y, tft.Color565((i - x) * 67, (i - x) * 67, (i - x) * 67));
  }
  yield();
  for (int i = x; i < x + 3; i++) {
    tft.drawFastVLine(i, y, altura - y, tft.Color565(201 - (i - x) * 67, 201 - (i - x) * 67, 201 - (i - x) * 67));
  }
  yield();
}

void lineasHdegradadas(int x, int y, int largo) {
  for (int i = y; i < y + 3; i++) {
    tft.drawFastHLine(x, i - 3, largo - x, tft.Color565((i - y) * 67, (i - y) * 67, (i - y) * 67));
  }
  yield();
  for (int i = y; i < y + 3; i++) {
    tft.drawFastHLine(x, i, largo - x, tft.Color565(201 - (i - y) * 67, 201 - (i - y) * 67, 201 - (i - y) * 67));
  }
  yield();
}

void degradado() {
  for (int i = 0; i < tft.height() / 4; i++) {
    tft.drawFastHLine(0, tft.height() - i, tft.width(), 0xF800 - (i / 4 * 0x0800));
  }
}
void loop()
{

}

ISR(TIMER2_OVF_vect) {
  cuenta++;
  if (cuenta > 29) {
    digitalWrite(13, ESTADO);
    ESTADO = !ESTADO;
    cuenta = 0;
  }
  if (current_screen = 1) {
    if (Gear != last_Gear) {
      displayGear(Gear, 350, 100, 0x0000, 0x47E0, 10);
    }
    if (Speed != last_Speed) {
      displaySpeed(Speed, 150, 10, 0x0000, 0xD800, 2);
    }
    if (Fuel != last_Fuel) {
      displayFuel(Fuel, 600, 115, 0x0000, 0xC81D, 1);
    }
    if (Boost != last_Boost) {
      displayBoost(Boost, 600, 195, 0x0000, 0x0278, 1);
    }
    if (AirTemp != last_AirTemp) {
      displayAirTemp(AirTemp, 600, 275, 0x0000, 0x0278, 1);
    }
    if (LastLapTime != last_LastLapTime) {
      displayLastLapTime(LastLapTime, 150, 120, 0x0000, 0xC81D, 0);
    }
    if (BestLapTime != last_LastBestTime) {
      displayBestLapTime(BestLapTime, 150, 175, 0x0000, 0xC81D, 0);
    }
    if (Position != last_Position) {
      displayPosition(Position, 665, 10, 0x0000, 0xDF00, 2);
    }
    if (CompletedLaps != last_CompletedLaps) {
      displayCompletedLaps(CompletedLaps, 350, 10, 0x0000, 0x06BC, 2);
    }
    if (NumberOfLaps != last_NumberOfLaps) {
      displayNumberOfLaps(NumberOfLaps, 500, 30, 0x0000, 0x06BC, 2);
    }
    if(Pit != last_Pit){
      displayPit(Pit, 160, 290, 0x0000, 0xEFE0, 2);
    }
  }
}
