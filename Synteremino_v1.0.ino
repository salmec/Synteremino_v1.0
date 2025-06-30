// SYNTEREMINO v1.0 Arduino-based granular synthesizer with ultrasonic distance control
// This code is released under GNU Lesser General Public License https://www.gnu.org/licenses/lgpl-3.0.html
// The code is by Salvatore Mecca
// Waveform management is taken from
// Auduino, the Lo-Fi granular synthesiser
//
// by Peter Knight,
//
// Help:      http://code.google.com/p/tinkerit/wiki/Auduino
// More help: http://groups.google.com/group/auduino
//
// Analog in 1: Grain 2 decay
// Analog in 2: Grain 1 decay
// Analog in 3: Grain 2 pitch
// Analog in 4: Grain repetition frequency

#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <LedControl.h>
#include <LiquidCrystal_I2C.h>
#include <SR04.h>

// LED Matrix configuration
#define DIN 10
#define CS 9
#define CLK 8
LedControl lc = LedControl(DIN, CLK, CS, 0);

// LCD configuration
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Note definitions
const int notes[] = { 
  31, 33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 58, 62, 65, 69, 73, 78, 82, 87, 93, 98, 104, 110, 117, 123, 131, 
  139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247, 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 
  494, 523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988, 1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 
  1661, 1760, 1865, 1976, 2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951, 4186, 4435, 4699, 4978 
};

const char* notes_name[] = { 
  "B0", "C1", "CS1", "D1", "DS1", "E1", "F1", "FS1", "G1", "GS1", "A1", "AS1", "B1", "C2", "CS2", "D2", "DS2", "E2", 
  "F2", "FS2", "G2", "GS2", "A2", "AS2", "B2", "C3", "CS3", "D3", "DS3", "E3", "F3", "FS3", "G3", "GS3", "A3", "AS3", 
  "B3", "C4", "CS4", "D4", "DS4", "E4", "F4", "FS4", "G4", "GS4", "A4", "AS4", "B4", "C5", "CS5", "D5", "DS5", "E5", 
  "F5", "FS5", "G5", "GS5", "A5", "AS5", "B5", "C6", "CS6", "D6", "DS6", "E6", "F6", "FS6", "G6", "GS6", "A6", "AS6", 
  "B6", "C7", "CS7", "D7", "DS7", "E7", "F7", "FS7", "G7", "GS7", "A7", "AS7", "B7", "C8", "CS8", "D8", "DS8" 
};

