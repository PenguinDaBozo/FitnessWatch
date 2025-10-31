# Day 1 - 10/20/25
<img width="500" height="500" alt="image" src="https://github.com/user-attachments/assets/845e6ced-2b89-4b47-ad93-eb5b1afc64ff" />

- PCBs I ordered... except um I don't think they work



- I don't think ESP dev kit works or maybe I got wrong usb cable... anyway I ordered some hopefully this board works or I just got scammed T-T
- Arduino Uno works fine with the TFT round display

<img width="500" height="500" alt="image" src="https://github.com/user-attachments/assets/b0d78cdd-9605-4bd8-8356-64391727e1e6" />

<img width="500" height="500" alt="image" src="https://github.com/user-attachments/assets/21759664-1880-4438-8099-33ef92634d4b" />


- but for some reason Arduiono Nano fades...even with D13(SDA) and D11(SCL) I wonder why. GPT says it's cuz low power and I lowkey agree because capacitator couldn't smooth out the flow 

<img width="500" height="500" alt="image" src="https://github.com/user-attachments/assets/638de216-6cce-4337-986c-732746815d4a" />

# Day 2 - 10/22/25
- ESP32 pico kit 1 has like no documentation... this is kind of sad
- got le wire today and had to figure out how to connect COM. thankfully GPT recommended the driver and shoutout to randomtechstuff on how to download
- finally figured out how to get the TFT display working after 2 hrs and half (GPT the goat)

- I dont understand the schematic :C

- I have to initialize the sda and scl thing apparently

- Next step: figure out internet stuff and then find out how to continually refresh screen except i dont got buttons...ummm

# Day 3 - 10/23/25
- figured out how to do internet stuff -> uses html so js might be an option
- bluetooth works although its a bit funky

## Design plan

<img width="500" height="500" alt="image" src="https://github.com/user-attachments/assets/2f47bae3-6184-4e50-ac74-4d3a0e19a864" />

- every page will have the dots so you know which page your on
- main screen is going to have the time date and day
- fitness page has steps, heartrate, and flights idk what else i can add
- wait how will stop watch work if no buttons
- weather will use temperature sensor and barometer
- compass uses imu
- setting is tbd
- four buttons potentially except it will be hard to manage the buttons so i might stick to two


figured out how to switch between states its very cool

<img width="500" height="500" alt="image" src="https://github.com/user-attachments/assets/5c6211d0-f45a-4cf2-9ab0-edb1645ae22a" />

<img width="500" height="500" alt="image" src="https://github.com/user-attachments/assets/c6f41da7-12a2-426b-bf0a-2373ce79c0cf" />

current code:
```
#include "spinlock.h"
#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"

// TFT display
#define TFT_SDA 13
#define TFT_SCL 14
#define TFT_DC 25
#define TFT_CS 26
#define TFT_RST 27 

// Buttons
#define LFT_BUTTON 20
#define RGT_BUTTON 21

#define NUM_SCREEN 3

int currentScreen = 0;

Adafruit_GC9A01A tft(TFT_CS, TFT_DC);

void setup() {
  Serial.begin(115200);
  Serial.println("GC9A01A running...");

  SPI.begin(TFT_SCL, -1, TFT_SDA);

  tft.begin();
  tft.setSPISpeed(12000000);

  pinMode(LFT_BUTTON, INPUT);
  pinMode(RGT_BUTTON, INPUT);

  Serial.println(F("Benchmark           Time (microseconds)"));
  delay(10);
  
}

void loop() {
  if(digitalRead(LFT_BUTTON) == HIGH) {
    Serial.println("prev");
    prevScreen();
    updateScreen();
  }
  if(digitalRead(RGT_BUTTON) == HIGH) {
    Serial.println("next");
    nextScreen();
    updateScreen();
  }
  delay(100);
  Serial.println("nothing pressed");
}

unsigned long firstScreen() {
  tft.fillScreen(GC9A01A_BLUE);
  unsigned long start = micros();
  tft.setCursor(30,50);
  tft.setTextColor(GC9A01A_BLACK);  tft.setTextSize(5);
  tft.println("Main page");
  return micros() - start;
}

unsigned long secondScreen() {
  tft.fillScreen(GC9A01A_RED);
  unsigned long start = micros();
  tft.setCursor(30,50);
  tft.setTextColor(GC9A01A_BLACK);  tft.setTextSize(5);
  tft.println("Fitness page");
  return micros() - start;
}

unsigned long thirdScreen() {
  tft.fillScreen(GC9A01A_GREEN);
  unsigned long start = micros();
  tft.setCursor(30,50);
  tft.setTextColor(GC9A01A_BLACK);  tft.setTextSize(5);
  tft.println("Stop watch page");
  return micros() - start;
}

void nextScreen(){
  if(currentScreen == NUM_SCREEN){
    currentScreen = 0;
  } else{
    currentScreen+=1;
  }
}
void prevScreen(){
  if(currentScreen == 0){
    currentScreen = NUM_SCREEN;
  } else{
    currentScreen-=1;
  }
}

void updateScreen(){
  switch(currentScreen){
    case 0:
      firstScreen();
      break;
    case 1:
      secondScreen();
      break;
    case 2:
      thirdScreen();
      break;
  }
}
```

