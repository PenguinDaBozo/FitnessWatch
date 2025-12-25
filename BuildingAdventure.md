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

# Day 11 - 10/31/25
- I fixed the code sort of?
- I also got the esp32 running on platform io. I dont remember how I fixed the com5 blocking -> i just closed a bunch of things and removed arduino analog and it just started working with occasional boot restarts
- right now the code bugs a lot and it shows stuff like this

<img width="500" height="500" alt="image" src="https://github.com/user-attachments/assets/f263fe5f-a717-4812-aaed-484f892b91f9" />

<img width="583" height="462" alt="image" src="https://github.com/user-attachments/assets/202ae8d7-709a-4f47-96e6-6ef2fa973a8d" />

and also the 10:01 and 10:00 bug gee

- Im also considering if I should combine RTC and NTP or just switch to NTP bec of the cell battery factor -> too much powering
- also I need to figure out how to correctly draw and update ex. uncanny eyes might work
- how tf do modern devices refresh and update their clocks????/

# Day 12 - 11/1/25
- OKKK, I THINK I FIGURED OUT A SOLUTION TO THE FLICKERS
- https://github.com/Lemme-lab/Project-Airframe/blob/main/Code/Esp32-Embedded/src/display.cpp
- THIS LINK WILL SOLVE EVERYTHING -> I NEED TO USE SPRITES
- which means I have to change the code base again xP
- i wasnt supposed to do much today but I ended up designing the main screen sprite
- apparently its only a reference but the background stays relatively the same or something?????

# Day 13 - 11/2/25
- designed the mainface and stuff but idk how to diosplay it on the display
- also this eSPI thing is tweaking breh cuz how is it that i cant even display now
- wtf is this black to white fade.

# Day 14 - 11/3/25
- HOLY CRAP I FINALLY FIGURED OUT THE STUPID TFT_ESPI THING -> its bec i didnt read the instructions inside the setup file and also the fact that its tft.init() and not begin()
- and the fact that I had different definitions than wiring was a bit dumb of me ngl good thing chat noticed
- yo how did that take like 6 hours to figure out T-T
- ok I shifted some code over and it still works yayy!!
- but theres a fluctuation in the screen when you tilt it -> maybe decouple caps?
- designed some noice images for weather and also separated the image into its parts
- tmr i shall get the numbers hooray!

# Day 15 - 11/4/25
- yahoo! i separated the stuff but in the middle of it i realized i had the wrong canvas size originally so spent like an hr half fixing it
- the font stuff was kind of dumb because i spent half the time trying to find a converter well at least tft_espi got one
- it looks cool

# Day 16 - 11/5/25
- oh no......
 <img width="500" height="500" alt="image" src="https://github.com/user-attachments/assets/83317300-bf01-4b0b-88d4-5eaca829b1f2" />

- 4 channel colors 💀 bec i dont have enough sram
- this is so sad cuz this board has no memory i guess function > aesthetics

# Day 17 - 11/7/25
- ok now the clock is working yippee but i did have to remove all the colors to save memory but it still has the good contrast
- also baloo is kind of pmo bec the porportions of the numbers keep changing so this stupid 44 would be so fat it almost crashes into the icons
- now i have to move onto steps
- what should i do for the next screen?

<img width="500" height="500" alt="image" src="https://github.com/user-attachments/assets/760c973d-2538-4666-93af-ff36e5510bb6" />

this looks so sick except why when I go to take pic the flash is so bright


# Day 18 - 11/8/25
- uh nothing rlly happened tbf
- i moved the step tracker to the main screen so most of the code is there but the rtc didn't work and was displaying wild numbers

# Day 19 - 11/9/25
- i found out that my rtc is stuck at 5v which is prob not good and i dont want to rip off the resistors on the board
- also mpu6050 is being buggy prob because of the former

# Day 20 - 11/10/25
- tested some stuff with the thing and still doesn't work
- surprisingly mpu6050 works on arduino so i think its the board itself that's dragging i2c to 5v
- also i swtiched rtc and it didnt work and removing that block of code is the only thing that keeps it going, which is weird so something in that code might be breaking it or maybe bec of the begin factor.

