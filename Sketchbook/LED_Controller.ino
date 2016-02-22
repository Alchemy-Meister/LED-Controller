// LED_Controller.ino

#include <math.h>

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

// Declares bi-dimensional array for storing spectrum's RGB colors.
const uint8_t COLORS_LENGTH	= 16;
const uint8_t SPECTRUM_COLORS[COLORS_LENGTH][3] = {
	{255, 0, 0},
	{255, 0, 0},
	{255, 153, 0},
	{255, 153, 0},
	{255, 255, 0},
	{255, 255, 0},
	{0, 255, 0},
	{0, 255, 0},
	{0, 255, 255},
	{0, 255, 255},
	{0, 0, 255},
	{0, 0, 255},
	{153, 0, 255},
	{153, 0, 255},
	{255, 0, 255},
	{255, 0, 255}
};

// Boolean to check if LEDs powered on/off.
uint8_t ledPower						= 1;

// Base RGB color from witch each effect starts. 
uint8_t redLedValue						= 0;
uint8_t greenLedValue					= 0;
uint8_t blueLedValue					= 0;

// RGB color increment or decrement value.
uint8_t redIncrement;
uint8_t greenIncrement;
uint8_t blueIncrement;

// Actual RBG color value.
float redLedCurrentValue;
float greenLedCurrentValue;
float blueLedCurrentValue;

// Target RGB color to be achieved in LedCurrentValue variables.
uint8_t targetRedLedValue;
uint8_t targetGreenLedValue;
uint8_t targetBlueLedValue;

// Speed with witch the RGB color changes from current to target.
uint8_t redEffectSpeed;
uint8_t greenEffectSpeed;
uint8_t blueEffectSpeed;

// Actual effect type.
uint8_t currentEffect					= STATIC;

/* SPECTRUM CYCLING EFFECT
 * ------------------------------
 * This effect loops a RGB color bi-dimensional array, makes a color 
 * transition and then stays in the target color for the same amount of time.
 */

// Duration for single color transition.
const float transitionDuration			= 2000000;
const float transitionDurationSec		= transitionDuration / 1000000;

// Index for accessing RGB color bi-dimensional array.
int8_t spectrumCyclingCount				= -1;

// Boolean to check if the effect needs initialization.
uint8_t spectrumEffectInitialization;

// Time when color transition starts in milliseconds.
uint32_t colorTransitionStart;

// Passed time since color transition started in milliseconds.
uint32_t colorTransitionElapsedTime;
//-------------------------------

/* FADE AND BREATHING EFFECTS.
 * ------------------------------
 * This effect fades in and out for a single color for a specific amount of
 * time. If the breathing variable is TRUE, after the fade out it stays
 * off for a determined amount of time and then repeats. If the breathing
 * variable is FALSE this las part is omitted.
 */

// Duration of fade ins and outs.
const float fadeDuration					= 3000000;
const float fadeDurationSeconds				= fadeDuration / 1000000;

// Duration of the off time for the breathing part effect.
const float offDuration						= 1500000;

// Boolean to check if the effect is fade in/out.
uint8_t fadeIn							= 0;

// Boolean to check if the effect is breathing.
uint8_t breathing						= 0;

// Time when fade in/out starts in milliseconds.
uint32_t fadeStart;

// Passed time since face in/out started in milliseconds.
uint32_t fadeElapsedTime;
//-------------------------------

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
	analogWrite(RED_PIN, redLedValue);
	analogWrite(GREEN_PIN, greenLedValue);
	analogWrite(BLUE_PIN, blueLedValue);

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
			redLedValue = code[1];
			// Writes RED color value into the PIN.
			analogWrite(RED_PIN, redLedValue);
			break;
		case WRITE_GREEN:
			// Updates GREEN base color.
			greenLedValue = code[1];
			// Writes GREEN color value into the PIN.
			analogWrite(GREEN_PIN, greenLedValue);
			break;
		case WRITE_BLUE:
			// Updates BLUE base color.
			blueLedValue = code[1];
			// Writes BLUE color value into the PIN.
			analogWrite(BLUE_PIN, blueLedValue);
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
			Serial.write(redLedValue);
			break;
		case READ_GREEN:
			// Sends GREEN component of the base color.
			Serial.write(greenLedValue);
			break;
		case READ_BLUE:
			// Sends BLUE component of the base color.
			Serial.write(blueLedValue);
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
	redLedCurrentValue		= redLedValue;
	greenLedCurrentValue	= greenLedValue;
	blueLedCurrentValue		= blueLedValue;
}

