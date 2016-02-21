// LED_Controller.ino

#include <math.h>

const uint8_t RED_PIN					= 3;
const uint8_t GREEN_PIN					= 5;
const uint8_t BLUE_PIN					= 6;

const uint8_t WRITE_RED					= 'R';
const uint8_t WRITE_GREEN				= 'G';
const uint8_t WRITE_BLUE				= 'B';

const uint8_t READ_RED					= 'C';
const uint8_t READ_GREEN 				= 'D';
const uint8_t READ_BLUE 				= 'E';

const uint8_t TURN_ON					= 'N';
const uint8_t TURN_OFF					= 'F';

const uint8_t FADE						= 'H';
const uint8_t BREATHING					= 'I';
const uint8_t SPECTRUM_CYCLING			= 'J';
const uint8_t STATIC					= 'K';

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

uint8_t ledPower						= 1;

uint8_t redLedValue						= 0;
uint8_t greenLedValue					= 0;
uint8_t blueLedValue					= 0;

uint8_t redIncrement;
uint8_t greenIncrement;
uint8_t blueIncrement;

float redLedCurrentValue;
float greenLedCurrentValue;
float blueLedCurrentValue;

uint8_t targetRedLedValue;
uint8_t targetGreenLedValue;
uint8_t targetBlueLedValue;

uint8_t redEffectSpeed;
uint8_t greenEffectSpeed;
uint8_t blueEffectSpeed;

uint8_t currentEffect					= STATIC;

int8_t spectrumCyclingCount				= -1;
float transitionDuration				= 2000000;
float transitionDurationSec				= transitionDuration / 1000000;
uint8_t spectrumEffectInitialization;
uint32_t colorTransitionStart;
uint32_t colorTransitionElapsedTime;

uint8_t fadeIn							= 0;
uint8_t breathing						= 0;
float fadeDuration						= 3000000;
float offDuration						= 1500000;
float fadeDurationSeconds				= fadeDuration / 1000000;
uint32_t fadeStart;
uint32_t fadeElapsedTime;

uint32_t now;
uint32_t lastTime;
float deltaTime;

uint8_t code[2];
uint8_t index = 0;

void setup() {
	const uint16_t rate = 9600;

	pinMode(RED_PIN, OUTPUT);
	pinMode(GREEN_PIN, OUTPUT);
	pinMode(BLUE_PIN, OUTPUT);

	analogWrite(RED_PIN, redLedValue);
	analogWrite(GREEN_PIN, greenLedValue);
	analogWrite(BLUE_PIN, blueLedValue);

	Serial.begin(rate);

	now = micros();
}

void writeOnPin() {
	switch(code[0]) {
		case WRITE_RED:
			redLedValue = code[1];
			analogWrite(RED_PIN, redLedValue);
			break;
		case WRITE_GREEN:
			greenLedValue = code[1];
			analogWrite(GREEN_PIN, greenLedValue);
			break;
		case WRITE_BLUE:
			blueLedValue = code[1];
			analogWrite(BLUE_PIN, blueLedValue);
			break;
		case TURN_OFF:
			ledPower = 0;
			analogWrite(RED_PIN, 0);
			analogWrite(GREEN_PIN, 0);
			analogWrite(BLUE_PIN, 0);
			break;
	}
}

void readPin() {
	switch(code[0]) {
		case READ_RED:
			Serial.write(redLedValue);
			break;
		case READ_GREEN:
			Serial.write(greenLedValue);
			break;
		case READ_BLUE:
			Serial.write(blueLedValue);
			break;
	}
}

