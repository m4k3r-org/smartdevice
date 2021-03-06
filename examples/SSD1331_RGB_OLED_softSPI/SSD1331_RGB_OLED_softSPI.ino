/***************************************************
  This is a example sketch demonstrating the graphics
  capabilities of the SSD1331 library  for the 0.96"
  16-bit Color OLED with SSD1331 driver chip
  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/684
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!
  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution

  with software SPI works!
  ESP32 use HSPI mode 3 not working!
  mode3 means when CPOL=1, CPHA=1. When SPI is idle, the clock output is logic high; data changes on
  the falling edge of the SPI clock and is sampled on the rising edge.
 ****************************************************/

#include <Adafruit_GFX.h>  //https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts
#include <Adafruit_SSD1331.h>
#include <SPI.h>

//#include<ESP32_SSD1331.h>
/*
  // You can use any (4 or) 5 pins
  #define sclk 13
  #define mosi 11
  #define cs   10
  #define rst  9
  #define dc   8
*/
//define the pins used by the 0.95 RGB OLED SPI ssd1331 module
//#define SCK 18
//#define MISO 19
//#define DC 27
//#define MOSI 23
//#define CS 5
////#define CS 14 //m5
//#define RST 33

//#define FSPI  1 //SPI bus attached to the flash (can use the same data lines but different SS)
//#define HSPI  2 //SPI bus normally mapped to pins 12 - 15, but can be matrixed to any pins, micro SD card interface for TTGO?
//#define VSPI  3 //SPI bus normally attached to pins 5, 18, 19 and 23, but can be matrixed to any pins


//ori lib def vspi
#define SCK 18
#define MOSI 23
#define DC 27
#define CS 15
#define RST 4
//SCLK-----GPIO #14
//MOSI-----GPIO #13
//MISO-----NONE
//CS-------GPIO #15
//D/C------GPIO #21
//RST------GPIO #4
//#define SPI_INTERFACES_COUNT 1
//#define SCK 14
//#define DC 27 //_C low for CMD
//#define MOSI 13
//#define MISO 12
//#define CS 15
//#define RST 4

#define SPI_Freq 7000000  //6.7M   5.714MHz
#define LED_pin 5

//ESP32_SSD1331 display = ESP32_SSD1331(SCK, MISO,MOSI,CS,DC,RST);

// Color definitions
#define  BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF
#define MilkWhite           0xCFCF
bool HardwareSPI_EN = false;
const uint8_t   OLED_pin_scl_sck        = 18;
const uint8_t   OLED_pin_sda_mosi       = 23;
const uint8_t   OLED_pin_cs_ss          = 15;
const uint8_t   OLED_pin_res_rst        = 4;
const uint8_t   OLED_pin_dc_rs          = 27;
// Option 1: use any pins but a little slower
//Adafruit_SSD1331 display = Adafruit_SSD1331(cs, dc, mosi, sclk, rst);
//Adafruit_SSD1331 display = Adafruit_SSD1331(CS, DC, MOSI, SCK, RST);
// declare the display with soft SPI @ about 1M, test ok!
Adafruit_SSD1331 display =
  Adafruit_SSD1331(
    OLED_pin_cs_ss,
    OLED_pin_dc_rs,
    OLED_pin_sda_mosi,
    OLED_pin_scl_sck,
    OLED_pin_res_rst
  );
//---------------------------------------------------------------------
// Option 2: must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)

//SPIClass hwspi(HSPI); //hspi->begin(0, 2, 4, 33); //SCLK, MISO, MOSI, SS
SPIClass hwspi(VSPI); //SCLK = 18, MISO = 19, MOSI = 23, SS = 5
//Adafruit_SSD1331 display = Adafruit_SSD1331(&SPI, OLED_pin_cs_ss, OLED_pin_dc_rs, OLED_pin_res_rst );
//Adafruit_SSD1331 display = Adafruit_SSD1331(OLED_pin_cs_ss, OLED_pin_dc_rs, OLED_pin_res_rst );//not working
//Adafruit_SSD1331 display = Adafruit_SSD1331(&hwspi, OLED_pin_cs_ss, OLED_pin_dc_rs, OLED_pin_res_rst ); //init ok, then not working
//Adafruit_SSD1331 display = Adafruit_SSD1331(&hwspi, CS, DC, RST); //working! default generate 8M


float p = 3.1415926;
uint16_t  run_cnt = 0;
uint8_t led_status = 0;
void LED_run(void) {

  led_status = 1 - led_status;
  if (led_status)
    digitalWrite(LED_pin, HIGH);
  else
    digitalWrite(LED_pin, LOW);

}


