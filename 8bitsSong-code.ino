#define BUZZER_PIN 27
#define BUTTON_PIN 35
#define LED_PIN 23  
#define NUM_SPEEDS 5  // จำนวนระดับความเร็ว

#include <Arduino.h>

const int notes[] = {1047, 0, 1047, 0, 1568, 0 ,1568,0, 1760,0, 1760,0, 1568, 0, //C C G G A A G
                     1397, 0,1397, 0,1319, 0,1319, 0,1175, 0,1175, 0,1047,0, //F F E E D D C
                     1568, 0,1568, 0,1397, 0,1397, 0,1319, 0,1319, 0,1175,0, //G G F F E E D
                     1568, 0,1568, 0,1397, 0,1397, 0,1319, 0,1319, 0,1175,0}; //G G F F E E D

const int durations[] = {500, 100, 500, 100, 500, 100,500, 100,500, 100,500,100,1000,100,
                         500, 100,500,100, 500, 100,500, 100,500, 100,500, 100,1000,100,
                         500, 100,500,100, 500, 100,500,100, 500, 100,500, 100,1000,100,
                         500, 100,500, 100,500, 100,500, 100,500, 100,500, 100,1000,100};

const int speeds[NUM_SPEEDS] = {1100, 900, 700, 500, 300};
volatile int currentSpeedIndex = 0;
volatile bool buttonPressed = false;
volatile int noteIndex = 0;

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR handleButtonPress() {
  buttonPressed = true;
}

void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  
  if (notes[noteIndex] == 0) {
    noTone(BUZZER_PIN);
  } else {
    tone(BUZZER_PIN, notes[noteIndex]);
  }

  int nextDuration = durations[noteIndex] * speeds[currentSpeedIndex] / 1000;
  timerAlarm(timer, nextDuration * 1000, true, 0);
  
  noteIndex++;
  if (noteIndex >= sizeof(notes) / sizeof(notes[0])) {
    noteIndex = 0; 
  }

  portEXIT_CRITICAL_ISR(&timerMux);
}

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); 
  pinMode(LED_PIN, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonPress, FALLING);

  timer = timerBegin(1520000);
  timerAttachInterrupt(timer, &onTimer);
  timerAlarm(timer, 15200000, true, 0);
  //timerAlarmEnable(timer);
}

void loop() {
  if (buttonPressed) {
    buttonPressed = false;
    currentSpeedIndex = (currentSpeedIndex + 1) % NUM_SPEEDS;
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  }
}
