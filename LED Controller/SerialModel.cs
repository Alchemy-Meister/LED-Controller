namespace LED_Controller
{
    using System;
    using System.Collections.Generic;

    class SerialModel
    {
        public const string DeviceName = "USB-SERIAL CH340 (COM";
        public const string COM = "COM";
        public const short BaudRate = 9600;

        private readonly Dictionary<string, byte> statusList =
           new Dictionary<string, byte>()
           {
                { "On", Convert.ToByte('N') },
                { "Off", Convert.ToByte('F') }
           };

        private readonly Dictionary<string, byte> effectList =
            new Dictionary<string, byte>()
            {
                { "Static", Convert.ToByte('K') },
                { "Fade", Convert.ToByte('H') },
                { "Breath", Convert.ToByte('I') },
                { "Flash", Convert.ToByte('L') },
                { "Double Flash", Convert.ToByte('M') },
                { "Spectrum cycle", Convert.ToByte('J') }
            };

        private bool ledPower = true;

        private byte redLedValue = 0;
        private byte greenLedValue = 0;
        private byte blueLedValue = 0;

        private short serialPortNumber = -1;

        public bool GetLedPower()
        {
            return this.ledPower;
        }

        public void SetLedPower(bool power)
        {
            this.ledPower = power;
        }

        public Dictionary<string, byte> GetEffectList()
        {
            return this.effectList;
        }

        public Dictionary<string, byte> GetStatusList()
        {
            return this.statusList;
        }

        public byte GetRedLedValue()
        {
            return this.redLedValue;
        }

        public void SetRedLedValue(byte value)
        {
            this.redLedValue = value;
        }

        public byte GetGreenLedValue()
        {
            return this.greenLedValue;
        }

        public void SetGreenLedValue(byte value)
        {
            this.greenLedValue = value;
        }

        public byte GetBlueLedValue()
        {
            return this.blueLedValue;
        }

        public void SetBlueLedValue(byte value)
        {
            this.blueLedValue = value;
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
