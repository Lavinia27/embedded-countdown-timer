//  Configurare afișaj 4x7seg 
const int segPins[7] = {2, 3, 4, 5, 6, 7, 8};  // Segmente A-G
const int dotPin = 13;                         // Punct zecimal
const int digitPins[4] = {9, 10, 11, 12};      // Cifrele DIG1..DIG4

// Harta segmentelor (catod comun: HIGH=aprins)
const byte digitMap[10][7] = {
  {1,1,1,1,1,1,0}, // 0
  {0,1,1,0,0,0,0}, // 1
  {1,1,0,1,1,0,1}, // 2
  {1,1,1,1,0,0,1}, // 3
  {0,1,1,0,0,1,1}, // 4
  {1,0,1,1,0,1,1}, // 5
  {1,0,1,1,1,1,1}, // 6
  {1,1,1,0,0,0,0}, // 7
  {1,1,1,1,1,1,1}, // 8
  {1,1,1,1,0,1,1}  // 9
};

//  Butoane + buzzer 
#define B1_PIN A0
#define B2_PIN A1
#define POT1_PIN A3
#define POT2_PIN A4
#define BUZZER_PIN A2

//  Stări sistem 
enum State { IDLE, CONFIG_MIN, CONFIG_SEC, COUNTDOWN };
State currentState = IDLE;

//  Variabile timp 
int minutes = 0;
int seconds = 0;
int digitsToShow[4] = {0, 0, 0, 0};

unsigned long lastTick = 0;
unsigned long lastDebounce = 0;
bool lastButtonState = HIGH;
bool buttonPressed = false;

bool lastB2 = HIGH;
unsigned long lastDebounceB2 = 0;
unsigned long buzzerStart = 0;
bool buzzerActive = false;

// SETUP
void setup() {
  // Pini afișaj
  for (int i = 0; i < 7; i++) pinMode(segPins[i], OUTPUT);
  pinMode(dotPin, OUTPUT);
  for (int i = 0; i < 4; i++) {
    pinMode(digitPins[i], OUTPUT);
    digitalWrite(digitPins[i], HIGH);
  }

  pinMode(B1_PIN, INPUT_PULLUP);
  pinMode(B2_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  Serial.begin(9600);

  updateDigitsFromTime(minutes, seconds);
}

// LOOP principal
void loop() {
  handleButton();

  switch (currentState) {
    case IDLE:
      if (buttonPressed) {
        buttonPressed = false;
        currentState = CONFIG_MIN;
        Serial.println(">> Intrare configurare MINUTE");
      }
      break;

    case CONFIG_MIN:
      minutes = readDigitFromPotentiometers();
      updateDigitsFromTime(minutes, seconds);
      if (buttonPressed) {
        buttonPressed = false;
        currentState = CONFIG_SEC;
        Serial.println(">> Intrare configurare SECUNDE");
      }
      break;

    case CONFIG_SEC:
      seconds = readDigitFromPotentiometers();
      updateDigitsFromTime(minutes, seconds);
      if (buttonPressed) {
        buttonPressed = false;
        if (minutes == 0 && seconds == 0) {
          Serial.println("!! Timp invalid, reia configurarea...");
          currentState = CONFIG_MIN;
        } else {
          currentState = COUNTDOWN;
          Serial.println(">> Cronometru pornit!");
        }
      }
      break;

    case COUNTDOWN:
      if (!buzzerActive && millis() - lastTick >= 1000) {
        lastTick = millis();
        if (minutes > 0 || seconds > 0) {
          tickTime();
          updateDigitsFromTime(minutes, seconds);
        } else {
          Serial.println(">> Cronometru terminat!");
          Serial.println(">> Buzzer 30 sec.");
          tone(BUZZER_PIN, 2000); // sunet clar, PWM cu FU 50%
          buzzerStart = millis();
          buzzerActive = true;
        }
      }

      if (buzzerActive && millis() - buzzerStart >= 30000) { // 30 secunde
        noTone(BUZZER_PIN);
        buzzerActive = false;
        currentState = IDLE;
        Serial.println(">> Buzzer oprit. Revenire în IDLE.");
      }

      // Buton B2 → +30s
      bool curB2 = digitalRead(B2_PIN);
      if (curB2 == LOW && lastB2 == HIGH && millis() - lastDebounceB2 > 200) {
        lastDebounceB2 = millis();
        add30Seconds();
        Serial.println("Buton B2 apăsat -> +30s");
      }
      lastB2 = curB2;
      break;
  }

  // Multiplexare afișaj
  for (int i = 0; i < 4; i++) {
    showDigit(i, digitsToShow[i], true);
  }
}

// FUNCȚII

void showDigit(int index, int digit, bool dotOn) {
  for (int i = 0; i < 4; i++) digitalWrite(digitPins[i], HIGH);
  for (int s = 0; s < 7; s++) digitalWrite(segPins[s], digitMap[digit][s]);
  if (index == 1 && dotOn) digitalWrite(dotPin, HIGH);
  else digitalWrite(dotPin, LOW);

  digitalWrite(digitPins[index], LOW);
  delay(2);
  digitalWrite(digitPins[index], HIGH);
  digitalWrite(dotPin, LOW);
}

void updateDigitsFromTime(int min, int sec) {
  digitsToShow[0] = min / 10;
  digitsToShow[1] = min % 10;
  digitsToShow[2] = sec / 10;
  digitsToShow[3] = sec % 10;
}

void handleButton() {
  bool currentStateBtn = digitalRead(B1_PIN);
  if (currentStateBtn == LOW && lastButtonState == HIGH && millis() - lastDebounce > 200) {
    buttonPressed = true;
    lastDebounce = millis();
  }
  lastButtonState = currentStateBtn;
}

int readDigitFromPotentiometers() {
  int potZeci = analogRead(POT1_PIN);
  int potUnit = analogRead(POT2_PIN);

  int zeci = map(potZeci, 0, 1023, 0, 5); // max 5 zeci
  int unit = map(potUnit, 0, 1023, 0, 9);

  return zeci * 10 + unit;
}

void tickTime() {
  if (seconds > 0) {
    seconds--;
  } else if (minutes > 0) {
    minutes--;
    seconds = 59;
  }
}

void add30Seconds() {
  int total = minutes * 60 + seconds + 30;
  if (total > 3599) total = 3599; // maxim 59:59
  minutes = total / 60;
  seconds = total % 60;
  updateDigitsFromTime(minutes, seconds);
}