// This function WRITES into the PINS the current RGB values of the effect.
void updateColor() {
	analogWrite(RED_PIN, redLedCurrentValue);
	analogWrite(GREEN_PIN, greenLedCurrentValue);
	analogWrite(BLUE_PIN, blueLedCurrentValue);
}

// This function updates PIN values and TIME variables.
void reset() {
	updateColor();

	now = micros();
	fadeStart = now;
}

// This function PROCESSES the STATIC EFFECT.
void staticEffect() {
	// Sets current effect to STATIC.
	currentEffect			= STATIC;
	
	// Updates current RGB values with the base color.	
	initializeCurrentColorValues();

	// Updates PINS with current RGB values.
	updateColor();	
}

// This function INITIALIZES the FADE and BREATHING EFFECTS.
void initializeFadeBreathingEffect() {
	// Sets current effect to FADE.
	currentEffect			= FADE;
	// Sets fade in flag as TRUE.
	fadeIn					= 0;
	
	// Updates current RGB values with the base color.
	initializeCurrentColorValues();

	// Calculates the face speed for the base color.
	redEffectSpeed			= ceil(redLedCurrentValue / fadeDurationSeconds);
	greenEffectSpeed		= ceil(greenLedCurrentValue / fadeDurationSeconds);
	blueEffectSpeed			= ceil(blueLedCurrentValue / fadeDurationSeconds);
	
	// IF the EFFECT is BREATHING activates its flag.
	if(code[0] == BREATHING)
		breathing			= 1;
	else
		breathing			= 0;

	// Updates PINS values and TIME variables.
	reset();
}

// This function INITIALIZES the SPECTRUM CYCLING EFFECT.
void initializeSpectrumCyclingEffect() {
	// Sets current effect to SPECTRUM CYCLING.
	currentEffect					= SPECTRUM_CYCLING;
	// Sets initial value of the array access INDEX.
	spectrumCyclingCount			= -1;
	// Sets effect INITIALIZATION flag as TRUE.
	spectrumEffectInitialization	= 1;
	
	// Updates current RGB values with the base color.
	initializeCurrentColorValues();

	// Updates PINS values and TIME variables.
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
	}
}

// This function Check if the input value exceeds the target value.
float colorDownLimiter(float current, uint8_t target) {
	// IF input surpasses the target returns the target value. 
	if(current < target)
		return target;
	// ELSE returns current value.
	return current;
}

// This function Check if the input value exceeds the target value.
float colorUpLimiter(float current, uint8_t target) {
	// IF input surpasses the target returns the target value. 
	if(current > target)
		return target;
	// ELSE returns current value.
	return current;
}

// This function returns a boolean flag to check if the input needs to 
// increment or decrement to reach the target value.
uint8_t isColorIncrement(float current, uint8_t target) {
	if(current < target)
		return 1;
	return 0;
}

// This function calculates the next color transition, depending on the 
// target color, and effect speed.
float colorTranslation(float current, uint8_t target,
	uint8_t effectSpeed)
{
	// IF color needs to increment to reach the target.
	if(isColorIncrement(current, target)) {
		// ADDS relative increment.
		current += effectSpeed * deltaTime;
		// Make correction if target value gets surpassed.
		current = colorUpLimiter(current, target);
	} else {
		// SUBTRACTS relative increment. (decrement)
		current -= effectSpeed * deltaTime;
		// Make correction if target value gets surpassed.
		current = colorDownLimiter(current, target);
	}
	return current;
}