// LED matrix configuration
const int MATRIX_ROWS = 8;
const int MATRIX_COLS = 8;
const byte matrix_notes[][MATRIX_ROWS] PROGMEM = {
  { 0xE7, 0x95, 0xF5, 0x95, 0xE7, 0x00, 0x00, 0x00 }, { 0xF1, 0x83, 0x81, 0x81, 0xF1, 0x00, 0x00, 0x00 }, 
  { 0xF1, 0x83, 0x81, 0x81, 0xF1, 0x12, 0x3C, 0x48 }, { 0xE1, 0x93, 0x91, 0x91, 0xE1, 0x00, 0x00, 0x00 }, 
  { 0xE1, 0x93, 0x91, 0x91, 0xE1, 0x12, 0x3C, 0x48 }, { 0xF1, 0x83, 0xE1, 0x81, 0xF1, 0x00, 0x00, 0x00 }, 
  { 0xF1, 0x83, 0xF1, 0x81, 0x81, 0x00, 0x00, 0x00 }, { 0xF1, 0x83, 0xF1, 0x81, 0x81, 0x12, 0x3C, 0x48 }, 
  { 0xF1, 0x83, 0xB1, 0x91, 0xF1, 0x00, 0x00, 0x00 }, { 0xF1, 0x83, 0xB1, 0x91, 0xF1, 0x12, 0x3C, 0x48 }, 
  { 0x61, 0x93, 0x91, 0xF1, 0x91, 0x00, 0x00, 0x00 }, { 0x61, 0x93, 0xF1, 0x91, 0x91, 0x12, 0x3C, 0x48 }, 
  { 0xE1, 0x93, 0xE1, 0x91, 0xE1, 0x00, 0x00, 0x00 }, { 0xF7, 0x81, 0x87, 0x84, 0xF7, 0x00, 0x00, 0x00 }, 
  { 0xF7, 0x81, 0x87, 0x84, 0xF7, 0x12, 0x3C, 0x48 }, { 0xE7, 0x91, 0x97, 0x94, 0xE7, 0x00, 0x00, 0x00 }, 
  { 0xE7, 0x91, 0x97, 0x94, 0xE7, 0x12, 0x3C, 0x48 }, { 0xF7, 0x81, 0xE7, 0x84, 0xF7, 0x00, 0x00, 0x00 }, 
  { 0xF7, 0x81, 0xF7, 0x84, 0x87, 0x00, 0x00, 0x00 }, { 0xF7, 0x81, 0xF7, 0x84, 0x87, 0x12, 0x3C, 0x48 }, 
  { 0xF7, 0x81, 0xB7, 0x94, 0xF7, 0x00, 0x00, 0x00 }, { 0xF7, 0x81, 0xB7, 0x94, 0xF7, 0x12, 0x3C, 0x48 }, 
  { 0x67, 0x91, 0x97, 0xF4, 0x97, 0x00, 0x00, 0x00 }, { 0x67, 0x91, 0xF7, 0x94, 0x97, 0x12, 0x3C, 0x48 }, 
  { 0xE7, 0x91, 0xE7, 0x94, 0xE7, 0x00, 0x00, 0x00 }, { 0xF7, 0x81, 0x87, 0x81, 0xF7, 0x00, 0x00, 0x00 }, 
  { 0xF7, 0x81, 0x87, 0x81, 0xF7, 0x12, 0x3C, 0x48 }, { 0xE7, 0x91, 0x97, 0x91, 0xE7, 0x00, 0x00, 0x00 }, 
  { 0xE7, 0x91, 0x97, 0x91, 0xE7, 0x12, 0x3C, 0x48 }, { 0xF7, 0x81, 0xE7, 0x81, 0xF7, 0x00, 0x00, 0x00 }, 
  { 0xF7, 0x81, 0xF7, 0x81, 0x87, 0x00, 0x00, 0x00 }, { 0xF7, 0x81, 0xF7, 0x81, 0x87, 0x12, 0x3C, 0x48 }, 
  { 0xF7, 0x81, 0xB7, 0x91, 0xF7, 0x00, 0x00, 0x00 }, { 0xF7, 0x81, 0xB7, 0x91, 0xF7, 0x12, 0x3C, 0x48 }, 
  { 0x67, 0x91, 0x97, 0xF1, 0x97, 0x00, 0x00, 0x00 }, { 0x67, 0x91, 0xF7, 0x91, 0x97, 0x12, 0x3C, 0x48 }, 
  { 0xE7, 0x91, 0xE7, 0x91, 0xE7, 0x00, 0x00, 0x00 }, { 0xF1, 0x83, 0x87, 0x81, 0xF1, 0x00, 0x00, 0x00 }, 
  { 0xF1, 0x83, 0x87, 0x81, 0xF1, 0x12, 0x3C, 0x48 }, { 0xE1, 0x93, 0x97, 0x91, 0xE1, 0x00, 0x00, 0x00 }, 
  { 0xE1, 0x93, 0x97, 0x91, 0xE1, 0x12, 0x3C, 0x48 }, { 0xF1, 0x83, 0xE7, 0x81, 0xF1, 0x00, 0x00, 0x00 }, 
  { 0xF1, 0x83, 0xF7, 0x81, 0x81, 0x00, 0x00, 0x00 }, { 0xF1, 0x83, 0xF7, 0x81, 0x81, 0x12, 0x3C, 0x48 }, 
  { 0xF1, 0x83, 0xB7, 0x91, 0xF1, 0x00, 0x00, 0x00 }, { 0xF1, 0x83, 0xB7, 0x91, 0xF1, 0x12, 0x3C, 0x48 }, 
  { 0x61, 0x93, 0x97, 0xF1, 0x91, 0x00, 0x00, 0x00 }, { 0x61, 0x93, 0xF7, 0x91, 0x91, 0x12, 0x3C, 0x48 }, 
  { 0xE1, 0x93, 0xE7, 0x91, 0xE1, 0x00, 0x00, 0x00 }, { 0xF7, 0x84, 0x87, 0x81, 0xF7, 0x00, 0x00, 0x00 }, 
  { 0xF7, 0x84, 0x87, 0x81, 0xF7, 0x12, 0x3C, 0x48 }, { 0xE7, 0x94, 0x97, 0x91, 0xE7, 0x00, 0x00, 0x00 }, 
  { 0xE7, 0x94, 0x97, 0x91, 0xE7, 0x12, 0x3C, 0x48 }, { 0xF7, 0x84, 0xE7, 0x81, 0xF7, 0x00, 0x00, 0x00 }, 
  { 0xF7, 0x84, 0xF7, 0x81, 0x87, 0x00, 0x00, 0x00 }, { 0xF7, 0x84, 0xF7, 0x81, 0x87, 0x12, 0x3C, 0x48 }, 
  { 0xF7, 0x84, 0xB7, 0x91, 0xF7, 0x00, 0x00, 0x00 }, { 0xF7, 0x84, 0xB7, 0x91, 0xF7, 0x12, 0x3C, 0x48 }, 
  { 0x67, 0x94, 0x97, 0xF1, 0x97, 0x00, 0x00, 0x00 }, { 0x67, 0x94, 0xF7, 0x91, 0x97, 0x12, 0x3C, 0x48 }, 
  { 0xE7, 0x94, 0xE7, 0x91, 0xE7, 0x00, 0x00, 0x00 }, { 0xF4, 0x84, 0x87, 0x85, 0xF7, 0x00, 0x00, 0x00 }, 
  { 0xF4, 0x84, 0x87, 0x85, 0xF7, 0x12, 0x3C, 0x48 }, { 0xE4, 0x94, 0x97, 0x95, 0xE7, 0x00, 0x00, 0x00 }, 
  { 0xE4, 0x94, 0x97, 0x95, 0xE7, 0x12, 0x3C, 0x48 }, { 0xF4, 0x84, 0xE7, 0x85, 0xF7, 0x00, 0x00, 0x00 }, 
  { 0xF4, 0x84, 0xF7, 0x85, 0x87, 0x00, 0x00, 0x00 }, { 0xF4, 0x84, 0xF7, 0x85, 0x87, 0x12, 0x3C, 0x48 }, 
  { 0xF4, 0x84, 0xB7, 0x95, 0xF7, 0x00, 0x00, 0x00 }, { 0xF4, 0x84, 0xB7, 0x95, 0xF7, 0x12, 0x3C, 0x48 }, 
  { 0x64, 0x94, 0x97, 0xF5, 0x97, 0x00, 0x00, 0x00 }, { 0x64, 0x94, 0xF7, 0x95, 0x97, 0x12, 0x3C, 0x48 }, 
  { 0xE4, 0x94, 0xE7, 0x95, 0xE7, 0x00, 0x00, 0x00 }, { 0xF7, 0x81, 0x81, 0x81, 0xF1, 0x00, 0x00, 0x00 }, 
  { 0xF7, 0x81, 0x81, 0x81, 0xF1, 0x12, 0x3C, 0x48 }, { 0xE7, 0x91, 0x91, 0x91, 0xE1, 0x00, 0x00, 0x00 }, 
  { 0xE7, 0x91, 0x91, 0x91, 0xE1, 0x12, 0x3C, 0x48 }, { 0xF7, 0x81, 0xE1, 0x81, 0xF1, 0x00, 0x00, 0x00 }, 
  { 0xF7, 0x81, 0xF1, 0x81, 0x81, 0x00, 0x00, 0x00 }, { 0xF7, 0x81, 0xF1, 0x81, 0x81, 0x12, 0x3C, 0x48 }, 
  { 0xF7, 0x81, 0xB1, 0x91, 0xF1, 0x00, 0x00, 0x00 }, { 0xF7, 0x81, 0xB1, 0x91, 0xF1, 0x12, 0x3C, 0x48 }, 
  { 0x67, 0x91, 0x91, 0xF1, 0x91, 0x00, 0x00, 0x00 }, { 0x67, 0x91, 0xF1, 0x91, 0x91, 0x12, 0x3C, 0x48 }, 
  { 0xE7, 0x91, 0xE1, 0x91, 0xE1, 0x00, 0x00, 0x00 }, { 0xF7, 0x85, 0x87, 0x85, 0xF7, 0x00, 0x00, 0x00 }, 
  { 0xF7, 0x85, 0x87, 0x85, 0xF7, 0x12, 0x3C, 0x48 }, { 0xE7, 0x95, 0x97, 0x95, 0xE7, 0x00, 0x00, 0x00 }, 
  { 0xE7, 0x95, 0x97, 0x95, 0xE7, 0x12, 0x3C, 0x48 }
};
const int NUM_FRAMES = sizeof(matrix_notes) / sizeof(matrix_notes[0]);

