#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// =====================================================
// EXAMWATCH FINAL ADVANCED VERSION
// Adaptive Whisper Detection + Sensor Fusion
// =====================================================

// ---------------- LCD ----------------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------------- PIN DEFINITIONS ----------------
const int pirPin = 2;
const int soundPin = A0;
const int buzzerPin = 8;
const int ledPin = 9;
const int buttonPin = 4;

// ---------------- SYSTEM VARIABLES ----------------

// Suspicion scoring
int suspicionScore = 0;
int eventCount = 0;

const int ALERT_THRESHOLD = 20;

// Silent mode
bool silentMode = false;

// Motion state
bool motionDetected = false;
unsigned long motionTime = 0;
bool pirWasHigh = false;
unsigned long lastRestingPrint = 0;

// Timing
unsigned long lastDecayTime = 0;
unsigned long lastLCDUpdate = 0;
unsigned long lastBurstTime = 0;

// ---------------- ADAPTIVE AUDIO ANALYSIS ----------------

// Adaptive baseline
float baseline = 400;

// Variance calculation
const int SAMPLE_COUNT = 40;
int samples[SAMPLE_COUNT];

// Detection tuning
const float BASELINE_ALPHA = 0.01;   // baseline adaptation speed
const float VARIANCE_THRESHOLD = 10; // whisper sensitivity
const int BURST_REQUIRED = 3;
int consecutiveWhispers = 0;

// Burst system
int burstCount = 0;

// Timing constants
const unsigned long BURST_WINDOW = 2000;
const unsigned long DECAY_INTERVAL = 3000;
const unsigned long LCD_INTERVAL = 500;
unsigned long lastSerialPrint = 0;

// Score weights
const int SCORE_SOUND = 4;
const float SCORE_MOTION = 0.05;
const int SCORE_COMBINED = 8;

// =====================================================
// SETUP
// =====================================================

