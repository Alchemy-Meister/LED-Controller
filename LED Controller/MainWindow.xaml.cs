using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.IO.Ports;
using System.Threading;
using System.Management;
using System.Windows.Interop;
using System.Windows.Forms;
using System.Drawing;
using System.IO;

namespace LED_Controller
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

        private const Int16 BAUDRATE = 9600;
        private const string DEVICE_NAME = "USB-SERIAL CH340 (COM";

        private bool deviceConnected = false;

        private deviceSerialPortDelegate mDeleg;
        private int serialPortNumber;

        private SerialPort serialPort;

        private NotifyIcon nIcon;

        public MainWindow()
        {
            mDeleg = new deviceSerialPortDelegate(deviceSerialPort);
            InitializeComponent();
            InitializeTray();
            InitializeSerialPort();
            
        }

        private void InitializeTray()
        {
            nIcon = new NotifyIcon()
            {
                ContextMenuStrip = new ContextMenuStrip(),
                Icon = new Icon(System.Windows.Application.GetResourceStream(
                new Uri("pack://application:,,,/LED Controller;component/resources/favicon.ico")).Stream)
            };
            System.Windows.Forms.MenuItem exit = new System.Windows.Forms.MenuItem();
            exit.Index = 0;
            exit.Text = "Exit";
            exit.Click += Exit_Click;
            System.Windows.Forms.ContextMenu trayMenu = new System.Windows.Forms.ContextMenu();
            trayMenu.MenuItems.AddRange(
                    new System.Windows.Forms.MenuItem[] { exit });
            nIcon.ContextMenu = trayMenu;
        }

        private void Exit_Click(object sender, EventArgs e)
        {
            UsbNotification.UnregisterUsbDeviceNotification();
            App.Current.Shutdown();
        }

        protected override void OnSourceInitialized(EventArgs e)
        {
            base.OnSourceInitialized(e);

            // Adds the windows message processing hook and registers USB device add/removal notification.
            HwndSource source = HwndSource.FromHwnd(new WindowInteropHelper(this).Handle);
            if (source != null)
            {
                IntPtr windowHandle = source.Handle;
                source.AddHook(HwndHandler);
                UsbNotification.RegisterUsbDeviceNotification(windowHandle);
            }
        }

        protected override void OnClosed(EventArgs e)
        {
            nIcon.Visible = true;   
        }

        /// <summary>
        /// Method that receives window messages.
        /// </summary>
        private IntPtr HwndHandler(IntPtr hwnd, int msg, IntPtr wparam, IntPtr lparam, ref bool handled)
        {
            if (msg == UsbNotification.WmDevicechange)
            {
                AsyncCallback callback;
                switch ((int)wparam)
                {
                    case UsbNotification.DbtDevicearrival:
                        callback = new AsyncCallback(deviceConnectedCallback);

                        // invoke the thread that will handle getting the serial port number.   
                        mDeleg.BeginInvoke(callback, null);
                        break;
                    case UsbNotification.DbtDeviceremovecomplete:
                        callback = new AsyncCallback(deviceDisconnectedCallback);

                        // invoke the thread that will handle getting the serial port number.
                        mDeleg.BeginInvoke(callback, null);
                        break;
                }
            }

            handled = false;
            return IntPtr.Zero;
        }

        private void InitializeSerialPort()
        {
            serialPortNumber = deviceSerialPort();
            if (serialPortNumber != -1)
            {
                deviceConnected = true;
                prepareSerialPort();
                processDeviceConnectionGUI();
            }
            else
            {
                deviceConnected = false;
                processDeviceDisconnectionGUI();
            }
        }

        // delegate wrapper function for the deviceSerialPort function
        private delegate short deviceSerialPortDelegate();

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
                        short startIndex = (short)(name.LastIndexOf("M") + 1);
                        short endIndex = (short)name.LastIndexOf(")");
                        comPort = Convert.ToInt16(name.Substring(startIndex, endIndex - startIndex));
                    }
                }
                searcher.Dispose();
            }
            return comPort;
        }

        // callback method when the thread returns  
        private void deviceConnectedCallback(IAsyncResult ar)
        {
            // got the Serial Port number.  
            serialPortNumber = mDeleg.EndInvoke(ar);
            if (serialPortNumber != -1 && !deviceConnected)
            {
                //Opens the serial connection and updates the GUI on a safe thread.
                deviceConnected = true;
                prepareSerialPort();
                safeExecution(processDeviceConnectionGUI);
            }
        }

        // callback method when the thread returns
        private void deviceDisconnectedCallback(IAsyncResult ar)
        {
            // got the Serial Port number.
            serialPortNumber = mDeleg.EndInvoke(ar);
            if (serialPortNumber == -1)
            {
                //Updates the GUI on a safe thread.
                deviceConnected = false;
                safeExecution(processDeviceDisconnectionGUI);
            }
        }

        private void prepareSerialPort()
        {
            serialPort = new SerialPort("COM" + serialPortNumber, BAUDRATE);
            serialPort.ReadTimeout = 500;
            serialPort.WriteTimeout = 500;
            if (!serialPort.IsOpen)
            {
                serialPort.Open();
            }
        }

        //The function that opens the serial connection and updates de GUI according to the device connection.
        private void processDeviceConnectionGUI()
        {
            textBox.Text = "COM" + serialPortNumber;
            redSlider.IsEnabled = true;
            greenSlider.IsEnabled = true;
            blueSlider.IsEnabled = true;
            applyButton.IsEnabled = true;
        }

        //Updates de GUI according to the device disconneciton.
        private void processDeviceDisconnectionGUI()
        {
            textBox.Text = "";
            redSlider.IsEnabled = false;
            greenSlider.IsEnabled = false;
            blueSlider.IsEnabled = false;
            applyButton.IsEnabled = false;
        }

        //Execute action in the GUI thread.
        public void safeExecution(Action action)
        {
            if (!this.Dispatcher.CheckAccess())
            {
                this.Dispatcher.BeginInvoke(action);
            }
            else
            {
                action();
            }
        }

        private void redSliderValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if (serialPort != null && serialPort.IsOpen)
            {
                try
                {
                    Byte[] command = new Byte[3];
                    Byte value = (Byte)Math.Round(redSlider.Value * 2.55);
                    command[0] = Convert.ToByte('R');
                    command[1] = value;
                    command[2] = Convert.ToByte('\n');
                    serialPort.Write(command, 0, command.Length);
                    Byte[] readCommand = new Byte[2];
                    readCommand[0] = Convert.ToByte('C');
                    readCommand[1] = Convert.ToByte('\n');
                    serialPort.Write(readCommand, 0, readCommand.Length);
                    Byte[] redLedValue = new Byte[1];
                    serialPort.Read(redLedValue, 0, 1);
                    Console.WriteLine(redLedValue[0]);
                }
                catch (TimeoutException) { }
            }
        }

        private void greenSliderValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if (serialPort != null && serialPort.IsOpen)
            {
                try
                {
                    Byte[] command = new Byte[3];
                    Byte value = (Byte)Math.Round(greenSlider.Value * 2.55);
                    command[0] = Convert.ToByte('G');
                    command[1] = value;
                    command[2] = Convert.ToByte('\n');
                    serialPort.Write(command, 0, command.Length);
                    Byte[] readCommand = new Byte[2];
                    readCommand[0] = Convert.ToByte('D');
                    readCommand[1] = Convert.ToByte('\n');
                    serialPort.Write(readCommand, 0, readCommand.Length);
                    Byte[] greenLedValue = new Byte[1];
                    serialPort.Read(greenLedValue, 0, 1);
                    Console.WriteLine(greenLedValue[0]);
                }
                catch (TimeoutException) { }
            }
        }

        private void blueSliderValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if (serialPort != null && serialPort.IsOpen)
            {
                try
                {
                    Byte[] command = new Byte[3];
                    Byte value = (Byte)Math.Round(blueSlider.Value * 2.55);
                    command[0] = Convert.ToByte('B');
                    command[1] = value;
                    command[2] = Convert.ToByte('\n');
                    serialPort.Write(command, 0, command.Length);
                    Byte[] readCommand = new Byte[2];
                    readCommand[0] = Convert.ToByte('E');
                    readCommand[1] = Convert.ToByte('\n');
                    serialPort.Write(readCommand, 0, readCommand.Length);
                    Byte[] blueLedValue = new Byte[1];
                    serialPort.Read(blueLedValue, 0, 1);
                    Console.WriteLine(blueLedValue[0]);
                }
                catch (TimeoutException) { }
            }
        }
    }
}
