namespace LED_Controller
{
    using System;
    using System.Collections.Generic;
    using System.Drawing;
    using System.Linq;
    using System.Windows;
    using System.Windows.Controls;
    using System.Windows.Forms;
    using System.Windows.Interop;

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private MainWindowController controller;

        private DeviceSerialPortDelegate deleg;

        private NotifyIcon notifyIcon;

        // Delegate wrapper function for the deviceSerialPort function
        private delegate short DeviceSerialPortDelegate();

        public MainWindow()
        {
            if (System.Diagnostics.Process.GetProcessesByName(
                System.IO.Path.GetFileNameWithoutExtension(
                    System.Reflection.Assembly.GetEntryAssembly().Location)).Count() > 1)
            {
                System.Diagnostics.Process.GetCurrentProcess().Kill();
            }
            else
            {
                this.controller = new MainWindowController();
                this.deleg = new DeviceSerialPortDelegate(this.controller.DeviceUpdatedSerialPort);
                this.Closing += this.MainWindow_Closing;
                this.StateChanged += this.Window_StateChanged;
                this.InitializeComponent();
                this.InitializeTray();
                this.InitializeSerialPort();
                this.InitializeEffectComboBox();
            }
        }

        protected override void OnSourceInitialized(EventArgs e)
        {
            base.OnSourceInitialized(e);

            // Adds the windows message processing hook and registers USB device add/removal notification.
            HwndSource source = HwndSource.FromHwnd(new WindowInteropHelper(this).Handle);
            if (source != null)
            {
                IntPtr windowHandle = source.Handle;
                source.AddHook(this.HwndHandler);
                UsbNotification.RegisterUsbDeviceNotification(windowHandle);
            }
        }

        private void InitializeTray()
        {
            this.notifyIcon = new NotifyIcon()
            {
                ContextMenuStrip = new ContextMenuStrip(),
                Icon = new Icon(System.Windows.Application.GetResourceStream(
                new Uri("pack://application:,,,/LED Controller;component/resources/favicon.ico")).Stream)
            };
            System.Windows.Forms.MenuItem exit = new System.Windows.Forms.MenuItem();
            exit.Index = 0;
            exit.Text = "Exit";
            exit.Click += this.Exit_Click;
            System.Windows.Forms.ContextMenu trayMenu = new System.Windows.Forms.ContextMenu();
            trayMenu.MenuItems.AddRange(
                    new System.Windows.Forms.MenuItem[] { exit });
            this.notifyIcon.ContextMenu = trayMenu;
            this.notifyIcon.Click += this.NIcon_Click;
        }

        private void NIcon_Click(object sender, EventArgs e)
        {
            MouseEventArgs me = e as System.Windows.Forms.MouseEventArgs;
            if (me != null && me.Button != MouseButtons.Right)
            {
                App.Current.MainWindow.Visibility = Visibility.Visible;
                this.ShowInTaskbar = true;
                if (this.WindowState == WindowState.Minimized)
                {
                    this.WindowState = WindowState.Normal;
                }

                this.notifyIcon.Visible = false;
            }
        }

        private void Exit_Click(object sender, EventArgs e)
        {
            UsbNotification.UnregisterUsbDeviceNotification();
            this.notifyIcon.Icon = null;
            this.notifyIcon.Visible = false;
            this.OnClosed(e);
            App.Current.Shutdown();
        }

