// LED_Controller.ino

const uint8_t RED_PIN = 3;
const uint8_t GREEN_PIN = 5;
const uint8_t BLUE_PIN = 6;

const uint8_t WRITE_RED = 'R';
const uint8_t WRITE_GREEN = 'G';
const uint8_t WRITE_BLUE = 'B';

const uint8_t READ_RED = 'C';
const uint8_t READ_GREEN = 'D';
const uint8_t READ_BLUE = 'E';

const uint8_t BREATHING = 'H';
const uint8_t SPECTRUM_CYCLING = 'I';
const uint8_t STATIC = 'J';

uint8_t redLedValue = 0;
float redLedCurrentValue;
uint8_t redLedFadeSpeed;		//Speed equals the RGB color divided by the duration of the animation in seconds.

uint8_t greenLedValue = 0;
float greenLedCurrentValue;
uint8_t greenLedFadeSpeed;

uint8_t blueLedValue = 0;
float blueLedCurrentValue;
uint8_t blueLedFadeSpeed;

uint8_t currentEffect = STATIC;

uint8_t fadeIn = 0;
float fadeDuration = 2000000.0; 			//Time of the fade animation in microseconds.
float fadeDurationSeconds = fadeDuration / 1000000;
unsigned long fadeStart;
unsigned long fadeElapsedTime;

unsigned long now;
unsigned long lastTime;
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

void writeOnPin(char pin) {
	switch(pin) {
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
	}
}

void readPin(char pin) {
	switch(pin) {
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

void reset() {

	fadeIn = 0;

	analogWrite(RED_PIN, redLedCurrentValue);
	analogWrite(GREEN_PIN, greenLedCurrentValue);
	analogWrite(BLUE_PIN, blueLedCurrentValue);

	now = micros();
	fadeStart = now;
}

void process(void) {
	switch(code[0]){
		case WRITE_RED: case WRITE_GREEN: case WRITE_BLUE:
			writeOnPin(code[0]);
			break;
		case READ_RED: case READ_GREEN: case READ_BLUE:
			readPin(code[0]);
			break;
		case BREATHING: case SPECTRUM_CYCLING: case STATIC:
			currentEffect = code[0];
			
			redLedCurrentValue = redLedValue;
			greenLedCurrentValue = greenLedValue;
			blueLedCurrentValue = blueLedValue;

			redLedFadeSpeed = redLedCurrentValue / fadeDurationSeconds;
			greenLedFadeSpeed = greenLedCurrentValue / fadeDurationSeconds;
			blueLedFadeSpeed = blueLedCurrentValue / fadeDurationSeconds;
			
			reset();
			break;
	}
}

void processEffect(void) {
	switch(currentEffect) {
		case BREATHING:
			fadeElapsedTime = micros() - fadeStart;
			if(fadeIn) {
				redLedCurrentValue += redLedFadeSpeed * deltaTime;
				greenLedCurrentValue += greenLedFadeSpeed * deltaTime;
				blueLedCurrentValue += blueLedFadeSpeed * deltaTime;
				if(fadeElapsedTime >= fadeDuration) {
					fadeIn = 0;
					fadeStart = micros();
				}
			} else {
				redLedCurrentValue -= redLedFadeSpeed * deltaTime;
				greenLedCurrentValue -= greenLedFadeSpeed * deltaTime;
				blueLedCurrentValue -= blueLedFadeSpeed * deltaTime;
				if(fadeElapsedTime >= fadeDuration) {
					fadeIn = 1;
					fadeStart = micros();
				}
			}
			analogWrite(RED_PIN, redLedCurrentValue);
			analogWrite(GREEN_PIN, greenLedCurrentValue);
			analogWrite(BLUE_PIN, blueLedCurrentValue);
			break;
		case SPECTRUM_CYCLING:
			
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
	processEffect();
}