void setup(void) {

  pinMode(LED_pin, OUTPUT); //RES
  digitalWrite(LED_pin, LOW);

  if (HardwareSPI_EN) {
    hwspi.begin(18, 19, 23, 32); //SCLK, MISO, MOSI, SS
    hwspi.setFrequency(SPI_Freq);
    display.begin(SPI_Freq); //adfruit
  }
  else
    display.begin();

  //SPI.begin(SCK, MISO, MOSI, CS);
  Serial.begin(115200);
  Serial.println("hello! Adafruit_SSD1331 SW SPI driver test");

  //  hwspi.begin(SCK, MISO, MOSI, CS);
  //  hwspi.setFrequency(SPI_Freq);
  //  hwspi.setDataMode(SPI_MODE3);
  //  hwspi.setHwCs(true);

  //Adafruit_SSD1331 display = Adafruit_SSD1331(&hwspi, OLED_pin_cs_ss, OLED_pin_dc_rs, OLED_pin_res_rst );
  //    spi.setFrequency(_freq); //SSD1331 のSPI Clock Cycle Time 最低150ns
  //  spi.setDataMode(SPI_MODE3);
  //  spi.setHwCs(bl);
  //hwspi.begin(SCK, MISO, MOSI, CS); //HW CS
  //   pinMode(RST, OUTPUT); //RES
  //  pinMode(DC, OUTPUT); //DC
  //
  //  digitalWrite(RST, HIGH);
  //  digitalWrite(RST, LOW);
  //  delay(1);
  //  digitalWrite(RST, HIGH);
  //
  //  digitalWrite(DC, HIGH);




  Serial.println("init");


  //display.fillScreen(BLUE);



  //delay(500);
  Serial.println("start screen test now!");
  uint16_t time = millis();
  //1st test
  lcdTestPattern(); //works
  uint16_t TT =  millis() - time;
  Serial.println("lcdTestPattern() finish");
  Serial.printf("time used: %d ms\r\n", TT);
  //display.Drawing_Pixel_65kColor(32,32, 255,0,0);


  //2st
  delay(1000);
  Serial.println("fillScreen black");
  //display.startWrite();
  display.fillScreen(BLACK);
  //display.endWrite();
  Serial.println("Print text: RGB OLED SWSPI driver test v0.9");
  TT =   millis();
  display.setCursor(0, 0);
  display.print("RGB OLED SWSPI driver test v0.9");
   TT =  millis() - TT;
  Serial.printf("time used: %d ms\r\n", TT);
  delay(1000);

  // tft print function!
  Serial.println("tftPrintTest");
  display.print("tftPrintTest");
  tftPrintTest();
  delay(2000);

  Serial.println("YELLOW_testlines");
  TT =   millis();
  testlines(YELLOW);//not working, woking now @4.1
  //Serial.println("YELLOW_testlines finish");
   TT =  millis() - TT;
  Serial.printf("time used: %d ms\r\n", TT);
  delay(500);

  // optimized lines
  Serial.println("testfastlines");
  //  LED_run();
  //  LED_run();
  testfastlines(RED, BLUE);//not working
  delay(500);

  Serial.println("testdrawrects");
  TT =   millis();
  testdrawrects(GREEN);
  TT =  millis() - TT;
  Serial.printf("time used: %d ms\r\n", TT);
  delay(1000);
  Serial.println("testfillrects");
  testfillrects(YELLOW, MAGENTA);
  delay(1000);
  Serial.println("testfillcircles");
  display.fillScreen(BLACK);
  testfillcircles(10, BLUE);
  Serial.println("testdrawcircles");
  testdrawcircles(10, WHITE);
  delay(1000);

  Serial.println("testroundrects");
  testroundrects();
  delay(500);

  Serial.println("testtriangles");
  testtriangles();
  delay(500);

  time = millis() - time;

  Serial.printf("time used: %d ms\r\n", time);
  Serial.println("done");
  delay(1000);
}

void loop() {

  uint32_t tt;
  LED_run();
  run_cnt++;

  uint8_t pixel_x = run_cnt % 95;
  uint8_t pixel_y = run_cnt % 63;
  uint8_t R =  run_cnt % 32;
  uint16_t color_R = R << 11;
  uint16_t color_G = R << 5;
  uint16_t color_B = R ;

  if  (run_cnt > 12000) {
    color_R = random (0, 65535);
    display. writePixel(pixel_x, pixel_y, color_R);
  }
  else if (run_cnt > 12000)
    display. writePixel(pixel_x, pixel_y, color_B);
  else if (run_cnt > 6000)
    display. writePixel(pixel_x, pixel_y, color_G);
  else
    display. writePixel(pixel_x, pixel_y, color_R);
  run_cnt++;
  if (run_cnt > 30000)
    run_cnt = 0;

  if (run_cnt % 1000 == 0) {
    tt =  millis() - tt;
    Serial.printf("loop %d,time cost: %d ms\r\n", run_cnt, tt);
    tt = millis();
  }
  delay(2);
}

