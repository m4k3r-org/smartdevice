/*
    kitchen_timer APP, by https://github.com/hideaki-kawahara/kitchen_timer/tree/develop
    Version 2.0   RGB888 24 bit color works!  2.1.2020
    Version 1.1   working with m5stack，  2.1.2020
    m5stack fire arduino device test app for kitchen_timer
    Author ling zhou, 30.12.2019
    note: need real device test
    note2: tst lcd lib modified for RGB888:
    void TFT_eSPI::setTextColor(uint32_t c, uint32_t b)
*/
#define M5STACK_200Q //// define must ahead #include <M5Stack.h>

#include <M5Stack.h>
//for use as bin app lovyan03
#include <M5StackUpdater.h> // https://github.com/tobozo/M5Stack-SD-Updater/
//#include <M5TreeView.h> // https://github.com/lovyan03/M5Stack_TreeView
//#include <M5OnScreenKeyboard.h> // https://github.com/lovyan03/M5Stack_OnScreenKeyboard/



#include <Adafruit_NeoPixel.h>

//Hardware pins
#define M5STACK_FIRE_NEO_NUM_LEDS  10
#define M5STACK_FIRE_NEO_DATA_PIN  15
#define M5STACKFIRE_MICROPHONE_PIN 34

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(M5STACK_FIRE_NEO_NUM_LEDS, M5STACK_FIRE_NEO_DATA_PIN, NEO_GRB + NEO_KHZ800);
POWER m5_power;

unsigned long run_cnt = 0;


boolean but_A = false, but_LEFT = false, but_RIGHT = false;
uint32_t targetTime = 0;
byte omm = 99, oss = 99;
uint8_t mm = 3;
int8_t ss = 0;
bool hasStop = true;
bool hasEnd = false;


float accX = 0.0F;
float accY = 0.0F;
float accZ = 0.0F;

#define CURRENT_400MA  (0x01 << 2)
//#define M5STACK_MPU6886 
//#define M5STACK_MPU9250 
// #define M5STACK_MPU6050


byte red = 31;
byte green = 0;
byte blue = 0;
byte state = 0;
unsigned int color = red << 11; // Colour order is RGB 5+6+5 bits each
uint8_t screen_dir = 0;

void setup() {
  M5.begin();
  M5.Lcd.setBrightness(50);  //define BLK_PWM_CHANNEL 7  PWM
  rainbow_fill();
  //for app flash back
  if (digitalRead(BUTTON_A_PIN) == 0) {
    Serial.println("Will Load menu binary");
    updateFromFS(SD);
    ESP.restart();
  }
  delay(500);
    m5_power.begin();//M5.powerOFF(); soft power off
  m5_power.setVinMaxCurrent(CURRENT_400MA );
  M5.Lcd.fillScreen(C_RGB565(TFT_IVORY)); //In_eSPI.H
  //void writeBlock(uint16_t color, uint32_t repeat)
  //void TFT_eSPI::fillScreen(uint32_t color)
  //for app flash back
  if (digitalRead(BUTTON_A_PIN) == 0) {
    Serial.println("Will Load menu binary");
    updateFromFS(SD);
    ESP.restart();
  }


  targetTime = millis() + 1000;
  screen_dir = M5.Lcd.getRotation();
  M5.Lcd.setTextFont(2);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor888(TFT_BLACK, TFT_IVORY);
  M5.Lcd.setCursor( 60, 200);
  M5.Lcd.print("M");
  M5.Lcd.setCursor( 145, 200);
  M5.Lcd.print("S");
  M5.Lcd.setTextSize(1);
  M5.Lcd.print("/Reset");
  //M5.Lcd.setCursor( 240, 220);
  M5.Lcd.setCursor( 240, 200);
  M5.Lcd.setTextColor(TFT_Crimson, TFT_IVORY);
  M5.Lcd.print("START");
  M5.Lcd.setCursor( 240, 220);
  M5.Lcd.print("/STOP");

  Serial.println("START");

  M5.IMU.Init(); //define first
  
  
}

