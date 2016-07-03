#include <Arduino.h>
#include <SPI.h>

// LED_Controller.ino

#include "color/color.h"
#include "color/float_color.h"
#include "effects/fade/fade.h"
#include "effects/flash/flash.h"
#include "effects/rainbow_spin/rainbow_spin.h"
#include "effects/spectrum_cycle/spectrum_cycle.h"

// Declares the RGB PINS for Arduino
const uint8_t RED_PIN				= 3;
const uint8_t GREEN_PIN			= 5;
const uint8_t BLUE_PIN			= 6;

// Declares HANDSHAKE COMMANDS
const uint8_t SYN_IN						= 0x01;
const uint8_t ACK_IN						= 0xFF;

// Declares MODEL COMMAND
const uint8_t MODEL							= 'P';

// Declares the RGB WRITE COMMANDS
const uint8_t WRITE_RED					= 'R';
const uint8_t WRITE_GREEN				= 'G';
const uint8_t WRITE_BLUE				= 'B';

// Declares the RGB READ COMMANDS
const uint8_t READ_RED					= 'C';
const uint8_t READ_GREEN				= 'D';
const uint8_t READ_BLUE					= 'E';

// Declares the POWER COMMANDS
const uint8_t TURN_ON						= 'N';
const uint8_t TURN_OFF					= 'F';

// Declares the EFFECTS COMMANDS
const uint8_t FADE							= 'H';
const uint8_t BREATHING					= 'I';
const uint8_t SPECTRUM_CYCLING	= 'J';
const uint8_t STATIC						= 'K';
const uint8_t FLASH 						= 'L';
const uint8_t DOUBLE_FLASH			= 'M';
const uint8_t RAINBOW_SPIN			= 'O';

//Declares the EFFECT SPEED COMMANDS
const uint8_t SPEED							= 'S';
const uint8_t TRIPLE						= 'T';
const uint8_t DOUBLE						= 'U';
const uint8_t NORMAL						= 'V';
const uint8_t HALF							= 'W';
const uint8_t THIRD							= 'X';

const uint32_t handShakeTime = 1000000;

// Boolean to check if serial authentication has been established.
uint8_t handShakeEstablished = 0;
uint8_t synCheck = 0;
uint32_t handShakeStart;

// Boolean to check if LEDs powered on/off.
uint8_t ledPower = 1;

// Boolean to check the use of the LPD8806 library.
uint8_t individualLEDController = 1;

// Number of LEDs connected to the strip.
uint8_t ledNumber = 92;

LPD8806 strip = LPD8806(ledNumber);

Fade fade = Fade(strip);
SpectrumCycle spectrumCycling = SpectrumCycle(strip);
Flash flash = Flash(strip);
RainbowSpin rainbowSpin = RainbowSpin(strip);

// Base RGB color from witch each effect starts.
Color baseColor = Color();

// Actual RBG color value.
FloatColor currentColor = FloatColor();

// Actual effect type.
uint8_t currentEffect = STATIC;

// Actual time in milliseconds.
uint32_t now;

// Time since the last iteration was executed.
uint32_t lastTime;

// Elapsed time between actual time and last iteration.
float deltaTime;

// Byte array where the COMMAND is stored.
uint8_t code[2];

// Index to access the COMMAND array.
uint8_t index = 0;

//Arduino's initialization
void setup() {
	// Sets serial communication's baud rate.
	const uint16_t rate = 9600;

	// Configure RGB pins as outputs.
	pinMode(RED_PIN, OUTPUT);
	pinMode(GREEN_PIN, OUTPUT);
	pinMode(BLUE_PIN, OUTPUT);

	// Initializes RGB color into the pins.
	analogWrite(RED_PIN, baseColor.getRed());
	analogWrite(GREEN_PIN, baseColor.getGreen());
	analogWrite(BLUE_PIN, baseColor.getBlue());

	// Defines if Single LED Controller is used to render effects.
	fade.enableSingleLEDController(individualLEDController);
	spectrumCycling.enableSingleLEDController(individualLEDController);
	flash.enableSingleLEDController(individualLEDController);

	// Starts serial connection.
	Serial.begin(rate);

	// Start up the LED strip
	strip.begin();

	// Initializes actual time.
	now = micros();
	deltaTime = 0;
}