// System state variables
int scale = 0;                  // Current scale mode (0=OFF, 1=Continuous, etc.)
bool output_ok = false;         // Audio output enable flag
int octave = 12;                // Central octave offset
int starting_note = 37;         // Default: C4 (MIDI note 60)
bool ON_mode = false;           // Synthesis engine active flag
String scale_name = "";         // Current scale name

// Ultrasonic sensor
SR04 sensor = SR04(12, 11);     // Trig=12, Echo=11
long distance;                  // Measured distance
int echoNote = 220;             // Current note frequency

// Distance parameters 
const int starting_distance = 2.5;  // higher note
const int step = 2.5;               // changing note step

// Button configuration
const int buttonOne = 2;
const int buttonTwo = 4;
const int buttonThree = 5;
const int buttonFour = 6;
const int buttonFive = 13;
const int buttonSix = 7;

// Button state tracking
int buttonStates[6] = {0};      // Debounce states

// Sound synthesis parameters
uint16_t syncPhaseAcc;
uint16_t syncPhaseInc;
uint16_t grainPhaseAcc;
uint16_t grainPhaseInc;
uint16_t grainAmp;
uint8_t grainDecay;
uint16_t grain2PhaseAcc;
uint16_t grain2PhaseInc;
uint16_t grain2Amp;
uint8_t grain2Decay;

