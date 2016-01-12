namespace LED_Controller
{
    using System;
    using System.Collections.Generic;

    class SerialModel
    {
        public const string DeviceName = "USB-SERIAL CH340 (COM";
        public const string COM = "COM";
        public const short BaudRate = 9600;

        private readonly Dictionary<string, byte> effectList =
            new Dictionary<string, byte>()
            {
                { "Static", Convert.ToByte('K') },
                { "Fading", Convert.ToByte('H') },
                { "Breathing", Convert.ToByte('I') },
                { "Spectrum cycling", Convert.ToByte('J') }
            };

        private short serialPortNumber = -1;

        public Dictionary<string, byte> GetEffectList()
        {
            return this.effectList;
        }

        public short GetSerialPortNumber()
        {
            return this.serialPortNumber;
        }

        public void SetSerialPortNumber(short portNumber)
        {
            this.serialPortNumber = portNumber;
        }

    }
}