// This function WRITES values into RGB pins depending on the received COMMAND.
void writeOnPin() {
	switch(code[0]) {
		case WRITE_RED:
			// Updates RED base color.
			baseColor.setRed(code[1]);
			// Writes RED color value into the PIN.
			analogWrite(RED_PIN, code[1]);
			break;
		case WRITE_GREEN:
			// Updates GREEN base color.
			baseColor.setGreen(code[1]);
			// Writes GREEN color value into the PIN.
			analogWrite(GREEN_PIN, code[1]);
			break;
		case WRITE_BLUE:
			// Updates BLUE base color.
			baseColor.setBlue(code[1]);
			// Writes BLUE color value into the PIN.
			analogWrite(BLUE_PIN, code[1]);
			break;
		case TURN_OFF:
			// Sets POWER flag to FALSE.
			ledPower = 0;

			// Writes 0 value into the PINs to turn off the LEDs.
			if(individualLEDController) {
				for(uint16_t i = 0; i < strip.numPixels(); i++) {
					strip.setPixelColor(i, 0);
				}
				strip.show();
			} else {
				analogWrite(RED_PIN, 0);
				analogWrite(GREEN_PIN, 0);
				analogWrite(BLUE_PIN, 0);
			}
			break;
	}
}

// This function SENDS the values of the base RGB color to the SERIAL.
void readPin() {
	switch(code[0]) {
		case READ_RED:
			// Sends RED component of the base color.
			Serial.write(baseColor.getRed());
			break;
		case READ_GREEN:
			// Sends GREEN component of the base color.
			Serial.write(baseColor.getGreen());
			break;
		case READ_BLUE:
			// Sends BLUE component of the base color.
			Serial.write(baseColor.getBlue());
			break;
	}
}

// This function restarts the effect and resumes the effect process.
void turnOnProcess() {
	// Activates the POWER ON flag to resume effect procedure.
	ledPower = 1;
	// Initializes the current effect.
	switch (currentEffect) {
		case BREATHING: case FADE:
			initializeFadeBreathingEffect();
			break;
		case RAINBOW_SPIN:
			initializeRainbowSpinEffect();
			break;
		case SPECTRUM_CYCLING:
			initializeSpectrumCyclingEffect();
			break;
		case STATIC:
			staticEffect();
			break;
	}
}

// This functions sets current RGB values with the base color.
void initializeCurrentColorValues() {
	currentColor.setColors(baseColor);
}

// This function WRITES into the PINS the current RGB values of the effect.
void updateColor() {
	if(individualLEDController){
		for(uint16_t i = 0; i < strip.numPixels(); i++) {
			strip.setPixelColor(
				i,
				currentColor.getRed(),
				currentColor.getGreen(),
				currentColor.getBlue()
			);
		}
		strip.show();
	} else {
		analogWrite(RED_PIN, currentColor.getRed());
		analogWrite(GREEN_PIN, currentColor.getGreen());
		analogWrite(BLUE_PIN, currentColor.getBlue());
	}
}

// This function updates PIN values and TIME variables.
void reset() {
	now = micros();
	fade.setStartTime(now);
	flash.setStartTime(now);
}

// This function PROCESSES the STATIC EFFECT.
void staticEffect() {
	// Sets current effect to STATIC.
	currentEffect = STATIC;

	// Updates current RGB values with the base color.
	initializeCurrentColorValues();

	// Updates PINS with current RGB values.
	updateColor();
}

// This function INITIALIZES the FADE and BREATHING EFFECTS.
void initializeFadeBreathingEffect() {
	// Sets current effect to FADE.
	currentEffect = FADE;

	// Updates current RGB values with the base color.
	initializeCurrentColorValues();

	fade.initializeEffect(currentColor, code[0] == BREATHING);

	// Updates PINS values and TIME variables.
	fade.updateStripColor(Color(currentColor), RED_PIN, GREEN_PIN, BLUE_PIN);
	reset();
}

// This function INITIALIZES the SPECTRUM CYCLING EFFECT.
void initializeSpectrumCyclingEffect() {
	// Sets current effect to SPECTRUM CYCLING.
	currentEffect = SPECTRUM_CYCLING;
	spectrumCycling.initializeEffect();

	// Updates current RGB values with the base color.
	initializeCurrentColorValues();

	// Updates PINS values and TIME variables.
	spectrumCycling.updateStripColor(
		Color(currentColor),
		RED_PIN, GREEN_PIN, BLUE_PIN
	);
	reset();
}