// Potentiometer mapping
#define GRAIN_FREQ_CONTROL 0
#define GRAIN_DECAY_CONTROL 1
#define GRAIN2_FREQ_CONTROL 2
#define GRAIN2_DECAY_CONTROL 3

// PWM configuration
#if defined(__AVR_ATmega1280__)
  #define PWM_PIN 3
  #define PWM_VALUE OCR3C
  #define PWM_INTERRUPT TIMER3_OVF_vect
#else
  #define PWM_PIN 3
  #define PWM_VALUE OCR2B
  #define PWM_INTERRUPT TIMER2_OVF_vect
#endif

// Note buffers
#define NUM_NOTE_FRAMES 15
byte notes_name_matrix_to_be_played[NUM_NOTE_FRAMES][MATRIX_ROWS];
int notes_to_be_played[NUM_NOTE_FRAMES];

// ======================================================================
// INITIALIZATION
// ======================================================================
void setup() {
  // LED matrix setup
  lc.shutdown(0, false);
  lc.setIntensity(0, 6);
  lc.clearDisplay(0);

  // LCD setup
  lcd.init();
  lcd.clear();
  lcd.backlight();

  // Button setup
  pinMode(buttonOne, INPUT_PULLUP);
  pinMode(buttonTwo, INPUT_PULLUP);
  pinMode(buttonThree, INPUT_PULLUP);
  pinMode(buttonFour, INPUT_PULLUP);
  pinMode(buttonFive, INPUT_PULLUP);
  pinMode(buttonSix, INPUT_PULLUP);

  // Serial setup for debugging
  //Serial.begin(9600);

  // Initial state
  scale_name = "OFF";
  update_display();
  update_notes(scale, starting_note, octave);
  
  // Audio system initialization
  pinMode(PWM_PIN, OUTPUT);
  audioOn();
}