void testlines(uint16_t color) {
  display.fillScreen(BLACK);
  for (int16_t x = 0; x < display.width() - 1; x += 6) {
    display.drawLine(0, 0, x, display.height() - 1, color);
  }
  for (int16_t y = 0; y < display.height() - 1; y += 6) {
    display.drawLine(0, 0, display.width() - 1, y, color);
  }

  display.fillScreen(BLACK);
  for (int16_t x = 0; x < display.width() - 1; x += 6) {
    display.drawLine(display.width() - 1, 0, x, display.height() - 1, color);
  }
  for (int16_t y = 0; y < display.height() - 1; y += 6) {
    display.drawLine(display.width() - 1, 0, 0, y, color);
  }

  // To avoid ESP8266 watchdog timer resets when not using the hardware SPI pins
  delay(0);

  display.fillScreen(BLACK);
  for (int16_t x = 0; x < display.width() - 1; x += 6) {
    display.drawLine(0, display.height() - 1, x, 0, color);
  }
  for (int16_t y = 0; y < display.height() - 1; y += 6) {
    display.drawLine(0, display.height() - 1, display.width() - 1, y, color);
  }

  display.fillScreen(BLACK);
  for (int16_t x = 0; x < display.width() - 1; x += 6) {
    display.drawLine(display.width() - 1, display.height() - 1, x, 0, color);
  }
  for (int16_t y = 0; y < display.height() - 1; y += 6) {
    display.drawLine(display.width() - 1, display.height() - 1, 0, y, color);
  }

}

void testdrawtext(char *text, uint16_t color) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  for (uint8_t i = 0; i < 168; i++) {
    if (i == '\n') continue;
    display.write(i);
    if ((i > 0) && (i % 21 == 0))
      display.println();
  }
}

void testfastlines(uint16_t color1, uint16_t color2) {
  display.fillScreen(BLACK);
  for (int16_t y = 0; y < display.height() - 1; y += 5) {
    display.drawFastHLine(0, y, display.width() - 1, color1);
  }
  for (int16_t x = 0; x < display.width() - 1; x += 5) {
    display.drawFastVLine(x, 0, display.height() - 1, color2);
  }
}

void testdrawrects(uint16_t color) {
  display.fillScreen(BLACK);
  for (int16_t x = 0; x < display.height() - 1; x += 6) {
    display.drawRect((display.width() - 1) / 2 - x / 2, (display.height() - 1) / 2 - x / 2 , x, x, color);
  }
}

void testfillrects(uint16_t color1, uint16_t color2) {
  display.fillScreen(BLACK);
  for (int16_t x = display.height() - 1; x > 6; x -= 6) {
    display.fillRect((display.width() - 1) / 2 - x / 2, (display.height() - 1) / 2 - x / 2 , x, x, color1);
    display.drawRect((display.width() - 1) / 2 - x / 2, (display.height() - 1) / 2 - x / 2 , x, x, color2);
  }
}

void testfillcircles(uint8_t radius, uint16_t color) {
  for (uint8_t x = radius; x < display.width() - 1; x += radius * 2) {
    for (uint8_t y = radius; y < display.height() - 1; y += radius * 2) {
      display.fillCircle(x, y, radius, color);
    }
  }
}

void testdrawcircles(uint8_t radius, uint16_t color) {
  for (int16_t x = 0; x < display.width() - 1 + radius; x += radius * 2) {
    for (int16_t y = 0; y < display.height() - 1 + radius; y += radius * 2) {
      display.drawCircle(x, y, radius, color);
    }
  }
}

void testtriangles() {
  display.fillScreen(BLACK);
  int color = 0xF800;
  int t;
  int w = display.width() / 2;
  int x = display.height();
  int y = 0;
  int z = display.width();
  for (t = 0 ; t <= 15; t += 1) {
    display.drawTriangle(w, y, y, x, z, x, color);
    x -= 4;
    y += 4;
    z -= 4;
    color += 100;
  }
}

void testroundrects() {
  display.fillScreen(BLACK);
  int color = 100;
  int i;
  int t;
  for (t = 0 ; t <= 4; t += 1) {
    int x = 0;
    int y = 0;
    int w = display.width();
    int h = display.height();
    for (i = 0 ; i <= 8; i += 1) {
      display.drawRoundRect(x, y, w, h, 5, color);
      x += 2;
      y += 3;
      w -= 4;
      h -= 6;
      color += 1100;
    }
    color += 100;
  }
}