void initializeFlashingEffect() {
	currentEffect = FLASH;

	flash.initializeEffect(baseColor, code[0] == DOUBLE_FLASH);

	flash.updateStripColor(Color(currentColor), RED_PIN, GREEN_PIN, BLUE_PIN);
	reset();
}

void initializeRainbowSpinEffect() {
	currentEffect = RAINBOW_SPIN;

	rainbowSpin.initializeEffect();
	rainbowSpin.processEffect();

	reset();
}

float speedComToFloat() {
	switch(code[1]) {
		case TRIPLE:
			return 3;
		case DOUBLE:
			return 2;
		case NORMAL:
			return 1;
		case HALF:
			return 0.5;
		case THIRD:
			return 0.333;
		default:
			return 0;
	}
}

void processSpeed() {
	float speedCom = speedComToFloat();
	switch(currentEffect) {
		case FADE: case BREATHING:
			fade.setSpeed(speedCom);
			break;
		case SPECTRUM_CYCLING:
			spectrumCycling.setSpeed(speedCom);
			break;
		case FLASH: case DOUBLE_FLASH:
			flash.setSpeed(speedCom);
			break;
	}
}

void processModel() {
	if(individualLEDController) {
		Serial.write(Color::SIGNED_RANGE - 1);
	} else {
		Serial.write(Color::UNSIGNED_RANGE - 1);
	}
}

void processHandShake() {
	if(micros() - handShakeStart >= handShakeTime) {
		synCheck = 0;
	}
	switch (code[0]) {
		case SYN_IN:
			synCheck = 1;
			Serial.write(ACK_IN);
			handShakeStart = micros();
			break;
		case ACK_IN:
			if(synCheck) {
				handShakeEstablished = 1;
			}
			break;
	}
}

// Processes the COMMAND received from SERIAL.
void process() {
	switch(code[0]){
		case SYN_IN:
			processHandShake();
			break;
		case TURN_ON:
			turnOnProcess();
			break;
		case WRITE_RED: case WRITE_GREEN: case WRITE_BLUE: case TURN_OFF:
			writeOnPin();
			break;
		case READ_RED: case READ_GREEN: case READ_BLUE:
			readPin();
			break;
		case BREATHING: case FADE:
			initializeFadeBreathingEffect();
			break;
		case SPECTRUM_CYCLING:
			initializeSpectrumCyclingEffect();
			break;
		case STATIC:
			staticEffect();
			break;
		case FLASH: case DOUBLE_FLASH:
			initializeFlashingEffect();
			break;
		case RAINBOW_SPIN:
			initializeRainbowSpinEffect();
			break;
		case SPEED:
			processSpeed();
			break;
		case MODEL:
			processModel();
			break;
	}
}

// This function the current effect single iteration.
void processEffect() {
	switch(currentEffect) {
		// Process FADE effect.
		case FADE:
			fade.processEffect(currentColor, deltaTime);
			fade.updateStripColor(
				Color(currentColor), RED_PIN, GREEN_PIN, BLUE_PIN
			);
			break;
		case FLASH:
			flash.processEffect(currentColor);
			flash.updateStripColor(
				Color(currentColor), RED_PIN, GREEN_PIN, BLUE_PIN
			);
			break;
		// Process SPECTRUM CYCLING effect.
		case SPECTRUM_CYCLING:
			spectrumCycling.processEffect(
				currentColor, deltaTime);
			fade.updateStripColor(
				Color(currentColor), RED_PIN, GREEN_PIN, BLUE_PIN
			);
			break;
		case RAINBOW_SPIN:
			rainbowSpin.processEffect();
			break;
		case STATIC:
			staticEffect();
			// Updates PINS with current RGB values.
			updateColor();
			break;
	}
}

// The iterative function that is executed after setup function.
void loop() {
	// Lock the main loop until the complete command is received.
	while(Serial.available()) {
		// Reads the COMMAND byte per byte.
		uint8_t serialIn = Serial.read();
		if(serialIn != '\n') {
			// Appends COMMAND bytes into the array until new line is received.
			code[index] = serialIn;
			index++;
		} else {
			// When the new line is received processes the COMMAND.
			index = 0;
			if(handShakeEstablished) {
				process();
			} else {
				processHandShake();
			}
		}
	}

	// After the COMMAND is processed updated the elapsed time
	// since last iteration.
	lastTime = now;
	now = micros();
	deltaTime = (now - lastTime) / 1000000.0;
	//IF led POWER is ON processes the current light effect.
	if(ledPower) {
		processEffect();
	}
}
