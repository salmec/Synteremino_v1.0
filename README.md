# Synteremino_v1.0
Ultrasonic Distance Controlled Granular Synthesizer

Project description

Inspired by Theremin, Synteremino v1.0 transforms hand gestures into ethereal soundscapes using an ultrasonic distance sensor. This Arduino-powered granular synthesizer generates rich, textured audio by manipulating "sound grains" in real-time. As you move your hand closer or farther from the sensor, pitch and timbre morph dynamically across 15 discrete notes or a continuous frequency range. Visual feedback is provided through an 8x8 LED matrix (showing note patterns) and a 16x2 LCD (displaying scales/notes). 
Key Features
1. Gesture-Controlled Synthesis: 
◦ Ultrasonic sensor (HC-SR04) maps hand distance (3–48 cm) to sound. 
◦ 6 Scale Modes: Continuous, Chromatic, Major, Natural Minor, Harmonic Minor, Pentatonic. 
◦ Central note and octave adjustable via tactile buttons. 
2. Granular Sound Engine (Credit Peter Knight http://code.google.com/p/tinkerit/wiki/Auduino): 
◦ Dual-grain oscillator with triangle-wave synthesis. 
◦ Real-time parameter control via potentiometers: 
▪ Grain 1: Frequency/Decay 
▪ Grain 2: Frequency/Decay 
3. Visual Feedback: 
◦ LED Matrix: Displays musical note played. 
◦ LCD: Shows active scale, root note, and octave. 
4. Amplified Audio Output: 
◦ 2x10W Audio Amplifier (PAM8610) with volume control knob. 
◦ Drives two 10W 8Ω speakers for immersive sound (When a 6.35mm male jack is inserted the audio output of the amplifier is muted ). 
Hardware Setup

Component	Specification 
Microcontroller:	Arduino Uno R3 
Power:	12V DC (Barrel Jack) 
Audio:	2x10W Amp + Volume Pot PAM 8610 
Sensors:	HC-SR04 Ultrasonic Sensor 
Displays:	8x8 LED Matrix (MAX7219), 16x2 I2C LCD 
Controls:	6x Tactile Buttons, 4x Pots 
Wiring Guide
Ultrasonic Sensor: Trig=Pin12, Echo=Pin11 
LED Matrix: DIN=10, CLK=8, CS=9 
LCD: I2C (Address 0x27)  (SDA,SCL) 
Buttons: Pins 2,4,5,6,7,13 
Audio Out: PWM Pin 3 → 6.5mm Jack female or Amplifier → Speakers 

Installation
1. Libraries Required: 
◦ LedControl (LED Matrix) 
◦ LiquidCrystal_I2C (LCD) 
◦ SR04 (Ultrasonic Sensor) 
2. Upload Code: 
◦ Compile and upload Synteremino_V1.0 to Arduino. 
3. Power Up: 
◦ Connect 12V DC power to Arduino barrel jack and Audio amplifier 

Usage
1. Scale Selection: 
◦ Cycle through scales with Button 5 (←) and Button 6 (→). 
◦ "OFF" mode mutes audio. 
2. Note Adjustment: 
◦ Buttons 1/2: Decrease/Increase root note. 
◦ Buttons 3/4: Shift octave down/up.
3. Gesture Control: 
◦ Move hand horizontally close to the sensor ( about 3–48 cm): 
▪ Close: Higher pitches 
▪ Far: Lower pitches 
◦ LED matrix lights up active note symbols. 
4. Sound Shaping: 
◦ Tweak potentiometers to morph grain textures: 
▪ Pot 1: Grain 1 Frequency 
▪ Pot 2: Grain 1 Decay 
▪ Pot 3: Grain 2 Frequency 
▪ Pot 4: Grain 2 Decay 
5. Audio Output 
◦ Audio Output goes to Amplifier and Speakers, except when a 6.35 Jack male is inserted, then the Speakers are muted. 

FUTURE IMPROVEMENTS
▪ Improve Audio output, using dedicated lybraries (i.e. Mozzi) 
▪ Add Midi Output feature 
▪ Add configuration feature, as creating new scale using push buttons or joystick 
▪ Add Pitch Wheel 