// ======================================================================
// MAIN LOOP
// ======================================================================
void loop() {
  // 1. Process button inputs
  handleButtons();
  
  // 2. Read distance sensor
  distance = sensor.Distance();
  
  // 3. Determine note based on distance and scale
  if (scale == 1) {  // Continuous mode
    // up to 15 step (step + step*15)
    echoNote = map(distance, 0, (starting_distance + (step * 15)), 4978, 31);
    lc.clearDisplay(0);  // Clear matrix in continuous mode
  } 
  else if (ON_mode) {
    // Discrete note selection
    for (int i = 0; i < NUM_NOTE_FRAMES; i++) {
      int dist_min = starting_distance + (step * i);
      int dist_max = starting_distance + (step * (i + 1));
      
      if (distance >= dist_min && distance < dist_max) {
        echoNote = notes_to_be_played[i];
        
        // Display note pattern on LED matrix
        for (int row = 0; row < MATRIX_ROWS; row++) {
          lc.setRow(0, row, notes_name_matrix_to_be_played[i][row]);
        }
        break;
      }
    }
  }

  // 4. Update synthesis parameters if active
  if (distance < (starting_distance + step * 15) && ON_mode) {
    syncPhaseInc = echoNote;
    grainPhaseInc = mapPhaseInc(analogRead(GRAIN_FREQ_CONTROL)) / 2;
    grainDecay = analogRead(GRAIN_DECAY_CONTROL) / 8;
    grain2PhaseInc = mapPhaseInc(analogRead(GRAIN2_FREQ_CONTROL)) / 2;
    grain2Decay = analogRead(GRAIN2_DECAY_CONTROL) / 4;
    output_ok = true;
  } 
  else {
    output_ok = false;
    lc.clearDisplay(0);
  }
}

// ======================================================================
// BUTTON HANDLING
// ======================================================================
void handleButtons() {
  bool display_updated = false;
  
  // Button 1: Decrease starting note
  if (digitalRead(buttonOne) == LOW && buttonStates[0] == 0) {
    starting_note = constrain(starting_note - 1, 0, 73);
    update_notes(scale, starting_note, octave);
    buttonStates[0] = 1;
    display_updated = true;
  } 
  else if (digitalRead(buttonOne) == HIGH) {
    buttonStates[0] = 0;
  }

  // Button 2: Increase starting note
  if (digitalRead(buttonTwo) == LOW && buttonStates[1] == 0) {
    starting_note = constrain(starting_note + 1, 0, 73);
    update_notes(scale, starting_note, octave);
    buttonStates[1] = 1;
    display_updated = true;
  } 
  else if (digitalRead(buttonTwo) == HIGH) {
    buttonStates[1] = 0;
  }

  // Button 3: Decrease octave
  if (digitalRead(buttonThree) == LOW && buttonStates[2] == 0) {
    octave = constrain(octave - 12, 0, 48);
    update_notes(scale, starting_note, octave);
    buttonStates[2] = 1;
    display_updated = true;
  } 
  else if (digitalRead(buttonThree) == HIGH) {
    buttonStates[2] = 0;
  }

  // Button 4: Increase octave
  if (digitalRead(buttonFour) == LOW && buttonStates[3] == 0) {
    octave = constrain(octave + 12, 0, 48);
    update_notes(scale, starting_note, octave);
    buttonStates[3] = 1;
    display_updated = true;
  } 
  else if (digitalRead(buttonFour) == HIGH) {
    buttonStates[3] = 0;
  }

  // Button 5: Previous scale
  if (digitalRead(buttonFive) == LOW && buttonStates[4] == 0) {
    scale = constrain(scale - 1, 0, 6);
    update_notes(scale, starting_note, octave);
    buttonStates[4] = 1;
    display_updated = true;
  } 
  else if (digitalRead(buttonFive) == HIGH) {
    buttonStates[4] = 0;
  }

  // Button 6: Next scale
  if (digitalRead(buttonSix) == LOW && buttonStates[5] == 0) {
    scale = constrain(scale + 1, 0, 6);
    update_notes(scale, starting_note, octave);
    buttonStates[5] = 1;
    display_updated = true;
  } 
  else if (digitalRead(buttonSix) == HIGH) {
    buttonStates[5] = 0;
  }

  // Update display only if a button was pressed
  if (display_updated) {
    update_display();
  }
}