        private void MainWindow_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            e.Cancel = true;
            App.Current.MainWindow.Visibility = Visibility.Hidden;
            this.notifyIcon.Visible = true;
        }

        private void Window_StateChanged(object sender, EventArgs e)
        {
            if (this.WindowState == WindowState.Minimized)
            {
                App.Current.MainWindow.Visibility = Visibility.Hidden;
                this.WindowState = WindowState.Normal;
                this.notifyIcon.Visible = true;
            }
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
                        callback = new AsyncCallback(this.DeviceConnectedCallback);

                        // invoke the thread that will handle getting the serial port number.   
                        this.deleg.BeginInvoke(callback, null);
                        break;
                    case UsbNotification.DbtDeviceremovecomplete:
                        callback = new AsyncCallback(this.DeviceDisconnectedCallback);

                        // invoke the thread that will handle getting the serial port number.
                        this.deleg.BeginInvoke(callback, null);
                        break;
                }
            }

            handled = false;
            return IntPtr.Zero;
        }

        private void InitializeSerialPort()
        {
            if (this.controller.DeviceUpdatedSerialPort() != -1)
            {
                this.controller.InitializeSerialPort();
                this.controller.OpenSerialPort();
                this.ProcessDeviceConnectionGUI();
            }
            else
            {
                this.ProcessDeviceDisconnectionGUI();
            }
        }

        private void InitializeEffectComboBox()
        {
            Dictionary<string, byte> effectList = this.controller.getEffectList();
            this.comboBox.ItemsSource = effectList;
            this.comboBox.DisplayMemberPath = "Key";
            this.comboBox.SelectedIndex = 0;
            this.comboBox.Items.Refresh();
        }

        // Callback method when the thread returns  
        private void DeviceConnectedCallback(IAsyncResult ar)
        {
            // Got the Serial Port number.  
            short serialPortNumber = this.deleg.EndInvoke(ar);
            
            // Lock the Controller until critical zone is completed
            lock (this.controller)
            {
                // CRITICAL ZONE since multiple device arrivals can be triggered 
                // at the same time and could try to open the port multiple times
                // raising an exception
                if (serialPortNumber != -1 && !this.controller.IsSerialPortOpened())
                {
                    // Opens the serial connection and updates the GUI on a safe thread.
                    this.controller.InitializeSerialPort();
                    this.controller.OpenSerialPort();
                    this.SafeExecution(this.ProcessDeviceConnectionGUI);
                }
            }
        }

        // Callback method when the thread returns
        private void DeviceDisconnectedCallback(IAsyncResult ar)
        {
            // Got the Serial Port number.
            short serialPortNumber = this.deleg.EndInvoke(ar);
            if (serialPortNumber == -1)
            {
                // Updates the GUI on a safe thread.
                this.controller.CloseSerialPort();
                this.SafeExecution(this.ProcessDeviceDisconnectionGUI);
            }
        }

        // The function that opens the serial connection and updates de GUI according to the device connection.
        private void ProcessDeviceConnectionGUI()
        {
            textBox.Text = "COM" + this.controller.GetSerialPort();
            this.InitializeEffectComboBox();
            this.comboBox.IsEnabled = true;
            redSlider.IsEnabled = true;
            greenSlider.IsEnabled = true;
            blueSlider.IsEnabled = true;
            applyButton.IsEnabled = true;
        }

        // Updates de GUI according to the device disconneciton.
        private void ProcessDeviceDisconnectionGUI()
        {
            textBox.Text = string.Empty;
            comboBox.IsEnabled = false;
            redSlider.IsEnabled = false;
            greenSlider.IsEnabled = false;
            blueSlider.IsEnabled = false;
            applyButton.IsEnabled = false;
        }

        // Execute action in the GUI thread.
        private void SafeExecution(Action action)
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

        private void RedSliderValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            this.controller.SendWriteMessage(Convert.ToByte('R'), (byte)Math.Round(redSlider.Value * 2.55));
            Console.WriteLine(this.controller.SendReadMessage(Convert.ToByte('C')));
        }

        private void GreenSliderValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            this.controller.SendWriteMessage(Convert.ToByte('G'), (byte)Math.Round(greenSlider.Value * 2.55));
            Console.WriteLine(this.controller.SendReadMessage(Convert.ToByte('D')));
        }

        private void BlueSliderValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            this.controller.SendWriteMessage(Convert.ToByte('B'), (byte)Math.Round(blueSlider.Value * 2.55));
            Console.WriteLine(this.controller.SendReadMessage(Convert.ToByte('E')));
        }

        private void EffectChanged(object sender, SelectionChangedEventArgs e)
        {
            this.controller.SendReadMessage(Convert.ToByte(((KeyValuePair<string, byte>)this.comboBox.SelectedItem).Value));
        }
    }
}
