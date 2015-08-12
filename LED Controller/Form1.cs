using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;
using System.Management;
using System.Runtime.InteropServices;
using System.Security.Permissions;

namespace LED_Controller
{

    public partial class Form1 : Form
    {

        private const int WM_DEVICECHANGE = 0x0219;                 // device change event      
        private const int DBT_DEVICEARRIVAL = 0x8000;               // system detected a new device      
        private const int DBT_DEVICEREMOVECOMPLETE = 0x8004;        // device is gone

        private const int DBT_DEVTYP_PORT = 0x00000003;             // serial, parallel  

        private const Int16 BAUDRATE = 9600;
        private const string DEVICE_NAME = "USB-SERIAL CH340 (COM";

        private bool deviceConnected = false;

        private deviceSerialPortDelegate mDeleg;
        private int serialPortNumber;

        public Form1()
        {
            InitializeComponent();
            InitializeSerialPort();
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void redTrackBar_ValueChanged(object sender, EventArgs e)
        {
            if (serialPort != null && serialPort.IsOpen)
            {
                try
                {
                    Byte[] command = new Byte[3];
                    Byte value = (Byte)Math.Round(redTrackBar.Value * 2.55);
                    command[0] = Convert.ToByte('R');
                    command[1] = value;
                    command[2] = Convert.ToByte('\n');
                    serialPort.Write(command, 0, command.Length);
                    Byte[] redLedValue = new Byte[1];
                    serialPort.Read(redLedValue, 0, 1);
                    Console.WriteLine(redLedValue[0]);
                } catch(TimeoutException) { }
            }
        }

        private void greenTrackBar_Scroll(object sender, EventArgs e)
        {
            if (serialPort != null && serialPort.IsOpen)
            {
                Byte[] command = new Byte[3];
                Byte value = (Byte)Math.Round(greenTrackBar.Value * 2.55);
                command[0] = Convert.ToByte('G');
                command[1] = value;
                command[2] = Convert.ToByte('\n');
                serialPort.Write(command, 0, command.Length);
                try
                {
                    Byte[] readCommand = new Byte[2];
                    readCommand[0] = Convert.ToByte('D');
                    readCommand[1] = Convert.ToByte('\n');
                    serialPort.Write(readCommand, 0, readCommand.Length);
                    Byte[] greenLedValue = new Byte[1];
                    Console.WriteLine(serialPort.Read(greenLedValue, 0, 1));
                }
                catch (TimeoutException) { }
            }
        }

        private void blueTrackBar_Scroll(object sender, EventArgs e)
        {
            if (serialPort != null && serialPort.IsOpen)
            {
                Byte[] command = new Byte[3];
                Byte value = (Byte)Math.Round(blueTrackBar.Value * 2.55);
                command[0] = Convert.ToByte('B');
                command[1] = value;
                command[2] = Convert.ToByte('\n');
                serialPort.Write(command, 0, command.Length);
                try
                {
                    Byte[] readCommand = new Byte[2];
                    readCommand[0] = Convert.ToByte('E');
                    readCommand[1] = Convert.ToByte('\n');
                    serialPort.Write(readCommand, 0, readCommand.Length);
                    Byte[] blueLedValue = new Byte[1];
                    Console.WriteLine(serialPort.Read(blueLedValue, 0, 1));
                }
                catch(TimeoutException) { }
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {

        }
        [SecurityPermissionAttribute(SecurityAction.InheritanceDemand, Flags = SecurityPermissionFlag.UnmanagedCode)]
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags = SecurityPermissionFlag.UnmanagedCode)]
        protected override void WndProc(ref Message m)
        {
            int devType;
            base.WndProc(ref m);

            switch (m.WParam.ToInt32())
            {
                case DBT_DEVICEARRIVAL:

                    devType = Marshal.ReadInt32(m.LParam, 4);
                    if (devType == DBT_DEVTYP_PORT)
                    {
                        mDeleg = new deviceSerialPortDelegate(deviceSerialPort);
                        AsyncCallback callback = new AsyncCallback(deviceConnectedCallback);

                        // invoke the thread that will handle getting the serial port number.   
                        mDeleg.BeginInvoke(callback, null);
                    }
                    break;

                case DBT_DEVICEREMOVECOMPLETE:

                    devType = Marshal.ReadInt32(m.LParam, 4);
                    if (devType == DBT_DEVTYP_PORT)
                    {
                        mDeleg = new deviceSerialPortDelegate(deviceSerialPort);
                        AsyncCallback callback = new AsyncCallback(deviceDisconnectedCallback);

                        // invoke the thread that will handle getting the serial port number.
                        mDeleg.BeginInvoke(callback, null);
                    }
                    break;
            }
        }

        // function queries the system using WMI and returns the serial port number if the device 
        //is connected to the system, -1 otherwise.   
        private short deviceSerialPort()
        {
            short comPort = -1;

            // getting a list of all available com port devices and their friendly names     
            // must add System.Management DLL resource to solution before using this     
            // Project -> Add Reference -> .Net tab, choose System.Management  
            ManagementObjectSearcher searcher = new ManagementObjectSearcher("root\\CIMV2",
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
                    if (name.Contains(DEVICE_NAME))
                    {
                        found = true;
                        short startIndex = (short) (name.LastIndexOf("M") + 1);
                        short endIndex = (short) name.LastIndexOf(")");
                        comPort = Convert.ToInt16(name.Substring(startIndex, endIndex - startIndex));
                    }
                }
                searcher.Dispose();
            }
            return comPort;
        }

        // delegate wrapper function for the deviceSerialPort function  
        private delegate short deviceSerialPortDelegate();

        // callback method when the thread returns  
        private void deviceConnectedCallback(IAsyncResult ar)
        {
            // got the Serial Port number.  
            serialPortNumber = mDeleg.EndInvoke(ar);
            if (serialPortNumber != -1 && !deviceConnected)
            {
                connectionUpdateGUI();
            }
        }

        // callback method when the thread returns
        private void deviceDisconnectedCallback(IAsyncResult ar)
        {
            // got the Serial Port number.
            serialPortNumber = mDeleg.EndInvoke(ar);
            if (serialPortNumber == -1)
            {
                disconnectionUpdateGUI();
            }
        }

        private void processDeviceConnection()
        {
            Console.WriteLine("Connection Test 2");
            textBox1.Text = "COM" + serialPortNumber;
            serialPort = new SerialPort("COM" + serialPortNumber, BAUDRATE);
            serialPort.ReadTimeout = 500;
            serialPort.WriteTimeout = 500;
            if (!serialPort.IsOpen)
            {
                serialPort.Open();
            }
            deviceConnected = true;
            redTrackBar.Enabled = true;
            greenTrackBar.Enabled = true;
            blueTrackBar.Enabled = true;
        }

        private void processDeviceDisconnection()
        {
            Console.WriteLine("Disconnection Test 2");
            textBox1.Text = "";
            deviceConnected = false;
            redTrackBar.Enabled = false;
            greenTrackBar.Enabled = false;
            blueTrackBar.Enabled = false;
        }

        public void connectionUpdateGUI()
        {
            if (this.InvokeRequired)
            {
                this.BeginInvoke(new MethodInvoker(() => connectionUpdateGUI()));
            }
            else
            {
                processDeviceConnection();
            }
        }

        public void disconnectionUpdateGUI()
        {
            if (this.InvokeRequired)
            {
                this.BeginInvoke(new MethodInvoker(() => disconnectionUpdateGUI()));
            }
            else
            {
                processDeviceDisconnection();
            }
        }
    }
}
