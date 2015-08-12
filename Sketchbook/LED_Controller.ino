// LED_Controller.ino

const uint8_t redLed = 3;
const uint8_t greenLed = 5;
const uint8_t blueLed = 6;

uint8_t redLedValue = 0;
uint8_t greenLedValue = 0;
uint8_t blueLedValue = 0;

uint8_t code[2];
uint8_t index = 0;

void setup() {
	const uint16_t rate = 9600;

	pinMode(redLed, OUTPUT);
	pinMode(greenLed, OUTPUT);
	pinMode(blueLed, OUTPUT);

	analogWrite(redLed, redLedValue);
	analogWrite(greenLed, greenLedValue);
	analogWrite(blueLed, blueLedValue);

	Serial.begin(rate);
}

void writeOnPin(char pin) {
	switch(pin) {
		case 'R':
			redLedValue = code[1];
			analogWrite(redLed, redLedValue);
			break;
		case 'G':
			greenLedValue = code[1];
			analogWrite(greenLed, greenLedValue);
			break;
		case 'B':
			blueLedValue = code[1];
			analogWrite(blueLed, blueLedValue);
			break;
	}
}

void readPin(char pin) {
	switch(pin) {
	    case 'C':
	    	Serial.write(redLedValue);
	     	break;
	    case 'D':
	    	Serial.write(greenLedValue);
	    	break;
	    case 'E':
	    	Serial.write(blueLedValue);
	    	break;
	}
}

void process(void) {
	switch(code[0]){
		//Writes LED intensity value on redLed (R), greenLed (G) or blueLed (B) pin.
		case 'R': case 'G': case 'B':
			writeOnPin(code[0]);
			break;
		//Sends current value of redLedValue (C), greenLedValue (D) or blueLedValue (E) over serial.
		case 'C': case 'D': case 'E':
			readPin(code[0]);
			break;
	}
}

void loop() {
	if(Serial.available()) {
		uint8_t serialIn = Serial.read();
		if(serialIn != '\n') {
			code[index] = serialIn;
			index++;
		} else {
			index = 0;
			process();
		}
	}
}