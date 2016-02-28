// LED_Controller.ino

#include "color.h"
#include "float_color.h"
#include "fade.h"
#include "flash.h"
#include "spectrum_cycle.h"

// Declares the RGB PINS for Arduino
const uint8_t RED_PIN					= 3;
const uint8_t GREEN_PIN					= 5;
const uint8_t BLUE_PIN					= 6;

// Declares the RGB WRITE COMMANDS
const uint8_t WRITE_RED					= 'R';
const uint8_t WRITE_GREEN				= 'G';
const uint8_t WRITE_BLUE				= 'B';

// Declares the RGB READ COMMANDS
const uint8_t READ_RED					= 'C';
const uint8_t READ_GREEN 				= 'D';
const uint8_t READ_BLUE 				= 'E';

// Declares the POWER COMMANDS
const uint8_t TURN_ON					= 'N';
const uint8_t TURN_OFF					= 'F';

// Declares the EFFECTS COMMANDS
const uint8_t FADE						= 'H';
const uint8_t BREATHING					= 'I';
const uint8_t SPECTRUM_CYCLING			= 'J';
const uint8_t STATIC					= 'K';
const uint8_t FLASH 					= 'L';
const uint8_t DOUBLE_FLASH				= 'M';

Fade fade = Fade();
SpectrumCycle spectrumCycling = SpectrumCycle();
Flash flash = Flash();

// Boolean to check if LEDs powered on/off.
uint8_t ledPower						= 1;

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

	// Starts serial connection.
	Serial.begin(rate);

	// Initializes actual time.
	now = micros();
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
			analogWrite(RED_PIN, 0);
			analogWrite(GREEN_PIN, 0);
			analogWrite(BLUE_PIN, 0);
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
	analogWrite(RED_PIN, currentColor.getRed());
	analogWrite(GREEN_PIN, currentColor.getGreen());
	analogWrite(BLUE_PIN, currentColor.getBlue());
}

// This function updates PIN values and TIME variables.
void reset() {
	updateColor();

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
	reset();
}

void initializeFlashingEffect() {
	currentEffect = FLASH;

	flash.initializeEffect(baseColor, code[0] == DOUBLE_FLASH);

	reset();
}

// Processes the COMMAND received from SERIAL.
void process() {
	switch(code[0]){
		case TURN_ON:
			turnOnProcess();
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
	}
}

// This function the current effect single iteration.
void processEffect() {
	switch(currentEffect) {
		// Process FADE effect.
		case FADE:
			fade.processEffect(currentColor, deltaTime);
			break;
		case FLASH:
			flash.processEffect(currentColor);
			break;
		// Process SPECTRUM CYCLING effect.
		case SPECTRUM_CYCLING:
			spectrumCycling.processEffect(
				currentColor, deltaTime);
			break;
		case STATIC:
			staticEffect();
			break;
	}
	// Updates PINS with current RGB values.
	updateColor();
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
			process();
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