// This function the current effect single iteration.
void processEffect() {
	switch(currentEffect) {
		// Process FADE effect.
		case FADE:
			// Updates fade elapsed time.
			fadeElapsedTime = micros() - fadeStart;
			
			// Processes the fade in/out effect parts depending on the flag.
			if(fadeIn) {
				// Processes the FADE IN effect part.

				// Calculates new color addition after transition.
				redLedCurrentValue = colorTranslation(
					redLedCurrentValue, 255, redEffectSpeed);
				greenLedCurrentValue = colorTranslation(
					greenLedCurrentValue, 255, greenEffectSpeed);
				blueLedCurrentValue = colorTranslation(
					blueLedCurrentValue, 255, blueEffectSpeed);
				
				// IF elapsed time surpasses fade duration.
				if(fadeElapsedTime >= fadeDuration) {
					// Sets the flag to FADE OUT.
					fadeIn = 0;
					// Restarts the face initialization time.
					fadeStart = micros();
				}
			} else {
				// Processes the FADE OUT effect part.

				// Calculates new color subtraction after transition.
				redLedCurrentValue = colorTranslation(
					redLedCurrentValue, 0, redEffectSpeed);
				greenLedCurrentValue = colorTranslation(
					greenLedCurrentValue, 0, greenEffectSpeed);
				blueLedCurrentValue = colorTranslation(
					blueLedCurrentValue, 0, blueEffectSpeed);

				// IF elapsed time surpasses fade duration.
				if(fadeElapsedTime >= fadeDuration) {
					// IF the breathing flag is enabled.
					if(breathing) {
						// Sets current effect to BREATHING.
						currentEffect = BREATHING;
						// Restarts the face initialization time.
						fadeStart = micros();
					} else {
						// ELSE set flag to FADE IN.
						fadeIn = 1;
						// Restarts the face initialization time.
						fadeStart = micros();
					}
				}
			}

			// Updates PINS with current RGB values.
			updateColor();
			break;
		// Process BREATHING effect (fade effect third part)
		case BREATHING:
			// Updates fade elapsed time.
			fadeElapsedTime = micros() - fadeStart;

			// IF elapsed time surpasses off state duration.
			if(fadeElapsedTime >= offDuration) {
				// Sets current effect to FADE.
				currentEffect = FADE;
				// ELSE set flag to FADE IN.
				fadeIn = 1;
				// Restarts the face initialization time.
				fadeStart = micros();
			}
			break;
		// Process SPECTRUM CYCLING effect.
		case SPECTRUM_CYCLING:
			// Updates cycling elapsed time.
			colorTransitionElapsedTime = micros() - colorTransitionStart;

			// IF elapsed time surpasses transitionDuration or initialization
			// flag is enabled.
			if(colorTransitionElapsedTime >= transitionDuration || 
				spectrumEffectInitialization) 
			{
				// Cleans cycling initialization flag.
				spectrumEffectInitialization = 0;
				
				// Updates bi-dimensional array accessing INDEX.
				if(spectrumCyclingCount < COLORS_LENGTH - 1) {
					spectrumCyclingCount++;
				} else {
					// IF INDEX reach array's end re-initializes it.
					spectrumCyclingCount = 0;
				}
				
				// Retrieves RGB color from the bi-dimensional as color targets.
				targetRedLedValue = SPECTRUM_COLORS[spectrumCyclingCount][0];
				targetGreenLedValue = SPECTRUM_COLORS[spectrumCyclingCount][1];
				targetBlueLedValue = SPECTRUM_COLORS[spectrumCyclingCount][2];

				// Calculates the transition speed.
				redEffectSpeed = ceil(
					abs(redLedCurrentValue - targetRedLedValue)
					/ transitionDurationSec);
				greenEffectSpeed = ceil(
					abs(greenLedCurrentValue - targetGreenLedValue)
					/ transitionDurationSec);
				blueEffectSpeed = ceil(
					abs(blueLedCurrentValue - targetBlueLedValue)
					/ transitionDurationSec);

				// Restarts the transition initialization time.
				colorTransitionStart = micros();
			}

			// Calculates new color subtraction after transition.
			redLedCurrentValue = colorTranslation(
				redLedCurrentValue, targetRedLedValue,
				redEffectSpeed);
			
			greenLedCurrentValue = colorTranslation(
				greenLedCurrentValue, targetGreenLedValue,
				greenEffectSpeed);

			blueLedCurrentValue = colorTranslation(
				blueLedCurrentValue, targetBlueLedValue,
				blueEffectSpeed);

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