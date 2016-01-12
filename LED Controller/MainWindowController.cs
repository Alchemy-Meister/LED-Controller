namespace LED_Controller
{
    using System;
    using System.Collections.Generic;
    using System.IO.Ports;
    using System.Management;

    class MainWindowController
    {
        private SerialPort serialPort;
        private SerialModel serialModel;

        public MainWindowController()
        {
            this.serialModel = new SerialModel();
        }

        public void InitializeSerialPort()
        {
            this.serialPort = new SerialPort(
                SerialModel.COM + this.serialModel.GetSerialPortNumber(),
                SerialModel.BaudRate);
            this.serialPort.ReadTimeout = 100;
            this.serialPort.WriteTimeout = 100;
        }

        // Opens the serial port connection if is not already.
        public void OpenSerialPort()
        {
            if (!this.serialPort.IsOpen)
            {
                this.serialPort.Open();
            }
        }

        public void CloseSerialPort()
        {
            this.serialPort.Close();
        }

        public bool IsSerialPortOpened()
        {
            return this.serialPort != null ? this.serialPort.IsOpen : false;
        }

        public Dictionary<string, byte> GetEffectList()
        {
            return this.serialModel.GetEffectList();
        }

        public short GetSerialPort()
        {
            return this.serialModel.GetSerialPortNumber();
        }
        
        // Function queries the system using WMI and returns the serial port number if the device 
        // is connected to the system, -1 otherwise.   
        public short DeviceUpdatedSerialPort()
        {
            this.serialModel.SetSerialPortNumber(-1);

            // Getting a list of all available com port devices and their friendly names     
            // must add System.Management DLL resource to solution before using this     
            // Project -> Add Reference -> .Net tab, choose System.Management  
            ManagementObjectSearcher searcher = new ManagementObjectSearcher(
                "root\\CIMV2",
                "Select Name from Win32_PnpEntity where ClassGuid=\"{4d36e978-e325-11ce-bfc1-08002be10318}\"");
            int length = searcher.Get().Count;
            if (length > 0)
            {
                ManagementObject[] deviceArray = new ManagementObject[searcher.Get().Count];
                searcher.Get().CopyTo(deviceArray, 0);
                bool found = false;
                for (int index = 0; !found && index < deviceArray.Length; index++)
                {
                    string name = deviceArray[index].GetPropertyValue("Name").ToString();

                    // Checks if the current device is the desired. 
                    if (name.Contains(SerialModel.DeviceName))
                    {
                        found = true;
                        short startIndex = (short)(name.LastIndexOf("M") + 1);
                        short endIndex = (short)name.LastIndexOf(")");
                        
                        // The maximum number of COM and LPT ports that Windows NT supports is 256
                        this.serialModel.SetSerialPortNumber(
                            Convert.ToInt16(name.Substring(startIndex, endIndex - startIndex)));
                    }
                }

                searcher.Dispose();
            }

            return this.serialModel.GetSerialPortNumber();
        }

        public void SendWriteMessage(byte command, byte value)
        {
            if (this.serialPort != null && this.serialPort.IsOpen)
            {
                try
                {
                    byte[] message = new byte[3];
                    message[0] = command;
                    message[1] = value;
                    message[2] = Convert.ToByte('\n');
                    this.serialPort.Write(message, 0, message.Length);
                }
                catch (TimeoutException)
                {
                }
            }
        }

        public string SendReadMessage(byte command)
        {
            if (this.serialPort != null && this.serialPort.IsOpen)
            {
                try
                {
                    byte[] message = new byte[2];
                    message[0] = command;
                    message[1] = Convert.ToByte('\n');
                    this.serialPort.Write(message, 0, message.Length);

                    byte[] response = new byte[1];
                    this.serialPort.Read(response, 0, 1);
                    return response[0].ToString();
                }
                catch (TimeoutException)
                {
                    return null;
                }
            }
            else
            {
                return null;
            }
        }
    }
}
