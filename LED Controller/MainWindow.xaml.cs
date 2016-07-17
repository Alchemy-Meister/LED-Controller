namespace LED_Controller
{
    using System;
    using System.Collections.Generic;
    using System.Drawing;
    using System.Linq;
    using System.Threading.Tasks;
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

        private NotifyIcon notifyIcon;

        private bool statusHardCoded = true;
        private bool effectHardCoded = true;

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
                this.Closing += this.MainWindow_Closing;
                this.StateChanged += this.Window_StateChanged;
                this.InitializeComponent();
                this.InitializeSerialPort();
                this.InitializeStatusComboBox();
                this.InitializeTray();
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
                switch ((int)wparam)
                {
                    case UsbNotification.DbtDevicearrival:
                        Task.Run(() =>
                        {
                            lock(this.controller)
                            {
                                if (this.controller.DeviceUpdatedSerialPort() != -1 && !this.controller.IsSerialPortOpened())
                                {
                                    // Opens the serial connection and updates the GUI on a safe thread.
                                    this.controller.InitializeSerialPort();
                                    this.controller.OpenSerialPort();

                                    // Make sure that serial has finished opening.
                                    Task.Delay(2000).Wait();
                                    if(this.controller.HandShake())
                                    {
                                        this.controller.UpdateModel();
                                        this.SafeExecution(this.ProcessDeviceConnectionGUI);
                                    }
                                }
                            }
                        });
                        break;
                    case UsbNotification.DbtDeviceremovecomplete:
                        Task.Run(() =>
                        {
                            if (this.controller.DeviceUpdatedSerialPort() == -1)
                            {
                                // Updates the GUI on a safe thread.
                                this.controller.CloseSerialPort();
                                this.SafeExecution(this.ProcessDeviceDisconnectionGUI);
                            }
                        });
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
                
                // Make sure that serial has finished opening.
                Task.Delay(2000).Wait();

                if (this.controller.HandShake())
                {
                    this.controller.UpdateModel();
                    this.ProcessDeviceConnectionGUI();
                }
            }
            else
            {
                this.ProcessDeviceDisconnectionGUI();
            }
        }

        private void InitializeEffectComboBox(short model)
        {
            Dictionary<string, byte> effectList = this.controller.GetEffectList(model);
            

            this.effectHardCoded = true;
            this.effectCBox.ItemsSource = effectList;
            this.effectCBox.DisplayMemberPath = "Key";
            if(this.effectCBox.SelectedIndex == -1)
            {
                this.effectCBox.SelectedIndex = 0;
                this.controller.SendWriteMessage(Convert.ToByte(((KeyValuePair<string, byte>)this.effectCBox.SelectedItem).Value));
            }

            this.effectHardCoded = false;
            this.effectCBox.Items.Refresh();
        }

        private void  InitializeStatusComboBox()
        {
            Dictionary<string, byte> statusList = this.controller.GetStatusList();

            this.statusHardCoded = true;
            this.statusCBox.ItemsSource = statusList;
            this.statusCBox.DisplayMemberPath = "Key";
            this.statusCBox.SelectedIndex = 0;
            this.statusHardCoded = false;
            this.statusCBox.Items.Refresh();
        }

        private void SaveRGBValues()
        {
            this.controller.SetRedLedValue((byte)Math.Round(redSlider.Value * 2.55));
            this.controller.SetGreenLedValue((byte)Math.Round(greenSlider.Value * 2.55));
            this.controller.SetBlueLedValue((byte)Math.Round(blueSlider.Value * 2.55));
        }

        // The function that opens the serial connection and updates de GUI according to the device connection.
        private void ProcessDeviceConnectionGUI()
        {
            short? model = this.controller.GetModel();
            if(model != null)
            {
                textBox.Text = "Model " + model + " COM" + this.controller.GetSerialPort();
                this.statusCBox.IsEnabled = true;
                this.InitializeEffectComboBox((short)model);

                if (this.controller.IsLedPowered())
                {
                    this.ProcessLedOnGUI();
                    this.controller.InitializeDevice();
                    this.controller.SendWriteMessage(Convert.ToByte(((KeyValuePair<string, byte>)this.effectCBox.SelectedItem).Value));
                }
            }
        }

        private void ProcessLedOnGUI()
        {
            this.effectCBox.IsEnabled = true;
            this.redSlider.IsEnabled = true;
            this.greenSlider.IsEnabled = true;
            this.blueSlider.IsEnabled = true;
            this.applyButton.IsEnabled = true;
        }

        // Updates de GUI according to the device disconneciton.
        private void ProcessDeviceDisconnectionGUI()
        {
            this.SaveRGBValues();
            this.textBox.Text = string.Empty;
            this.statusCBox.IsEnabled = false;
            this.ProcessLedOffGUI();
        }

        private void ProcessLedOffGUI()
        {
            this.effectCBox.IsEnabled = false;
            this.redSlider.IsEnabled = false;
            this.greenSlider.IsEnabled = false;
            this.blueSlider.IsEnabled = false;
            this.applyButton.IsEnabled = false;
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

        private async void RedSliderValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            double value = redSlider.Value;
            await Task.Run(() =>
            {
                this.controller.SendWriteMessage(Convert.ToByte('R'), (byte)Math.Round(value * 2.55));
                Console.WriteLine(this.controller.SendReadMessage(Convert.ToByte('C')));
            });
        }

        private async void GreenSliderValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            double value = greenSlider.Value;
            await Task.Run(() =>
            {
                this.controller.SendWriteMessage(Convert.ToByte('G'), (byte)Math.Round(value * 2.55));
                Console.WriteLine(this.controller.SendReadMessage(Convert.ToByte('D')));
            });
        }

        private async void BlueSliderValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            double value = blueSlider.Value;
            await Task.Run(() =>
            {
                this.controller.SendWriteMessage(Convert.ToByte('B'), (byte)Math.Round(value * 2.55));
                Console.WriteLine(this.controller.SendReadMessage(Convert.ToByte('E')));
            });
        }

        private async void EffectChanged(object sender, SelectionChangedEventArgs e)
        {
            if (!this.effectHardCoded)
            {
                byte selectedEffect = ((KeyValuePair<string, byte>)this.effectCBox.SelectedItem).Value;
                await Task.Run(() =>
                {
                    this.controller.SendWriteMessage(Convert.ToByte(selectedEffect));
                });
            }
        }

        private async void StatusChanged(object sender, SelectionChangedEventArgs e)
        {
            if(!this.statusHardCoded)
            {
                KeyValuePair<string, byte> selectedStatus =
                (KeyValuePair<string, byte>) this.statusCBox.SelectedItem;

                await Task.Run(() =>
                {
                    this.controller.SendWriteMessage(Convert.ToByte(selectedStatus.Value));

                    if (selectedStatus.Key.Equals(this.controller.GetStatusList().Keys.First()))
                    {
                        this.controller.SetLedPower(true);
                        this.controller.InitializeDevice();
                        this.SafeExecution(this.ProcessLedOnGUI);
                    }
                    else
                    {
                        this.controller.SetLedPower(false);
                        this.SafeExecution(this.SaveRGBValues);
                        this.SafeExecution(this.ProcessLedOffGUI);
                    }
                });
            }
        }

        private void Checked(object sender, RoutedEventArgs e)
        {
            this.CheckChanged(sender as System.Windows.Controls.CheckBox);
        }

        private void Unchecked(object sender, RoutedEventArgs e)
        {
            this.CheckChanged(sender as System.Windows.Controls.CheckBox);
        }

        private async void CheckChanged(System.Windows.Controls.CheckBox checkbox)
        {
            bool clockwiseChanged = (bool)checkbox.IsChecked;
            await Task.Run(() =>
            {
                if (clockwiseChanged)
                {
                    this.controller.SendWriteMessage(Convert.ToByte('Q'), Convert.ToByte(1));
                }
                else
                {
                    this.controller.SendWriteMessage(Convert.ToByte('Q'), Convert.ToByte(0));
                }
            });
        }
    }
}