// ======================================================================
// DISPLAY UPDATE FUNCTION
// ======================================================================
void update_display() {
  // First line: Scale information
  lcd.setCursor(0, 0);
  lcd.print("                ");  // Clear line
  lcd.setCursor(0, 0);
  lcd.print(scale);
  lcd.print(" ");
  lcd.print(scale_name);
  
  // Second line: Conditional display
  lcd.setCursor(0, 1);
  lcd.print("                ");  // Clear line
  
  // Only show starting note for scales 2 and above
  if (scale >= 2) {
    lcd.setCursor(0, 1);
    lcd.print("Starting note");
    
    // Calculate remaining space and display note name
    int remaining_space = 16 - 13; // "Starting note" is 13 characters
    const char* note = notes_name[starting_note + octave];
    int note_length = strlen(note);
    
    if (note_length <= remaining_space) {
      lcd.print(note);
    } else {
      // If note name is too long, truncate it
      for (int i = 0; i < remaining_space; i++) {
        lcd.print(note[i]);
      }
    }
  }
}

// ======================================================================
// SOUND ENGINE
// ======================================================================
SIGNAL(PWM_INTERRUPT) {
  uint8_t value;
  uint16_t output;

  syncPhaseAcc += syncPhaseInc;
  if (syncPhaseAcc < syncPhaseInc) {
    // Time to start the next grain
    grainPhaseAcc = 0;
    grainAmp = 0x7fff;
    grain2PhaseAcc = 0;
    grain2Amp = 0x7fff;
    PORTB ^= 1 << 5;  // Toggle LED on pin 13
  }

  // Increment the phase of the grain oscillators
  grainPhaseAcc += grainPhaseInc;
  grain2PhaseAcc += grain2PhaseInc;

  // Convert phase into a triangle wave
  value = (grainPhaseAcc >> 7) & 0xff;
  if (grainPhaseAcc & 0x8000) value = ~value;
  // Multiply by current grain amplitude to get sample
  output = value * (grainAmp >> 8);

  // Repeat for second grain
  value = (grain2PhaseAcc >> 7) & 0xff;
  if (grain2PhaseAcc & 0x8000) value = ~value;
  output += value * (grain2Amp >> 8);

  // Make the grain amplitudes decay by a factor every sample
  grainAmp -= (grainAmp >> 8) * grainDecay;
  grain2Amp -= (grain2Amp >> 8) * grain2Decay;

  // Scale output to the available range
  output >>= 9;
  if (output > 255) output = 255;

  // Output to PWM if enabled
  if (output_ok) {
    PWM_VALUE = output;
  }
}

void audioOn() {
  #if defined(__AVR_ATmega1280__)
    TCCR3A = _BV(COM3C1) | _BV(WGM30);
    TCCR3B = _BV(CS30);
    TIMSK3 = _BV(TOIE3);
  #else
    TCCR2A = _BV(COM2B1) | _BV(WGM20);
    TCCR2B = _BV(CS20);
    TIMSK2 = _BV(TOIE2);
  #endif
}

// Frequency mapping functions
uint16_t mapPhaseInc(uint16_t input) {
  static const uint16_t antilogTable[] = {
    64830,64132,63441,62757,62081,61413,60751,60097,59449,58809,58176,57549,56929,56316,55709,55109,
    54515,53928,53347,52773,52204,51642,51085,50535,49991,49452,48920,48393,47871,47356,46846,46341,
    45842,45348,44859,44376,43898,43425,42958,42495,42037,41584,41136,40693,40255,39821,39392,38968,
    38548,38133,37722,37316,36914,36516,36123,35734,35349,34968,34591,34219,33850,33486,33125,32768
  };
  return (antilogTable[input & 0x3f]) >> (input >> 6);
}