void turnOnProcess() {
	ledPower = 1;
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

void updateColor() {
	analogWrite(RED_PIN, redLedCurrentValue);
	analogWrite(GREEN_PIN, greenLedCurrentValue);
	analogWrite(BLUE_PIN, blueLedCurrentValue);
}

void reset() {
	updateColor();

	now = micros();
	fadeStart = now;
}

void staticEffect() {
	currentEffect			= STATIC;
			
	redLedCurrentValue		= redLedValue;
	greenLedCurrentValue	= greenLedValue;
	blueLedCurrentValue		= blueLedValue;

	updateColor();	
}

void initializeFadeBreathingEffect() {
	currentEffect			= FADE;
	fadeIn					= 0;
	
	redLedCurrentValue		= redLedValue;
	greenLedCurrentValue	= greenLedValue;
	blueLedCurrentValue		= blueLedValue;

	redEffectSpeed			= ceil(redLedCurrentValue / fadeDurationSeconds);
	greenEffectSpeed		= ceil(greenLedCurrentValue / fadeDurationSeconds);
	blueEffectSpeed			= ceil(blueLedCurrentValue / fadeDurationSeconds);
	
	if(code[0] == FADE)
		breathing			= 0;
	else
		breathing			= 1;

	reset();
}

void initializeSpectrumCyclingEffect() {
	currentEffect					= SPECTRUM_CYCLING;
	spectrumCyclingCount			= -1;
	spectrumEffectInitialization	= 1;
	
	redLedCurrentValue				= redLedValue;
	greenLedCurrentValue			= greenLedValue;
	blueLedCurrentValue				= blueLedValue;

	reset();
}

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

float colorDownLimiter(float current, uint8_t target) {
	if(current < target)
		return target;
	return current;
}

float colorUpLimiter(float current, uint8_t target) {
	if(current > target)
		return target;
	return current;
}

uint8_t isColorIncrement(float current, uint8_t target) {
	if(current < target)
		return 1;
	return 0;
}

float colorTranslation(float current, uint8_t target,
	uint8_t effectSpeed, uint8_t increment)
{
	if(increment) {
		current += effectSpeed * deltaTime;
		if(current > target) {
			current = target;
		}
	} else {
		current -= effectSpeed * deltaTime;
		if(current < target) {
			current = target;
		}
	}
	return current;
}

void processEffect() {
	switch(currentEffect) {
		case FADE:
			fadeElapsedTime = micros() - fadeStart;
			
			if(fadeIn) {
				redLedCurrentValue += redEffectSpeed * deltaTime;
				greenLedCurrentValue += greenEffectSpeed * deltaTime;
				blueLedCurrentValue += blueEffectSpeed * deltaTime;
				
				redLedCurrentValue = colorUpLimiter(redLedCurrentValue, 255);
				greenLedCurrentValue = colorUpLimiter(
					greenLedCurrentValue, 255);
				blueLedCurrentValue = colorUpLimiter(blueLedCurrentValue, 255);

				if(fadeElapsedTime >= fadeDuration) {
					fadeIn = 0;
					fadeStart = micros();
				}
			} else {
				redLedCurrentValue -= redEffectSpeed * deltaTime;
				greenLedCurrentValue -= greenEffectSpeed * deltaTime;
				blueLedCurrentValue -= blueEffectSpeed * deltaTime;
				
				redLedCurrentValue = colorDownLimiter(redLedCurrentValue, 0);
				greenLedCurrentValue = colorDownLimiter(
					greenLedCurrentValue, 0);
				blueLedCurrentValue = colorDownLimiter(blueLedCurrentValue, 0);

				if(fadeElapsedTime >= fadeDuration) {
					if(breathing) {
						currentEffect = BREATHING;
						fadeStart = micros();
					} else {
						fadeIn = 1;
						fadeStart = micros();
					}
				}
			}

			updateColor();
			break;
		case BREATHING:
			fadeElapsedTime = micros() - fadeStart;

			if(fadeElapsedTime >= offDuration) {
				currentEffect = FADE;
				fadeIn = 1;
				fadeStart = micros();
			}
			break;
		case SPECTRUM_CYCLING:
			colorTransitionElapsedTime = micros() - colorTransitionStart;

			if(colorTransitionElapsedTime >= transitionDuration || 
				spectrumEffectInitialization) 
			{
				spectrumEffectInitialization = 0;
				
				if(spectrumCyclingCount < COLORS_LENGTH - 1) {
					spectrumCyclingCount++;
				} else {
					spectrumCyclingCount = 0;
				}
				
				targetRedLedValue = SPECTRUM_COLORS[spectrumCyclingCount][0];
				targetGreenLedValue = SPECTRUM_COLORS[spectrumCyclingCount][1];
				targetBlueLedValue = SPECTRUM_COLORS[spectrumCyclingCount][2];

				redIncrement = isColorIncrement(
					redLedCurrentValue, targetRedLedValue);
				greenIncrement = isColorIncrement(
					greenLedCurrentValue, targetGreenLedValue);
				blueIncrement = isColorIncrement(
					blueLedCurrentValue, targetBlueLedValue);

				redEffectSpeed = ceil(
					abs(redLedCurrentValue - targetRedLedValue)
					/ transitionDurationSec);
				greenEffectSpeed = ceil(
					abs(greenLedCurrentValue - targetGreenLedValue)
					/ transitionDurationSec);
				blueEffectSpeed = ceil(
					abs(blueLedCurrentValue - targetBlueLedValue)
					/ transitionDurationSec);

				colorTransitionStart = micros();
			}

			redLedCurrentValue = colorTranslation(
				redLedCurrentValue, targetRedLedValue,
				redEffectSpeed, redIncrement);
			
			greenLedCurrentValue = colorTranslation(
				greenLedCurrentValue, targetGreenLedValue,
				greenEffectSpeed, greenIncrement);

			blueLedCurrentValue = colorTranslation(
				blueLedCurrentValue, targetBlueLedValue,
				blueEffectSpeed, blueIncrement);

			updateColor();
			break;
	}
}

void loop() {
	while(Serial.available()) {
		uint8_t serialIn = Serial.read();
		if(serialIn != '\n') {
			code[index] = serialIn;
			index++;
		} else {
			index = 0;
			process();
		}
	}

	lastTime = now;
	now = micros();
	deltaTime = (now - lastTime) / 1000000.0;
	if(ledPower) {
		processEffect();
	}
}