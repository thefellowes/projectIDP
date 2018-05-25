#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Ogenbmp.h"


#define OLED_RESET 4
#define AMOUNT_OF_SLIDES 12
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
#define BUTTON_PIN 2

Adafruit_SSD1306 display_1(OLED_RESET);
Adafruit_SSD1306 display_2(OLED_RESET);
int i = 0;
int button_state = 0;

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!, change #define SSD1306_128_32 to #define SSD1306_128_64");
#endif


void setup()
{
  i = 0;
  Serial.begin(9600);
//  //led
//  pinMode(6, OUTPUT);
//  digitalWrite(6, HIGH);
  
  display_1.begin(SSD1306_SWITCHCAPVCC, 0x3D);  // initialize with the I2C addr 0x3C 
  display_2.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // clear display buffers
  display_1.clearDisplay();
  display_2.clearDisplay();

  Serial.print("setup complete");

  display_1.drawBitmap(0, 0,  Oog_0, 128, 64, 1);
  display_2.drawBitmap(0, 0,  Oog_0, 128, 64, 1);
  display_1.invertDisplay(true);
  display_2.invertDisplay(true);
  display_1.display();
  display_2.display();

  // initialize the pushbutton pin as an input:
  pinMode(BUTTON_PIN, INPUT);
  
}


void loop()
{
  Serial.print(i);
  
  // read the state of the pushbutton value:
  button_state = digitalRead(BUTTON_PIN);
  
  int op = i;
  switch(op) {
    case 0:
        display_1.drawBitmap(0, 0, Oog_0, 128, 64, 1); // write bitmap to buffer
        display_1.display(); // display buffer
        display_1.clearDisplay(); // clear buffer
        display_2.drawMirroredBitmap(0, 0, Oog_0, 128, 64, 1); //write mirrored bitmap to buffer
        display_2.display();
        display_2.clearDisplay();
        break;
    case 1:
        display_1.drawBitmap(0, 0, Oog_1, 128, 64, 1);
        display_1.display();
        display_1.clearDisplay();
        display_2.drawMirroredBitmap(0, 0, Oog_1, 128, 64, 1);
        display_2.display();
        display_2.clearDisplay();
        break;
    case 2:
        display_1.drawBitmap(0, 0, Oog_2, 128, 64, 1);
        display_1.display();
        display_1.clearDisplay();
        display_2.drawMirroredBitmap(0, 0, Oog_2, 128, 64, 1);
        display_2.display();
        display_2.clearDisplay();
        break;
    case 3:
        display_1.drawBitmap(0, 0, Oog_3, 128, 64, 1);
        display_1.display();
        display_1.clearDisplay();
        display_2.drawMirroredBitmap(0, 0, Oog_3, 128, 64, 1);
        display_2.display();
        display_2.clearDisplay();
        break;
    case 4:
        display_1.drawBitmap(0, 0, Oog_4, 128, 64, 1);
        display_1.display();
        display_1.clearDisplay();
        display_2.drawMirroredBitmap(0, 0, Oog_4, 128, 64, 1);
        display_2.display();
        display_2.clearDisplay();
        break;
    case 5:
        display_1.drawBitmap(0, 0, Oog_5, 128, 64, 1);
        display_1.display();
        display_1.clearDisplay();
        display_2.drawMirroredBitmap(0, 0, Oog_5, 128, 64, 1);
        display_2.display();
        display_2.clearDisplay();
        break;
    case 6:
        display_1.drawBitmap(0, 0, Oog_6, 128, 64, 1);
        display_1.display();
        display_1.clearDisplay();
        display_2.drawMirroredBitmap(0, 0, Oog_6, 128, 64, 1);
        display_2.display();
        display_2.clearDisplay();
        break;
    case 7:
        display_1.drawBitmap(0, 0, Oog_7, 128, 64, 1);
        display_1.display();
        display_1.clearDisplay();
        display_2.drawMirroredBitmap(0, 0, Oog_7, 128, 64, 1);
        display_2.display();
        display_2.clearDisplay();
        break;
    case 8:
        display_1.drawBitmap(0, 0, Oog_8, 128, 64, 1);
        display_1.display();
        display_1.clearDisplay();
        display_2.drawMirroredBitmap(0, 0, Oog_8, 128, 64, 1);
        display_2.display();
        display_2.clearDisplay();
        break;
    case 9:
        display_1.drawBitmap(0, 0, Oog_9, 128, 64, 1);
        display_1.display();
        display_1.clearDisplay();
        display_2.drawMirroredBitmap(0, 0, Oog_9, 128, 64, 1);
        display_2.display();
        display_2.clearDisplay();
        break;
    case 10:
        display_1.drawBitmap(0, 0, Oog_10, 128, 64, 1);
        display_1.display();
        display_1.clearDisplay();
        display_2.drawMirroredBitmap(0, 0, Oog_10, 128, 64, 1);
        display_2.display();
        display_2.clearDisplay();
        break;
    case 11:
        display_1.drawBitmap(0, 0, Oog_11, 128, 64, 1);
        display_1.display();
        display_1.clearDisplay();
        display_2.drawMirroredBitmap(0, 0, Oog_11, 128, 64, 1);
        display_2.display();
        display_2.clearDisplay();
        break;                                
    default:
        display_1.drawBitmap(0, 0, Oog_0, 128, 64, 1);
        display_1.display();
        display_1.clearDisplay();
        display_2.drawMirroredBitmap(0, 0, Oog_0, 128, 64, 1);
        display_2.display();
        display_2.clearDisplay();
  }
  ++i;
  if (button_state == LOW){
        display_1.drawBitmap(0, 0, Oog_12, 128, 64, 1);
        display_1.display();
        display_1.clearDisplay();
        display_2.drawMirroredBitmap(0, 0, Oog_12, 128, 64, 1);
        display_2.display();
        display_2.clearDisplay();
        delay(1000);
        
        while(button_state == LOW){
label_d:
          display_1.drawBitmap(0, 0, Oog_13, 128, 64, 1);
          display_1.display();
          display_1.clearDisplay();
          display_2.drawMirroredBitmap(0, 0, Oog_13, 128, 64, 1);
          display_2.display();
          display_2.clearDisplay();
          button_state = digitalRead(BUTTON_PIN);
        }
        delay(1000);button_state = digitalRead(BUTTON_PIN);
        button_state = digitalRead(BUTTON_PIN);
        if (button_state == LOW)
          goto label_d;
          
        i = 0;
  }
  else if (i >= AMOUNT_OF_SLIDES) {
    delay(3000);
    i = 0;
  }
//  delay(10);
}