// ======================================================================
// SCALE MANAGEMENT 
// ======================================================================
void update_notes(int scale_update, int starting_note_update, int octave_update) {
  // Update note sequences based on selected scale
  switch (scale_update) {
    case 0:  // OFF
      ON_mode = false;
      scale_name = "OFF";
      lc.clearDisplay(0);
      break;
    
    case 1:  // Continuous
      ON_mode = true;
      scale_name = "Continuous";
      break;
    
    case 2:  // Chromatic
      {
        ON_mode = true;
        scale_name = "Chromatic";
        for (int i = 0; i < NUM_NOTE_FRAMES; i++) {
          int note_index = constrain(starting_note_update + octave_update - i, 0, 87);
          notes_to_be_played[i] = notes[note_index];
          for (int row = 0; row < MATRIX_ROWS; row++) {
            notes_name_matrix_to_be_played[i][row] = 
              pgm_read_byte(&matrix_notes[note_index][row]);
          }
        }
      }
      break;
    
    case 3:  // Major
      {
        ON_mode = true;
        scale_name = "Major";
        // Major scale intervals: W-W-H-W-W-W-H
        int major_offsets[15] = {0, -1, -3, -5, -7, -8, -10, -12, -13, -15, -17, -19, -20, -22, -24};
        for (int i = 0; i < NUM_NOTE_FRAMES; i++) {
          int note_index = constrain(starting_note_update + octave_update + major_offsets[i], 0, 87);
          notes_to_be_played[i] = notes[note_index];
          for (int row = 0; row < MATRIX_ROWS; row++) {
            notes_name_matrix_to_be_played[i][row] = 
              pgm_read_byte(&matrix_notes[note_index][row]);
          }
        }
      }
      break;
    
    case 4:  // Natural Minor
      {
        ON_mode = true;
        scale_name = "Nat Minor";
        // Natural minor scale intervals: W-H-W-W-H-W-W
        int minor_offsets[15] = {0, -2, -4, -5, -7, -9, -10, -12, -14, -16, -17, -19, -21, -22, -24};
        for (int i = 0; i < NUM_NOTE_FRAMES; i++) {
          int note_index = constrain(starting_note_update + octave_update + minor_offsets[i], 0, 87);
          notes_to_be_played[i] = notes[note_index];
          for (int row = 0; row < MATRIX_ROWS; row++) {
            notes_name_matrix_to_be_played[i][row] = 
              pgm_read_byte(&matrix_notes[note_index][row]);
          }
        }
      }
      break;
    
    case 5:  // Harmonic Minor
      {
        ON_mode = true;
        scale_name = "Harm Minor";
        // Harmonic minor scale intervals: W-H-W-W-H-WH-H
        int harm_offsets[15] = {0, -1, -4, -5, -7, -9, -10, -12, -13, -16, -17, -19, -21, -22, -24};
        for (int i = 0; i < NUM_NOTE_FRAMES; i++) {
          int note_index = constrain(starting_note_update + octave_update + harm_offsets[i], 0, 87);
          notes_to_be_played[i] = notes[note_index];
          for (int row = 0; row < MATRIX_ROWS; row++) {
            notes_name_matrix_to_be_played[i][row] = 
              pgm_read_byte(&matrix_notes[note_index][row]);
          }
        }
      }
      break;
    
    case 6:  // Pentatonic
      {
        ON_mode = true;
        scale_name = "Pentatonic";
        // Pentatonic scale intervals: W-W-W-WH-W
        int pentatonic_offsets[15] = {0, -3, -5, -8, -10, -12, -15, -17, -20, -22, -24, -27, -29, -32, -34};
        for (int i = 0; i < NUM_NOTE_FRAMES; i++) {
          int note_index = constrain(starting_note_update + octave_update + pentatonic_offsets[i], 0, 87);
          notes_to_be_played[i] = notes[note_index];
          for (int row = 0; row < MATRIX_ROWS; row++) {
            notes_name_matrix_to_be_played[i][row] = 
              pgm_read_byte(&matrix_notes[note_index][row]);
          }
        }
      }
      break;
  }
}