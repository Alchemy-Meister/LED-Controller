namespace LED_Controller
{
    using System;
    using System.Collections.Generic;
    using System.ComponentModel;
    using System.Reflection;

    class SerialModel
    {
        public const string DeviceName = "USB-SERIAL CH340 (COM";
        public const string COM = "COM";
        public const short BaudRate = 9600;

        public enum models : short {LPD8805 = 127, LED5050 = 255};

        public enum handShake { SYN = 0x01, ACK = 0xFF};

        private readonly Dictionary<string, byte> statusList =
           new Dictionary<string, byte>()
           {
                { "On", Convert.ToByte('N') },
                { "Off", Convert.ToByte('F') }
           };

        public enum effectNames {
            [Description("Static")] STATIC,
            [Description("Fade")] FADE,
            [Description("Breath")] BREATH,
            [Description("Flash")] FLASH,
            [Description("Double flash")] DOUBLE_FLASH,
            [Description("Spectrum cycle")] SPECTRUM_CYCLE,
            [Description("Rainbow spin")] RAINBOW_SPIN
        };

        private readonly Dictionary<string, byte[]> effectList =
            new Dictionary<string, byte[]>()
            {
                { GetEffectName(effectNames.STATIC), new byte[] { Convert.ToByte('K'), 1 } },
                { GetEffectName(effectNames.FADE), new byte[] { Convert.ToByte('H'), 1 } },
                { GetEffectName(effectNames.BREATH), new byte[] { Convert.ToByte('I'), 1 } },
                { GetEffectName(effectNames.FLASH), new byte[] { Convert.ToByte('L'), 1 } },
                { GetEffectName(effectNames.DOUBLE_FLASH), new byte[] { Convert.ToByte('M'), 1 } },
                { GetEffectName(effectNames.SPECTRUM_CYCLE), new byte[] { Convert.ToByte('J'), 1 } },
                { GetEffectName(effectNames.RAINBOW_SPIN), new byte[] { Convert.ToByte('O'), 0 } }
            };

        private bool ledPower = true;

        private byte redLedValue = 0;
        private byte greenLedValue = 0;
        private byte blueLedValue = 0;

        private short serialPortNumber = -1;

        private short? model = null;

        public bool GetLedPower()
        {
            return this.ledPower;
        }

        public void SetLedPower(bool power)
        {
            this.ledPower = power;
        }
        
        public static string GetEffectName(Enum value)
        {
            FieldInfo fi = value.GetType().GetField(value.ToString());

            DescriptionAttribute[] attributes =
                (DescriptionAttribute[]) fi.GetCustomAttributes(typeof(DescriptionAttribute), false);

            if (attributes != null && attributes.Length > 0)
                return attributes[0].Description;
            else
                return value.ToString();
        }

        public Dictionary<string, byte> GetEffectList(models model)
        {
            Dictionary<string, byte> returnDict = new Dictionary<string, byte>();
            switch (model)
            {
                case models.LPD8805:
                    foreach (KeyValuePair<string, byte[]> entry in effectList)
                    {
                        returnDict.Add(entry.Key, entry.Value[0]);
                    }
                    break;
                case models.LED5050:
                    foreach (KeyValuePair<string, byte[]> entry in effectList)
                    {
                        if(entry.Value[1].Equals(Convert.ToByte(1)))
                        {
                            returnDict.Add(entry.Key, entry.Value[0]);
                        }
                    }
                    break;
            }
            return returnDict;
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

        public void SetModel(models model)
        {
            this.model = (short) model;
        }

        public short? GetModel()
        {
            return this.model;
        }
    }
}