# Day 21 - 11/13/25
- rotary encoder doesnt even work :sob:
- im switching to buttons possibly
- also the reason why the thing didnt work is cuz the ds1307 runs on 5v and the mpu6050 on 3.3v and so i need an i2c shifter, which means more soldering..... :sob: 

# Day 22 - 11/15/25
- I soldered some i2c shifters and broke my soldeirn ggun 

# Day 23 - 11/16/25
- damn bro these stupid ahh i2c shifters dont even do anything bec both of them r powered by 5v 😭

# Day 24 - 11/17/25

<img width="532" height="351" alt="image" src="https://github.com/user-attachments/assets/878d9be3-493b-40f8-a21e-038b6671767c" />

look at this what am i supposed to do bruh

<img width="1006" height="401" alt="image" src="https://github.com/user-attachments/assets/5e0ce56b-3e15-4aac-a35d-8373bc3fe24c" />

ok i think i found a solution i have to use a different library that actually takes 0x69 as the input aka electro cat or smth -> now i have to rewrite code i pray this works

<img width="794" height="311" alt="image" src="https://github.com/user-attachments/assets/967c65e6-cc6f-4f59-bc1e-37e860a31511" />

ok well it still doesn't work but at least rtc works now i guess? but this might be bec the mpu's pins are not soldered well

<img width="877" height="982" alt="image" src="https://github.com/user-attachments/assets/44cc9d29-ad13-4b03-807e-78e51f4d0556" />

hip hip hooray thats another feature done that really shouldnt have taken a week and a half to solve ngl but yayyyyy. also um the problem is that the first read if always going to be the default so idk how to fix that and also the fact that stupid day light savings bruh

<img width="1312" height="1469" alt="image" src="https://github.com/user-attachments/assets/5e920591-d540-455b-adcb-687c2001a3a6" />

also this cuz why is the step increasing by itself :sob: omg so many issues after issues. also steps doesnt save to eeprom so i think the video i watched may be invalid

<img width="812" height="566" alt="image" src="https://github.com/user-attachments/assets/2ae41c6a-c5de-4a4f-b0dc-b04fd2f86500" />

ok nvm the video was valid and the calculation was right its just because im silly and i put the thing in the else statment which makes prevMag forver that value. now i need to fix the screen display through the first run and the daylight saving thing

<img width="885" height="1081" alt="image" src="https://github.com/user-attachments/assets/051a1bf4-4c07-4975-9456-4e90ac67ede9" />

<img width="891" height="1138" alt="image" src="https://github.com/user-attachments/assets/62cc0ef5-6963-42df-960e-65e27efa572b" />

next i need to fix the day thing and think of features to add. i think box breathing is next or maybe i should do a compass except i think that sounds too hard so box breathing it is and if the stupid stm32 etg sensor breaks everything fml

wait actually no i ahve to do the weather which sounds simple.... but is it tho?

# Day 25 - 11/18/25

I fixed the hour, dayofweek, and date great job me. wifi connection is not as simple as it seems bec why is everything deprecated. not me reading the whole docuumentation for openweatherapi and also arduinojson :sob: bro why is there no weather example for version 7.0

<img width="2772" height="1333" alt="image" src="https://github.com/user-attachments/assets/b5b8812b-6c13-42f9-a3ec-3b0363b44cf1" />

ummm memory overload prob on ram oopsies daisies anyway 

<img width="879" height="836" alt="image" src="https://github.com/user-attachments/assets/95dd2a7d-7fbc-44ec-b677-c8f961695961" />


so i tried to change version and then i glitched pio again but apparently you can just do this

<img width="1218" height="104" alt="image" src="https://github.com/user-attachments/assets/96309ac2-531f-4541-b9d5-4cb0c4c599c7" />