void setup() {

  pinMode(pirPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("   ExamWatch");
  lcd.setCursor(0, 1);
  lcd.print("  Initializing");
  delay(2000);

  lcd.clear();
}

// =====================================================
// MAIN LOOP
// =====================================================

void loop() {

  handleButton();

  bool whisperDetected = analyzeSound();

  handleMotion();

  // -------------------------------------------------
  // SENSOR FUSION
  // -------------------------------------------------

  if (whisperDetected) {

    suspicionScore += SCORE_SOUND;

    consecutiveWhispers++;

    Serial.print("Whisper Count: ");
    Serial.println(consecutiveWhispers);

    Serial.println("Whisper Pattern Detected");

    // -------------------------------------------------
    // MOTION + WHISPER
    // -------------------------------------------------

    if (motionDetected) {

      suspicionScore += SCORE_COMBINED;

      Serial.println("Combined Suspicious Activity Detected");

      motionDetected = false;
    }

    // -------------------------------------------------
    // WHISPER ONLY TRIGGER
    // -------------------------------------------------

    if (consecutiveWhispers >= 5) {

      Serial.println("5 CONSECUTIVE WHISPERS DETECTED");

      triggerAlert();

      suspicionScore = 0;

      eventCount++;

      consecutiveWhispers = 0;
    }
}

  // -------------------------------------------------
  // SCORE DECAY
  // -------------------------------------------------

  if (millis() - lastDecayTime > DECAY_INTERVAL) {

    if (suspicionScore > 0) {
      suspicionScore--;
    }

    lastDecayTime = millis();
  }

  // -------------------------------------------------
  // LCD UPDATE
  // -------------------------------------------------

  if (millis() - lastLCDUpdate > LCD_INTERVAL) {

    updateLCD();

    lastLCDUpdate = millis();
  }

  // -------------------------------------------------
  // ALERT CHECK
  // -------------------------------------------------

  if (suspicionScore >= ALERT_THRESHOLD) {

    triggerAlert();

    suspicionScore = 0;

    eventCount++;
  }

  // Reset whisper streak if no burst occurs for long time

if (millis() - lastBurstTime > 7000) {

    consecutiveWhispers = 0;
}

}

// =====================================================
// BUTTON HANDLER
// =====================================================

void handleButton() {

  if (digitalRead(buttonPin) == LOW) {

    silentMode = !silentMode;

    delay(300);
  }
}

// =====================================================
// MOTION DETECTION
// =====================================================

void handleMotion() {

  int pirState = digitalRead(pirPin);

  if (pirState == HIGH) {

    motionDetected = true;

    motionTime = millis();

    suspicionScore += SCORE_MOTION;

    Serial.println("Motion Detected");
  }

  // Motion timeout
  if (millis() - motionTime > 5000) {

    motionDetected = false;
  }

  // ── Resting: only prints if sensor just came down from HIGH ──
    if (pirState == LOW && pirWasHigh) {
        if (millis() - lastRestingPrint > 2000) {
            Serial.println("Sensor Resting");
            lastRestingPrint = millis();
        }

        // stop resting message once cooldown is clearly over
        if (millis() - motionTime > 10000) {
            pirWasHigh = false;
        }
    }
}

// =====================================================
// ADVANCED SOUND ANALYSIS
// =====================================================

bool analyzeSound() {

  // Reset stale bursts FIRST
  if (millis() - lastBurstTime > BURST_WINDOW) {
    burstCount = 0;
  }
  // ... rest of the sampling code

  long sum = 0;

  // -------------------------------------------------
  // SAMPLE COLLECTION
  // -------------------------------------------------

  for (int i = 0; i < SAMPLE_COUNT; i++) {

    samples[i] = analogRead(soundPin);

    sum += samples[i];

    delay(2);
  }

  // -------------------------------------------------
  // MOVING AVERAGE
  // -------------------------------------------------

  float average = (float)sum / SAMPLE_COUNT;

  // -------------------------------------------------
  // ADAPTIVE BASELINE
  // Slowly follows room noise
  // -------------------------------------------------

  baseline =
    (BASELINE_ALPHA * average) +
    ((1.0 - BASELINE_ALPHA) * baseline);

  // -------------------------------------------------
  // VARIANCE / FLUCTUATION ANALYSIS
  // -------------------------------------------------

  float variance = 0;

  for (int i = 0; i < SAMPLE_COUNT; i++) {

    variance += sq(samples[i] - average);
  }

  variance /= SAMPLE_COUNT;

  float stdDeviation = sqrt(variance);

  // -------------------------------------------------
  // DEBUG OUTPUT
  // -------------------------------------------------

  // -------------------------------------------------
// SERIAL OUTPUT EVERY 15 SECONDS
// -------------------------------------------------

if (millis() - lastSerialPrint > 15000) {

  Serial.print("Average: ");
  Serial.print(average);

  Serial.print(" | Baseline: ");
  Serial.print(baseline);

  Serial.print(" | StdDev: ");
  Serial.println(stdDeviation);

  lastSerialPrint = millis();
}

  // -------------------------------------------------
  // WHISPER DETECTION
  // -------------------------------------------------

  // Small fluctuations above ambient noise
  if (
    average > baseline + 5 &&
    stdDeviation > VARIANCE_THRESHOLD &&
    stdDeviation < 120
  ) {

    burstCount++;

    lastBurstTime = millis();

    Serial.println("Audio Burst");
  }

  // -------------------------------------------------
  // BURST VALIDATION
  // -------------------------------------------------

  if (burstCount >= BURST_REQUIRED) {

    burstCount = 0;

    Serial.println("SOUND DETECTED");

    return true;
}

 /* // Reset old bursts
  if (millis() - lastBurstTime > BURST_WINDOW) {

    burstCount = 0;
  }*/

  return false;
}

// =====================================================
// LCD UPDATE
// =====================================================

void updateLCD() {

  lcd.setCursor(0, 0);

  lcd.print("S:");
  lcd.print(suspicionScore);
  lcd.print(" ");

  // Confidence levels
  if (suspicionScore <= 6) {

    lcd.print("LOW ");
  }
  else if (suspicionScore <= 14) {

    lcd.print("MED ");
  }
  else {

    lcd.print("HIGH");
  }

  lcd.print(" ");

  // -------------------------------------------------

  lcd.setCursor(0, 1);

  lcd.print("E:");
  lcd.print(eventCount);

  lcd.print(" ");

  if (silentMode) {

    lcd.print("SILENT ");
  }
  else {

    lcd.print("BUZZ   ");
  }
}

// =====================================================
// ALERT FUNCTION
// =====================================================

void triggerAlert() {

  Serial.println("ALERT TRIGGERED");

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("   !! ALERT !!");

  lcd.setCursor(0, 1);
  lcd.print(" !!Suspicious!!");

  // Buzzer
  if (!silentMode) {

    digitalWrite(buzzerPin, HIGH);
  }

  // LED blinking
  for (int i = 0; i < 10; i++) {

    digitalWrite(ledPin, HIGH);

    delay(200);

    digitalWrite(ledPin, LOW);

    delay(200);
  }

  digitalWrite(buzzerPin, LOW);

  lcd.clear();
}