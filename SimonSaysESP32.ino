#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int notes[] = {262, 330, 392, 494};

int const pinLeds[] = {18, 19, 2, 23};
int const pinButtons[] = {13, 14, 32, 33};
int const pinNum = 4;

int const pinBuz = 4;
int const pinStart = 5;

const int MAX_LEVEL = 5;

int sequence[MAX_LEVEL];
int level = 0;

enum States {
  WAITING,
  PLAYING,
  WON,
  LOST
};

States state = WAITING;


//Prototypes des fonctions
void show_sequence();
void leds_off();
bool check_player();
void add_color();
void game_lost();
void game_won();


void setup() {
Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
      while (true) {
    }
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.display();

  for (int i = 0; i < pinNum; i++){
    pinMode(pinLeds[i], OUTPUT);
    pinMode(pinButtons[i], INPUT_PULLUP);
    digitalWrite(pinLeds[i], LOW);
  }
  pinMode(pinBuz, OUTPUT);
  pinMode(pinStart, INPUT_PULLUP);

  randomSeed(analogRead(25));

}

void loop() {

  

  if(state == WAITING) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.println("Press Red button");
    display.println("to start");
    display.display();

    if (digitalRead(pinStart) == LOW) {
      while (digitalRead(pinStart) == LOW) {}
        level = 0;
        add_color();
        state = PLAYING;
      }
    }
  
  else if(state == PLAYING) {

    display.clearDisplay();
    display.setCursor(0, 20);
    display.setTextSize(2);
    display.print("Level: ");
    display.println(level);
    display.display();

    show_sequence();
    bool correct = check_player();

    if(correct) {
      if(level == MAX_LEVEL) {
        state = WON;
      }
      else {
        add_color();
      }
    }
    else {
      state = LOST;
    }
  }

  else if(state == WON) {
    display.clearDisplay();
    display.setCursor(25, 20);
    display.setTextSize(3);
    display.println("WON!");
    display.display();

    game_won();

    state = WAITING;
  }

  else if(state == LOST) {
    display.clearDisplay();
    display.setCursor(15, 20);
    display.setTextSize(3);
    display.println("LOST");
    display.display();

    game_lost();
    state = WAITING;
  }


}

void show_sequence() {
  for (int i = 0; i < level; i++) {
    switch (sequence[i]) {
      case 0:
        tone(pinBuz, notes[0]);
        digitalWrite(pinLeds[0], HIGH);
        break;
      case 1:
        tone(pinBuz, notes[1]);
        digitalWrite(pinLeds[1], HIGH);
        break;
      case 2:
        tone(pinBuz, notes[2]);
        digitalWrite(pinLeds[2], HIGH);
        break;
      case 3:
        tone(pinBuz, notes[3]);
        digitalWrite(pinLeds[3], HIGH);
        break;
    }
    delay(500);
    noTone(pinBuz);
    leds_off();
    delay(200);
  }
}

void leds_off() {
  for(int i = 0; i < pinNum; i++) {
    digitalWrite(pinLeds[i], LOW);
  }
}

bool check_player() {
  for (int i = 0; i < level; i++) {
    int pressedButton = -1;
    while(pressedButton == -1) {
      for(int b=0; b < pinNum; b++) {
        if(digitalRead(pinButtons[b]) == LOW) {
          pressedButton = b;
        }
      }
    }

    tone(pinBuz, notes[pressedButton]);
    digitalWrite(pinLeds[pressedButton], HIGH);

    while (digitalRead(pinButtons[pressedButton]) == LOW) {}

    noTone(pinBuz);
    leds_off();
    delay(100);

    if (pressedButton != sequence[i]) {
      return false;
    }

  }

  return true;

}

void game_lost() {

  for (int i = 0; i < pinNum; i++) {
    digitalWrite(pinLeds[i], HIGH);
  }
  tone(pinBuz, 392);
  delay(250);
  leds_off();
  noTone(pinBuz);
  delay(100);

  for (int i = 0; i < pinNum; i++) {
    digitalWrite(pinLeds[i], HIGH);
  }
  tone(pinBuz, 330);
  delay(250);
  leds_off();
  noTone(pinBuz);
  delay(100);

  for (int i = 0; i < pinNum; i++) {
    digitalWrite(pinLeds[i], HIGH);
  }
  tone(pinBuz, 262);
  delay(500);
  leds_off();
  noTone(pinBuz);

  delay(2000);
}

void game_won() {

  for(int i = 0; i < 4; i++) {
    tone(pinBuz, notes[i]);
    for(int j=0; j < 4; j++) {
      digitalWrite(pinLeds[j], HIGH);
      delay(100);
      leds_off();
      delay(50);
    }
    noTone(pinBuz);
  }

  delay(2000);

}

void add_color() {
  sequence[level] = random(0, pinNum);
  level++;
}
