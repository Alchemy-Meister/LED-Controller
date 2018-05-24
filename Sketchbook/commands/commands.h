#ifndef COMMANDS_H
#define COMMANDS_H

class Commands
{
public:
    // Declares HANDSHAKE COMMANDS
    enum HANDSHAKE {
        SYN_IN           = 0x01,
        ACK_IN           = 0xFF
    };

    // Declares MODEL COMMAND
    enum MODEL {
        MODEL          = 'P'
    };

    // Declares the POWER COMMANDS
    enum POWER {
        TURN_ON          = 'N',
        TURN_OFF         = 'F'
    };

    // Declares the RGB WRITE COMMANDS
    enum WRITE {
        WRITE_RED        = 'R',
        WRITE_GREEN      = 'G',
        WRITE_BLUE       = 'B'
    };

    // Declares the RGB READ COMMANDS
    enum READ {
        READ_RED         = 'C',
        READ_GREEN       = 'D',
        READ_BLUE        = 'E'
    };

    // Declares the EFFECTS COMMANDS
    enum EFFECT {
        FADE             = 'H',
        BREATHING        = 'I',
        SPECTRUM_CYCLING = 'J',
        STATIC           = 'K',
        FLASH            = 'L',
        DOUBLE_FLASH     = 'M',
        RAINBOW_SPIN     = 'O'
    };

    // Declares the EFFECT SPEED COMMANDS
    enum SPEED {
        SPEED            = 'S',
        TRIPLE           = 'T',
        DOUBLE           = 'U',
        NORMAL           = 'V',
        HALF             = 'W',
        THIRD            = 'X'
    };

    // Declared the EFFECT SENSE COMMAND
    enum SENSE {
        SENSE            = 'Q',
        CLOCKWISE        = 0x01,
    };
};

#endif