void loop() {
  //screen auto rotate,setRotation
  M5.IMU.getAccelData(&accX,&accY,&accZ);
  if (accY<-0.4){
  M5.Lcd.setRotation(screen_dir+2);
  }
  else if (accY>0.4){
    M5.Lcd.setRotation(screen_dir);
    //setRotation
  }

  
  if (M5.BtnC.wasPressed() && !hasStop) {
    hasStop = true;
  } else if (M5.BtnC.wasPressed() && hasStop) {
    hasStop = false;
  }
  if (M5.BtnB.wasPressed() && hasStop) {
    ss++;
    hasEnd = false;
    if (ss == 60) {
      ss = 0;
    }
    draw_seconds(ss);
  }
  if (M5.BtnA.wasPressed() && hasStop) {
    mm++;
    hasEnd = false;
    if (mm == 60) {
      mm = 0;
    }
    draw_minutes(mm);
  }
  M5.update();
  //long press BtnB to reset timer!
  if (M5.BtnB.pressedFor(500)) {
    buttonB_longPressed();
  }
  //run every second
  if (targetTime < millis()) {
    targetTime = millis() + 1000;

    // 時間停止してない
    if (!hasStop) {
      ss--;
      if (ss == -1) {
        // 分秒ともに0なら表示停止＆timerストップ
        // Timer count down to zero
        if (mm == 0) {
          ss = 0;
          hasStop = true;
          hasEnd = true;
          M5.Speaker.beep();
          delay(100);
          M5.Speaker.beep();
          //rgb
          for (uint8_t n = 0; n < M5STACK_FIRE_NEO_NUM_LEDS; n++)
          {
            pixels.setPixelColor(n, pixels.Color(100 + n * 10, 10, 10));
          }
        } else {
          ss = 59;
          omm = mm;
          mm--;
        }
      }
    }

    if (hasEnd) {
      // timerストップの表示
      M5.Lcd.setCursor( 100, 150);
      M5.Lcd.setTextFont(2);
      M5.Lcd.print("Timer Stop");
      //rgb
      for (uint8_t n = 0; n < M5STACK_FIRE_NEO_NUM_LEDS; n++)
      {
        pixels.setPixelColor(n, pixels.Color(100 + n * 10, 10, 10));
      }
    } else {
      // 表示を消す
      M5.Lcd.setCursor( 100, 150);
      M5.Lcd.setTextFont(2);
      M5.Lcd.print("          ");
    }

    // 7seg font
    M5.Lcd.setTextFont(7);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor888(TFT_BLACK, TFT_IVORY);

    // 分の更新
    if (omm != mm) {
      omm = mm;
      draw_minutes(mm);
    }

    // 秒の更新
    if (oss != ss) {
      oss = ss;

      // インジケーターの点滅
      if (ss % 2 && !hasStop) {
        M5.Lcd.setCursor( 150, 40);
        M5.Lcd.setTextColor888(TFT_IVORY, TFT_IVORY);
        M5.Lcd.print(":");
        M5.Lcd.setTextColor888(TFT_BLACK, TFT_IVORY);
      }
      else {
        M5.Lcd.setCursor( 150, 40);
        M5.Lcd.print(":");
      }

      draw_seconds(ss);
    }


    //SYS  GUI

    int Akku_level = m5_power.getBatteryLevel();
    //  M5.Lcd.setCursor(0, 0);
    //  M5.Lcd.setTextSize(2); //size 2 to 8
    //  M5.Lcd.setTextColor(ORANGE, BLACK);
    //  M5.Lcd.printf("Akku: %3d%%   Run:%d", Akku_level, run_cnt);
  }
}

// 分の表示
void draw_minutes(int mm) {
  if (mm < 10) {
    M5.Lcd.setCursor( 0, 40);
    M5.Lcd.print("0");
    M5.Lcd.print(mm);
  } else {
    M5.Lcd.setCursor( 0, 40);
    M5.Lcd.print(mm);
  }
}

// 秒の表示
void draw_seconds(int ss) {
  if (ss < 10) {
    M5.Lcd.setCursor( 180, 40);
    M5.Lcd.print("0");
    M5.Lcd.print(ss);
  } else {
    M5.Lcd.setCursor( 180, 40);
    M5.Lcd.print(ss);
  }
}

//reset timer
void buttonB_longPressed(void) {
  //M5.Speaker.beep();  // too laud
  //M5.Speaker.tone(800, 20);
  //M5.lcd.setBrightness(30);


  Serial.println(" --^^^--- buttonA/S1 is longPressed over 0.5s !!!");
  mm = 0;
  ss = 0;

}

uint16_t C_RGB565 ( uint32_t color) {
  uint16_t RGB565_color;
  if ( color > 0xFFFF) {
    RGB565_color = (((color & 0xf80000) >> 8) + ((color & 0xfc00) >> 5) + ((color & 0xf8) >> 3));
    return RGB565_color;
  }
  else return color;
}


// Fill screen with a rainbow pattern
void rainbow_fill()
{
  // The colours and state are not initialised so the start colour changes each time the funtion is called
  int rotation = M5.Lcd.getRotation();
  M5.Lcd.setRotation(random(4));
  for (int i = M5.Lcd.height() - 1; i >= 0; i--) {
    // This is a "state machine" that ramps up/down the colour brightnesses in sequence
    switch (state) {
      case 0:
        green ++;
        if (green == 64) {
          green = 63;
          state = 1;
        }
        break;
      case 1:
        red--;
        if (red == 255) {
          red = 0;
          state = 2;
        }
        break;
      case 2:
        blue ++;
        if (blue == 32) {
          blue = 31;
          state = 3;
        }
        break;
      case 3:
        green --;
        if (green == 255) {
          green = 0;
          state = 4;
        }
        break;
      case 4:
        red ++;
        if (red == 32) {
          red = 31;
          state = 5;
        }
        break;
      case 5:
        blue --;
        if (blue == 255) {
          blue = 0;
          state = 0;
        }
        break;
    }
    color = red << 11 | green << 5 | blue;
    // Draw a line 1 pixel wide in the selected colour
    M5.Lcd.drawFastHLine(0, i, M5.Lcd.width(), color); // in this example M5.Lcd.width() returns the pixel width of the display
  }
  M5.Lcd.setRotation(rotation);
}
