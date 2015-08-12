namespace LED_Controller
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.redTrackBar = new System.Windows.Forms.TrackBar();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.greenTrackBar = new System.Windows.Forms.TrackBar();
            this.blueTrackBar = new System.Windows.Forms.TrackBar();
            this.button1 = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.redTrackBar)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.greenTrackBar)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.blueTrackBar)).BeginInit();
            this.SuspendLayout();
            // 
            // redTrackBar
            // 
            this.redTrackBar.LargeChange = 10;
            this.redTrackBar.Location = new System.Drawing.Point(404, 33);
            this.redTrackBar.Maximum = 100;
            this.redTrackBar.Name = "redTrackBar";
            this.redTrackBar.Size = new System.Drawing.Size(347, 90);
            this.redTrackBar.TabIndex = 0;
            this.redTrackBar.TabStop = false;
            this.redTrackBar.TickStyle = System.Windows.Forms.TickStyle.None;
            this.redTrackBar.ValueChanged += new System.EventHandler(this.redTrackBar_ValueChanged);
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(12, 12);
            this.textBox1.Multiline = true;
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(373, 213);
            this.textBox1.TabIndex = 1;
            // 
            // greenTrackBar
            // 
            this.greenTrackBar.LargeChange = 10;
            this.greenTrackBar.Location = new System.Drawing.Point(404, 101);
            this.greenTrackBar.Maximum = 100;
            this.greenTrackBar.Name = "greenTrackBar";
            this.greenTrackBar.Size = new System.Drawing.Size(347, 90);
            this.greenTrackBar.TabIndex = 2;
            this.greenTrackBar.TabStop = false;
            this.greenTrackBar.TickStyle = System.Windows.Forms.TickStyle.None;
            this.greenTrackBar.Scroll += new System.EventHandler(this.greenTrackBar_Scroll);
            // 
            // blueTrackBar
            // 
            this.blueTrackBar.LargeChange = 10;
            this.blueTrackBar.Location = new System.Drawing.Point(404, 170);
            this.blueTrackBar.Maximum = 100;
            this.blueTrackBar.Name = "blueTrackBar";
            this.blueTrackBar.Size = new System.Drawing.Size(347, 90);
            this.blueTrackBar.TabIndex = 3;
            this.blueTrackBar.TabStop = false;
            this.blueTrackBar.TickStyle = System.Windows.Forms.TickStyle.None;
            this.blueTrackBar.Scroll += new System.EventHandler(this.blueTrackBar_Scroll);
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(590, 245);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(161, 75);
            this.button1.TabIndex = 4;
            this.button1.Text = "Apply";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(12F, 25F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(774, 347);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.blueTrackBar);
            this.Controls.Add(this.greenTrackBar);
            this.Controls.Add(this.textBox1);
            this.Controls.Add(this.redTrackBar);
            this.Name = "Form1";
            this.Text = "LED Controller";
            this.Load += new System.EventHandler(this.Form1_Load);
            ((System.ComponentModel.ISupportInitialize)(this.redTrackBar)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.greenTrackBar)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.blueTrackBar)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        private void InitializeSerialPort()
        {
            serialPortNumber = deviceSerialPort();
            if(serialPortNumber != -1)
            {
                processDeviceConnection();
                System.Console.WriteLine("Connection Test 1");
            }
            else
            {
                processDeviceDisconnection();
                System.Console.WriteLine("Disconnection Test 1");
            } 
        }

        #endregion

        private System.Windows.Forms.TrackBar redTrackBar;
        private System.Windows.Forms.TrackBar greenTrackBar;
        private System.Windows.Forms.TrackBar blueTrackBar;
        private System.IO.Ports.SerialPort serialPort;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Button button1;
    }
}

