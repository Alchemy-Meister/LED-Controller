#include <Arduino.h>
#include <SPI.h>

// LED_Controller.ino

#include "color/color.h"
#include "color/float_color.h"
#include "commands/commands.h"
#include "effects/fade/fade.h"
#include "effects/flash/flash.h"
#include "effects/rainbow_spin/rainbow_spin.h"
#include "effects/spectrum_cycle/spectrum_cycle.h"

// Declares the RGB PINS for Arduino
const uint8_t RED_PIN   = 3;
const uint8_t GREEN_PIN = 5;
const uint8_t BLUE_PIN  = 6;

// SD Chip Select Pin.
// const uint8_t SDCS_PIN = 4;
// const char CONF_FILENAME[] = "Config";

// LPD8806 needs to be connected into SPI ports.

// Timeout to
const uint32_t handShakeTime = 1000000;

// Boolean to check if serial authentication has been established.
uint8_t handShakeEstablished = 0;
uint8_t synCheck = 0;
uint32_t handShakeStart;

// Boolean to check if LEDs powered on/off.
uint8_t ledPower = 1;

// Loads and Saves configuration to SD card.
uint8_t externalStorage = 1;

// Initialize settings with previosly saved configuration.
uint8_t loadSettings = 1;

// Boolean to check the use of the LPD8806 library.
uint8_t individualLEDController = 1;

// Number of LEDs connected to the strip.
uint8_t ledNumber = 90;

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
uint8_t currentEffect = Commands::STATIC;

// Actual time in milliseconds.
uint32_t now;

// Time since the last iteration was executed.
uint32_t lastTime;

// Elapsed time between actual time and last iteration.
float deltaTime;

// Byte array where the COMMAND is stored.
uint8_t code[3];

// Index to access the COMMAND array.
uint8_t index = 0;

// void firstTimeInitialization() {
//     currentEffect = Commands::STATIC;
// }

//Arduino's initialization
void setup() {
    // Sets serial communication's baud rate.
    const uint16_t rate = 9600;

    // Configure RGB pins as outputs.
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);

    // Defines if Single LED Controller is used to render effects.
    fade.enableSingleLEDController(individualLEDController);
    spectrumCycling.enableSingleLEDController(individualLEDController);
    flash.enableSingleLEDController(individualLEDController);

    // Start up the LED strip
    strip.begin();

    // Initializes RGB color into the pins.
    updateColor(baseColor);

    // Starts serial connection.
    Serial.begin(rate);

    // Initializes actual time.
    now = micros();
    deltaTime = 0;
}

// WRITES values into RGB pins depending on the received COMMAND.
void writeOnPin() {
    switch(code[0]) {
        case Commands::WRITE_RED:
            // Updates RED base color.
            baseColor.setRed(code[1]);
            // Writes RED color value into the PIN.
            analogWrite(RED_PIN, code[1]);
            break;
        case Commands::WRITE_GREEN:
            // Updates GREEN base color.
            baseColor.setGreen(code[1]);
            // Writes GREEN color value into the PIN.
            analogWrite(GREEN_PIN, code[1]);
            break;
        case Commands::WRITE_BLUE:
            // Updates BLUE base color.
            baseColor.setBlue(code[1]);
            // Writes BLUE color value into the PIN.
            analogWrite(BLUE_PIN, code[1]);
            break;
    }
}

// SENDS the values of the base RGB color to the SERIAL.
void readPin() {
    switch(code[0]) {
        case Commands::READ_RED:
            // Sends RED component of the base color.
            Serial.write(baseColor.getRed());
            break;
        case Commands::READ_GREEN:
            // Sends GREEN component of the base color.
            Serial.write(baseColor.getGreen());
            break;
        case Commands::READ_BLUE:
            // Sends BLUE component of the base color.
            Serial.write(baseColor.getBlue());
            break;
    }
}

// Restarts the effect and resumes the effect process.
void turnOnProcess() {
    // Activates the POWER ON flag to resume effect procedure.
    ledPower = 1;
    // Initializes the current effect.
    switch (currentEffect) {
        case Commands::BREATHING: case Commands::FADE:
            initializeFadeBreathingEffect();
            break;
        case Commands::RAINBOW_SPIN:
            initializeRainbowSpinEffect();
            break;
        case Commands::SPECTRUM_CYCLING:
            initializeSpectrumCyclingEffect();
            break;
        case Commands::STATIC:
            staticEffect();
            break;
    }
}

// Turns off the strip and stop effect process.
void turnOffProcess() {
    // Sets POWER flag to FALSE.
    ledPower = 0;
    // Writes 0 value into the PINs to turn off the LEDs.
    updateColor(Color());
}

// Sets current RGB values with the base color.
void initializeCurrentColorValues() {
    currentColor.setColors(baseColor);
}

// WRITES into the PINS the RGB values of the color passed.
void updateColor(const FloatColor updateColor) {
    if(individualLEDController){
        for(uint16_t i = 0; i < strip.numPixels(); i++) {
            strip.setPixelColor(
                i,
                updateColor.getRed(),
                updateColor.getGreen(),
                updateColor.getBlue()
            );
        }
        strip.show();
    } else {
        analogWrite(RED_PIN, updateColor.getRed());
        analogWrite(GREEN_PIN, updateColor.getGreen());
        analogWrite(BLUE_PIN, updateColor.getBlue());
    }
}