void tftPrintTest() {
  display.fillScreen(BLACK);
  display.setCursor(0, 5);
  display.setTextColor(RED);
  display.setTextSize(1);
  display.println("Hello World!");
  display.setTextColor(YELLOW, GREEN);
  display.setTextSize(2);
  display.print("Hello Wo");
  display.setTextColor(BLUE);
  display.setTextSize(3);
  display.print(1234.567);
  delay(1500);
  display.setCursor(0, 5);
  display.fillScreen(BLACK);
  display.setTextColor(WHITE);
  display.setTextSize(0);
  display.println("Hello World!");
  display.setTextSize(1);
  display.setTextColor(GREEN);
  display.print(p, 5);
  display.println(" Want pi?");
  display.print(8675309, HEX); // print 8,675,309 out in HEX!
  display.print(" Print HEX");
  display.setTextColor(WHITE);
  display.println("Sketch has been");
  display.println("running for: ");
  display.setTextColor(MAGENTA);
  display.print(millis() / 1000);
  display.setTextColor(WHITE);
  display.print(" seconds.");
}

void mediabuttons() {
  // play
  display.fillScreen(BLACK);
  display.fillRoundRect(25, 10, 78, 60, 8, WHITE);
  display.fillTriangle(42, 20, 42, 60, 90, 40, RED);
  delay(500);
  // pause
  display.fillRoundRect(25, 90, 78, 60, 8, WHITE);
  display.fillRoundRect(39, 98, 20, 45, 5, GREEN);
  display.fillRoundRect(69, 98, 20, 45, 5, GREEN);
  delay(500);
  // play color
  display.fillTriangle(42, 20, 42, 60, 90, 40, BLUE);
  delay(50);
  // pause color
  display.fillRoundRect(39, 98, 20, 45, 5, RED);
  display.fillRoundRect(69, 98, 20, 45, 5, RED);
  // play color
  display.fillTriangle(42, 20, 42, 60, 90, 40, GREEN);
}

/**************************************************************************/
/*!
    @brief  Renders a simple test pattern on the LCD
*/
/**************************************************************************/

//time used: 579 ms @2M HWSPI VSPI, 420 ms @ 7M(6.6) HWSPI VSPI
//472 ms @ 919K software SPI same pin

void lcdTestPattern(void)
{
  uint8_t w, h;
  //display.setAddrWindow(0, 0, 96, 64); //actually write 0,0,95,63.  include startWrite() at the end of setAddrWindow;
  display.writePixel(0, 0, WHITE); //writePixels works!
  display.endWrite(); //needed, in
  display.writePixel(2, 0, WHITE); //writePixels
  display.endWrite();
  display.writePixel(4, 0, RED); //writePixels
  display.endWrite();
  //display.startWrite();
  //display color bars
  for (h = 0; h < 64; h++) {
    for (w = 0; w < 96; w++) {
      if (w > 83) {
        //display.drawPixel(w, h, WWWW);//not working
        display.writePixel(w, h, MilkWhite);
        //display.endWrite();
      } else if (w > 71) {
        display.writePixel(w, h, BLUE);
        //display.endWrite();
      } else if (w > 59) {
        display.writePixel(w, h, GREEN);
        //display.endWrite();
      } else if (w > 47) {
        display.writePixel(w, h, CYAN);
        //display.endWrite();
      } else if (w > 35) {
        display.writePixel(w, h, RED);
        //display.endWrite();
      } else if (w > 23) {
        display.writePixel(w, h, MAGENTA);
        //display.endWrite();
      } else if (w > 11) {
        display.writePixel(w, h, YELLOW);
        //display.endWrite();
      } else {
        // Serial.println("fisrt bar Black");
        // display.drawPixel(w, h, WHITE);

        display.writePixel(w, h, WHITE); //signle bar cost 8.5ms @2M HWSPI
        //display.endWrite();
      }
    }
    //delay(1);
  }
  //display.writePixel(0, 0, WHITE); //writePixels works!
  //display.writePixel(1, 0, WHITE); //writePixels works!
  display.endWrite();
}

//---run results
//init
//start screen test now!
//lcdTestPattern() finish
//time used: 472 ms
//fillScreen black
//Print text: RGB OLED driver test v0.9
//tftPrintTest
//YELLOW_testlines
//time used: 7562 ms
//testfastlines
//testdrawrects
//time used: 8847 ms
//testfillrects
//testfillcircles
//testdrawcircles
//testroundrects
//testtriangles
//time used: 14173 ms
//done
//loop 1000,time cost: 18027 ms
//loop 2000,time cost: 19027 ms