Today GPT helped me with the pull down on the buttons and exploring the internet and BT functions. It also helped with brainstorming some ideas of how to implement bluetooth and internet into the watch yay

# Day 4 - 10/24/25
- I was working on the time scheduling and stuff but then realized that I did it wrong oops
- then I wanted to draw a circle around the numebrs - for some reason only the small ones were drawn
- ok but the worst of all is how the screen continually refreshes because I realized that it only updated if I click the buttons and that's because I'm dumb and I put update under the if statements
- now idk what to do but chatgpt says that i have to calculate the timing and update iff it changes aka dirty?
- i have to enum the screens and change up the code
- damn i thought this was gna be easy but ok nvm bec im not that good at coding and idk what the star next to char even does

Here's the code right now (Idk if I should copy and paste GPT, i mean idk how to even write it up tbf since Idk C++)
```
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"
#include <Wire.h>
#include <I2C_RTC.h>

// I2C connections
#define SDA 19
#define SCL 22

// TFT display
#define TFT_MOSI 13
#define TFT_SCK 14
#define TFT_DC 25
#define TFT_CS 26
#define TFT_RST 27

// Buttons
#define LFT_BUTTON 20
#define RGT_BUTTON 21

#define NUM_SCREEN 3

static Adafruit_GC9A01A tft(TFT_CS, TFT_DC);
static DS1307 RTC;

int currentScreen = 0;


void setup() {
  Serial.begin(115200);

  Wire.setPins(SDA, SCL);
  SPI.begin(TFT_SCK, -1, TFT_MOSI);
  
  RTC.begin();
  tft.begin();
  tft.setSPISpeed(12000000);

  pinMode(LFT_BUTTON, INPUT);
  pinMode(RGT_BUTTON, INPUT);

  Serial.println(F("Benchmark           Time (microseconds)"));
  delay(10);
  
}

void loop() {
  
  if(digitalRead(LFT_BUTTON) == HIGH) {
    Serial.println("prev");
    prevScreen();
  }
  if(digitalRead(RGT_BUTTON) == HIGH) {
    Serial.println("next");
    nextScreen();
  }
  updateScreen();
}

unsigned long firstScreen() {
  tft.fillScreen(GC9A01A_BLACK);
  int center_x = tft.width()/2, center_y = tft.height()/2;
  int month, hours, minutes, date, week;
  String day, month_name;
  if(RTC.isRunning()){
    
    week = RTC.getWeek();
    date = RTC.getDay();
    hours = RTC.getHours();
    minutes = RTC.getMinutes();
    month = RTC.getMonth();

  }
  switch(week){
      case 1: day = "Sun";
      break;
      case 2: day = "Mon";
      break;
      case 3: day = "Tue";
      break;
      case 4: day = "Wed";
      break;
      case 5: day = "Thu";
      break;
      case 6: day = "Fri";
      break;
      case 7: day = "Sat";
      break;
  }
  switch(month){
      case 1: month_name = "Jan";
      break;
      case 2: month_name = "Feb";
      break;
      case 3: month_name = "Mar";
      break;
      case 4: month_name = "Apr";
      break;
      case 5: month_name = "May";
      break;
      case 6: month_name = "June";
      break;
      case 7: month_name = "July";
      break;
      case 8: month_name = "Aug";
      break;
      case 9: month_name = "Sept";
      break;
      case 10: month_name = "Oct";
      break;
      case 11: month_name = "Nov";
      break;
      case 12: month_name = "Dec";
      break;
  }

  unsigned long start = micros();
  
  tft.setCursor(center_x-60,center_y-50);
  tft.setTextColor(GC9A01A_WHITE);  tft.setTextSize(2);
  tft.print(day + " " + month_name + " "); tft.println(date);
  tft.setCursor(center_x-40,center_y-30);
  tft.setTextSize(3);
  tft.print(hours); tft.print(":");
  if(minutes<10) tft.print("0");
  tft.print(minutes);

  return micros() - start;
}

unsigned long secondScreen() {
  tft.fillScreen(GC9A01A_RED);
  unsigned long start = micros();
  tft.setCursor(30,50);
  tft.setTextColor(GC9A01A_BLACK);  tft.setTextSize(5);
  tft.println("Fitness page");
  return micros() - start;
}

unsigned long thirdScreen() {
  tft.fillScreen(GC9A01A_GREEN);
  unsigned long start = micros();
  tft.setCursor(30,50);
  tft.setTextColor(GC9A01A_BLACK);  tft.setTextSize(5);
  tft.println("Stop watch page");
  return micros() - start;
}

void nextScreen(){
  if(currentScreen == NUM_SCREEN){
    currentScreen = 0;
  } else{
    currentScreen+=1;
  }
}
void prevScreen(){
  if(currentScreen == 0){
    currentScreen = NUM_SCREEN;
  } else{
    currentScreen-=1;
  }
}

void updateScreen(){
  switch(currentScreen){
    case 0:
        firstScreen();
      }
      break;
    case 1:
      secondScreen();
      break;
    case 2:
      thirdScreen();
      break;
  }
}
```