// Updates PIN values and TIME variables.
void reset() {
    now = micros();
    fade.setStartTime(now);
    flash.setStartTime(now);
}

// PROCESSES the STATIC EFFECT.
void staticEffect() {
    // Sets current effect to STATIC.
    currentEffect = Commands::STATIC;

    // Updates current RGB values with the base color.
    initializeCurrentColorValues();

    // Updates PINS with current RGB values.
    updateColor(currentColor);
}

// INITIALIZES the FADE and BREATHING EFFECTS.
void initializeFadeBreathingEffect() {
    // Sets current effect to FADE.
    currentEffect = Commands::FADE;

    // Updates current RGB values with the base color.
    initializeCurrentColorValues();

    fade.initializeEffect(currentColor, code[0] == Commands::BREATHING);

    // Updates PINS values and TIME variables.
    fade.updateStripColor(Color(currentColor), RED_PIN, GREEN_PIN, BLUE_PIN);
    reset();
}

// INITIALIZES the SPECTRUM CYCLING EFFECT.
void initializeSpectrumCyclingEffect() {
    // Sets current effect to SPECTRUM CYCLING.
    currentEffect = Commands::SPECTRUM_CYCLING;
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
    currentEffect = Commands::FLASH;

    flash.initializeEffect(baseColor, code[0] == Commands::DOUBLE_FLASH);

    flash.updateStripColor(Color(currentColor), RED_PIN, GREEN_PIN, BLUE_PIN);
    reset();
}

void initializeRainbowSpinEffect() {
    currentEffect = Commands::RAINBOW_SPIN;

    rainbowSpin.initializeEffect();
    rainbowSpin.processEffect();

    reset();
}

float speedComToFloat() {
    switch(code[1]) {
        case Commands::TRIPLE:
            return 3;
        case Commands::DOUBLE:
            return 2;
        case Commands::NORMAL:
            return 1;
        case Commands::HALF:
            return 0.5;
        case Commands::THIRD:
            return 0.333;
        default:
            return 0;
    }
}

void processSpeed() {
    float speedCom = speedComToFloat();
    switch(currentEffect) {
        case Commands::FADE: case Commands::BREATHING:
            fade.setSpeed(speedCom);
            break;
        case Commands::SPECTRUM_CYCLING:
            spectrumCycling.setSpeed(speedCom);
            break;
        case Commands::FLASH: case Commands::DOUBLE_FLASH:
            flash.setSpeed(speedCom);
            break;
    }
}

void processHandShake() {
    if(micros() - handShakeStart >= handShakeTime) {
        synCheck = 0;
    }
    switch (code[0]) {
        case Commands::SYN_IN:
            synCheck = 1;
            Serial.write(Commands::ACK_IN);
            handShakeStart = micros();
            break;
        case Commands::ACK_IN:
            if(synCheck) {
                handShakeEstablished = 1;
            }
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

void processSense() {
    switch(currentEffect) {
        case Commands::RAINBOW_SPIN:
            if(code[1] == Commands::CLOCKWISE) {
                rainbowSpin.setSense(1);
            } else {
                rainbowSpin.setSense(0);
            }
            break;
    }
}

// Processes the COMMAND received from SERIAL.
void process() {
    switch(code[0]){
        case Commands::SYN_IN:
            processHandShake();
            break;
        case Commands::MODEL:
            processModel();
            break;
        case Commands::TURN_ON:
            turnOnProcess();
            break;
        case Commands::TURN_OFF:
            turnOffProcess();
            break;
        case Commands::WRITE_RED: case Commands::WRITE_GREEN:
        case Commands::WRITE_BLUE:
            writeOnPin();
            break;
        case Commands::READ_RED: case Commands::READ_GREEN:
        case Commands::READ_BLUE:
            readPin();
            break;
        case Commands::BREATHING: case Commands::FADE:
            initializeFadeBreathingEffect();
            break;
        case Commands::SPECTRUM_CYCLING:
            initializeSpectrumCyclingEffect();
            break;
        case Commands::STATIC:
            staticEffect();
            break;
        case Commands::FLASH: case Commands::DOUBLE_FLASH:
            initializeFlashingEffect();
            break;
        case Commands::RAINBOW_SPIN:
            initializeRainbowSpinEffect();
            break;
        case Commands::SPEED:
            processSpeed();
            break;
        case Commands::SENSE:
            processSense();
            break;
    }
}

// Processes a single iteration of current effect.
void processEffect() {
    switch(currentEffect) {
        // Process FADE effect.
        case Commands::FADE:
            fade.processEffect(currentColor, deltaTime);
            fade.updateStripColor(
                Color(currentColor), RED_PIN, GREEN_PIN, BLUE_PIN
            );
            break;
        case Commands::FLASH:
            flash.processEffect(currentColor);
            flash.updateStripColor(
                Color(currentColor), RED_PIN, GREEN_PIN, BLUE_PIN
            );
            break;
        // Process SPECTRUM CYCLING effect.
        case Commands::SPECTRUM_CYCLING:
            spectrumCycling.processEffect(
                currentColor, deltaTime);
            fade.updateStripColor(
                Color(currentColor), RED_PIN, GREEN_PIN, BLUE_PIN
            );
            break;
        case Commands::RAINBOW_SPIN:
            rainbowSpin.processEffect();
            break;
        case Commands::STATIC:
            staticEffect();
            // Updates PINS with current RGB values.
            updateColor(currentColor);
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