[credits: https://community.platformio.org/t/library-registry-semantic-version-error/44723] 

# Day 26 - 11/19/25

I fixed the glitching of the display bec of memory by rverting to 6.1.15 for arduino json worked very well
i spent 1 hour and half literally designing the weather icons kms why was it so hard

# Day 27 - 11/20/25

I converted all those pngs to cpp and now I have to adjust the box for the frame and test different weathers

<img width="679" height="729" alt="image" src="https://github.com/user-attachments/assets/82945a1b-78fc-481d-bc41-176bfb8d54c6" />

Hehe... I'm in Japan right now. ok so all of it works yippe the only thing left for this page is to fix the formating of the numbers and letters but thats a later problem

# Day 28 - 11/21/25

I worked a little on the changing page mechanism also u cant use /blah blah for folder embedding structures :C

# Day 29 - 11/22/25

making the screen and stuff and then my stomach and head hurt so i went to zzz

# Day 30 - 11/23/25

designed the box breathing screen and put the code down except font didnt work but then worked apparently only had to call once in any function

anyway heres the pic:<img width="1720" height="969" alt="image" src="https://github.com/user-attachments/assets/3a189c32-632b-417f-8078-e4d8291b9a6c" />

dont separate a font file into cpp and h

also im trying to fix the switch button things and fotns and the start button is not swtiching to the new screen

<img width="876" height="891" alt="image" src="https://github.com/user-attachments/assets/94bce6ed-f06b-4efd-9796-7736c4f80d09" />

weird glitchingg that happens might be bec of button

anyway i forgot to call update in main earlier which is why it didnt update (silly me)

<img width="878" height="850" alt="image" src="https://github.com/user-attachments/assets/8cb93507-b2f4-45ae-b855-fa2e065d5c5d" />

<img width="820" height="859" alt="image" src="https://github.com/user-attachments/assets/f3a43e25-b6a7-40c5-8b43-fef09d9d53c0" />

very sick! i was testing if i should use two buttons to change states or juddt one and rn its one but i have to addd thhe flags to make sure ot doesnt hot switch

# Day 31 - 11/24/25

added buzzer sound to a countdown for the breathingbox sim and also dixed the thing with mad flags and fixing some bugs 

tried to use the max30102 but its so funky bec sparkfun algo for heartrate doesnt work very well imo but the ir works 

also i need to work faster so i should skip doing sensors and big stuff and do it all later to save time since interface still takes a lot of time

# Day 32 - 11/25/25

I have not update github code for like ages i need to clean up this repo

also i made the stopwatch (it was really easy to code apparently) added some misty weather icons um idk what else i did but the stop watch is very sigma

<img width="894" height="967" alt="image" src="https://github.com/user-attachments/assets/bcd4c449-533c-4ca0-aeee-d84de13cdc77" />

this is so sigma

# Day 33 - 11/26/25

i fixed the stopwatch... maybe consider having it extend more than an hour by changing the size?

# Day 34 - 11/27/25

Goddamn how did i spend so much time on the stupid animation and it still doesnt work :sob:

anyways I think i started making the pcb (i forogt to record here so i have no clue when i started the pcb)

# Day 35 - 11/28/25

added the battery circuit and worked on the mpu5060

# day 36 - 11/29/25

decided to delete the regulators and stuff and holy freaking gosh bro i have so much tabs open just bec im looking at datasheets 

# day 37 - 11/30/25 

I started designing the PCB and oh god it was horrible -> switched to the s3 because I realized the pico d4 chip is dying 

<img width="452" height="503" alt="image" src="https://github.com/user-attachments/assets/c3aab669-b959-4df6-9f3d-83afd6dbec09" />

what le hell is dis

well I can't recover the original so this is the best image representation

# day 38 - 12/1/25

I continue placing stuff and stuff. 4 hours later...

# day 40 - 12/2/25

I restarted a new board because apparently you can't have buttons at an angle and restarting was easier. Placing components til god know when.

# day 41 - 12/3/25

Placing.... placing... -> tracing then realizing there's a lot of rules I broke...

# day 42 - 12/4/25

I think I finished here and then realized that it wasn't good enough...

<img width="409" height="368" alt="image" src="https://github.com/user-attachments/assets/3f5a2652-7293-4d96-a279-f579e90a3517" />


** note i mightve not updated this in a while and am updating on 12/21/25
# day 43 - 12/5/25

restarted for the fourth time and migrated to pro version of easyeda because lowkey I shouldve done so from the start. then i didn't like the placement so I remade another design... oh god.

# day 44 - 12/6/25

placing components later... I start routing and I'm like following GPT's guide of track width 

# day 45 - 12/7/25

at this point bro just stop.

I watched some videos - Thanks to Altium academy and the other thing I don't remmeber watching. Applied those over the next couple of days. anyway I start checking the modules, starting from the sensors, checking every datashee in the world and making sure the capacitor and ended up pulling an all nighter because  I realize that the placement of things aint correct sooo.... fixing the wiring size and stuff and also the placement of things more to the left because I created these paper models and realized I had the wiring wrong for the tft display. Great idea for simulation me btw. 

<img width="561" height="548" alt="image" src="https://github.com/user-attachments/assets/76dbcb15-ff6d-4331-a32b-5304a5594bc0" />

reference for original wiring

<img width="583" height="516" alt="image" src="https://github.com/user-attachments/assets/2967aeb7-6f4c-4237-a679-1d4e2888a417" />

# day 46 - 12/8/25


<img width="466" height="374" alt="image" src="https://github.com/user-attachments/assets/45b21d61-12b5-4725-8084-adb8afb037cd" />

checking and adjusting capacitors. moving the router and crystal and shfiting the heart rate ssensor waas really painful but its ok I liekd it.

# day 47 - 12/9/25 

checking and adjusting capacitors. 

# day 48 - 12/10/25

checking and adjusting capacitors. I think i moved everything to the left because I was changing the wing size and fixed the button wiring. 

# day 49 - 12/11/25

checking and adjusting capacitors. 

# day 50 - 12/12/25

final touches on the wiring width and especially the ground planes. added a top and bottom plane for some reason just bec i thought it was cool perhaps this might eat me in thebutt later.

<img width="849" height="566" alt="image" src="https://github.com/user-attachments/assets/67c1234d-19f4-4462-ae1e-d0c6e6891c5a" />


# day 51 - 12/13/25

I finish checking the PCB yay, but I kind of forgot to check the differential pairs for the USB... oops but I don't think it will have that big of an impact. Had to fix the antenna rlly quick bec of impedence control.

<img width="656" height="519" alt="image" src="https://github.com/user-attachments/assets/a1a5307b-0280-48cd-aa78-d20fb3cc0e7f" />

<img width="491" height="362" alt="image" src="https://github.com/user-attachments/assets/f350602a-4244-4886-a9bd-73d1884ac2e9" />


# day 52 - 12/14/25

Turns out I have some vias that are too small and stuff so I canceled the order and reordered with bigger vias to avoid the 40 dollar fee and then it turns out that there were still some vias I didn't get rid of so I paid that 40 dollars :sob:

I start mmeasuring and stuff of the TFT display and create a base circle. Didn't fit the first time so I adjusted it for tolerance. Then I added some snazzy deatils and print it out:

<img width="460" height="766" alt="image" src="https://github.com/user-attachments/assets/f0e8c076-4823-4dd2-a61a-493e3aee59a3" />

You can tell I definitely took a lot of inspiraiton from casio watches. I mean I did have one when I was younger 😋 last one is broken because i was testing durability. 

# day 53 - 12/15/25

Issues with the board because I didn't standardize the BOM because I didn't know I had to :C. Asking Frank from JCLPCB to fix some stuff. Then who is Paul?? Anyway told me he can't modify 

I create the PCB case and add the four corner hooks. Turns out I did not learn my lesson about tolerance so I printed one where the hooks didn't snap with the cap. But after some adjusting it worked.

<img width="571" height="771" alt="image" src="https://github.com/user-attachments/assets/2bdab382-0bfb-4123-80e2-8139052daaba" />


# day 54 - 12/16/25

I respond back to Paul saying I want to replace components on the board. But he wasn't responding so I asked frank again and he responds and gives me permissioin yayy! I replace the Capacitors with Yageo 603 -> i shoudlve done this in the first place :sob:

Now I started on the straps, which were pretty simple enough and designed a mechanism for the watch straps to attach.

<img width="625" height="185" alt="image" src="https://github.com/user-attachments/assets/bf8e27bf-1081-4f21-b8c0-286a5104ae4f" />

<img width="744" height="650" alt="image" src="https://github.com/user-attachments/assets/0c9577ca-5fdc-4ff0-a051-f4243490a813" />

# day 55 - 12/17/25

Then they had to confirm the speaker stuff which had some trouble because the website was gltiching and the DFM was stuck on modification. To which I start asking Nick about whats happening and stuff and some guy with the S name helps me enable the confirmation and then I talk to Nick again...

I making the curviest of the cruvisest buckle. Hooly cow how do u do cruve stuff on onShape. After a few dimensions I finish and it looks ok?

<img width="581" height="778" alt="image" src="https://github.com/user-attachments/assets/bdee2100-5336-4c1f-a591-9b8405acd050" />

# day 56 - 12/18/25

Now I making the hook and chamfer some edges so its curvy.

they tell me that production starts at 8 and im like finally yay.

# day 57 - 12/19/25

pcb in the making and lowkey it was pretty fast. 

started printing the case stuff out and mom accidentally snapped off two of the hooks of the pcb case... oops but this told me to improve the hook to be thicker 

<img width="459" height="238" alt="image" src="https://github.com/user-attachments/assets/d9d14445-d744-4159-9909-6f07c94f4d74" />


<img width="548" height="703" alt="image" src="https://github.com/user-attachments/assets/b57ecb67-4acc-463c-918b-0bf60b667aa9" />


# day 58 - 12/20/25

waiting for watch wrist bands to print the entire day:

some pics also dont print vertically because lots of stringing. 

also changed the height like 50 times -> started with 3 then 5 then 4 then switched to 3. for some reason there was a splotch so I reprint the next day. 

<img width="1011" height="516" alt="image" src="https://github.com/user-attachments/assets/601f2b6a-f8b0-4b31-af92-505b032fd383" />

epic watch spread with the right being the first version


# day 59 - 12/21/25

It might have been a while since I updated this, but I've done a lot since then aka making the watch case and stuff. 3 -> 2 mm and used rectilinear for that nice grid texture

changed the hooks a bit because of tolerance

<img width="692" height="728" alt="image" src="https://github.com/user-attachments/assets/1d52106f-e4bd-4af4-958a-455f0bcb4818" />


Here are the final results: 

<img width="945" height="723" alt="image" src="https://github.com/user-attachments/assets/633e0d53-d0a7-4894-abf9-47d0c35ca9bd" />


I think they look pretty sick. need to ensure the setting for layer height is 0.1 so everything is very good quality. 

I might need to adjust the wristband because it slides out too easily.

pcb is ready to ship hip hip hooray

# day 60 - 12/22/25

i tried wiring the new esp32 baord with tft display on arduino ide which worked. idk if i hallucinated but i thought 5v of the board did work

# day 61 - 12/23/25

tried uploading code via platformio to the new board but erm problems again. one apparently theres a memory issue perhaps because of the specs of the baord and two apparently the 5v pin of the baord doesnt give out 5v. 

how do i know? i dont because the probing of the pins were very interesting kept showing 3 on the 5v. but then my genius ass came up with the solution of using a power module on the breadboard YAY. taht worked and the mpu6050 and rtc works.

at fiirst i thought oh this was just another dumb address collision but apparently it was saying smth else -> aka the memory bruh.

# day 62 - 12/24/25

PCB has arrived HOORAY! JCLPCB came in clutch except two problems:

1. I can't believe I didn't tie EN of buck step down regulator to HIGH :sob: this is why u read the datasheet

<img width="1199" height="137" alt="image" src="https://github.com/user-attachments/assets/663278f2-bb74-46c3-b826-0d052a758cad" />
  
2. idk if its cuz the s3 had no power or if you have to have usb to uart converter (anyway im pretty sure it wouldve been fine if i had one on the board anyway) but i cant upload code rn