# Day 5 - 10/25/25
- Oh god. Why did I decide to do h files. Everything is messy and keeps saying redefintion
- anyway i decided to follow some of GpT's advice especialy the update and draw
- i think i understand so u have something liike a previous update = -1 and a current update and if theyre not equal then it means u have to update
- u also have the full redraw thing to make sure to redraw the whole thing when needed
- the draw function just has a bucnh of tiny modular sections of data with the dirty thing (which is like means u have to redraw)
- right now i need to figure out how to freaking have the stuff for all the files (currently i have a file that contains all the definitions and modules and packages
- also im checking every 250 ms for main screen
- i did not follow gpt's advice on buttons (i dont think its necessary tbf)

- ok i did some more searching up and it turns out that header files has a cpp file linked with it so thats like double the files
- also the biggest problem is the redeclaration of functions -> 15 errors on this
- cpp + header combo is best for long-term but more annoying what should i do hmmmm

# Day 6 - 10/26/25
- Holy crap that was an emotional roller coaster
- everything went ok with migration to platformio. i havent tested it yet but it seems reliable for now. also platform seems super cool and stuff
- and then everything went downhill after i decided to push it to git
- first of all why is it so complicated and second why did it delete like everything three times over
- i almost like just wanted to jump off smth after deleting all my work from the past two hours. thankfully gpt clutched up even though because i followed it originally that I almost deleted everything
- second why did i commit a deletion of my journal bro. and then i had to dig through the trenches of dangling blobs and trees commits to find this file. but ykw at least its back yay and the files r up. the only downside is the past commits is invisible but i dont think its that important. the content were more important to me.
- today was crazy i did not expect to be sleeping at like 3 oops

# Day 7 - 10/27/25
- I didn't have that much time to work on it but I checked out how to use the MPU5060
- it should be simple to use and implement
- but the thing that ain't simple is apparently for step counting i have to use the pedometer algorithm
- smth about accelerometer and how ur feet deccelerates or smth
- also github copilot is like a cheat this thing just automatically thinks wow -> should i turn off?
- at least today i didnt mess up on committing but for some reason the commit did not go through the code was added? I dont think im supposed to use ```git commit -m```

# Day 8 - 10/28/25
- did a little research on how heartrate sensors work and stuff
- ppg reflect green light to skin and when at peak less light at bottom more light
- spent 1.5 hours on soldering and ended up just buying flux T-T
- wrote the step counter code along with its daily reset -> EEPREM
- not bad recovery and also tried to imitate using green led and photoresistor but its not that accurate

# Day 9 - 10/29/25
- not much happen because I can't test the code
- I tried running platformio for nano but it kept saying com3 was blocked but com3 worked for arduino ide so idk what that means

# Day 10 - 10/30/25
- soldering session with le flux
- except I think I inhaled too much smoke and now my lungs kinda hurt maybe I need to touch grass for a bit
- i messed up one of the imu and one of the visible light and the rest seems ok... the only one that turned out super well was the L shaped connector IMU


