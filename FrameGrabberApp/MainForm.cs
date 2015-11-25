using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;
using FrameGrabberApp.Properties;
using System.IO;
using FrameGrabberApp;
using System.Reflection;

namespace FrameGrabberApp
{
    public partial class MainForm : Form
    {
        private SerialPort _serial = new SerialPort();
        private Media _media = new Media();
        private Logger _logger = new Logger();

        public MainForm( )
        {
            InitializeComponent( );
            _logger.Init( Path.GetDirectoryName(Assembly.GetEntryAssembly().Location)
                + @"\FrameGrabberApp.log");
            _logger.WriteInfo("Application started");
        }

   
        private void MainForm_Load( object sender, EventArgs e )
        {
            try
            {
                 _media.Init(_logger);
                foreach (string device in _media.EnumCaptureDevices())
                     cmbDevices.Items.Add(device);

                cmbDevices.SelectedIndex = 0;
                cmbPort.SelectedIndex = 0;

                EnableControls(true);

                LoadSettings();

                InitSerial();

                StartCapture();
            }
            catch (Exception ex)
            {
                _logger.WriteError("Failed to load main form - " + ex.Message);
            }
               
        }
        private void MainForm_FormClosing( object sender, FormClosingEventArgs e )
        {
            SaveSettings();
            StopCapture( );
            _serial.Close();
            _serial.PinChanged -= _serial_PinChanged;
        }

        private void EnableControls( bool enable )
        {
            cmbDevices.Enabled = enable;
            btnPicture.Enabled = !enable;
            //cmbPort.Enabled = enable;
            tbOutputFolder.Enabled = enable;
            btnBrowse.Enabled = enable;
            
        }

        //// New video device is selected
        //private void devicesCombo_SelectedIndexChanged( object sender, EventArgs e )
        //{
        //    //if ( _videoDevices.Count != 0 )
        //    //{
        //    //    _videoDevice = new VideoCaptureDevice( _videoDevices[cmbDevices.SelectedIndex].MonikerString );
        //    //    EnumeratedSupportedFrameSizes( _videoDevice );
        //    //}
        //}

        // Collect supported video and snapshot sizes
        //private void EnumeratedSupportedFrameSizes( VideoCaptureDevice videoDevice )
        //{
        //    this.Cursor = Cursors.WaitCursor;

        //    try
        //    {
        //        //_videoCapabilities = videoDevice.VideoCapabilities;
        //        //_snapshotCapabilities = videoDevice.SnapshotCapabilities;
        //    }
        //    finally
        //    {
        //        this.Cursor = Cursors.Default;
        //    }
        //}

  

       
        // Simulate snapshot trigger
        private void triggerButton_Click( object sender, EventArgs e )
        {
        }

       

       
        private void InitSerial()
        {
           if (_serial.IsOpen)
           {
               _serial.Close();
               _serial.PinChanged -= _serial_PinChanged;
           }
           try
           {
               _serial.PinChanged += _serial_PinChanged;
               _serial.DataReceived += _serial_DataReceived;
               _serial.PortName = cmbPort.Text;
               _serial.BaudRate = 9600;
               _serial.DataBits = 8;
               _serial.StopBits = StopBits.One;
               _serial.Parity = Parity.Even;
               _serial.RtsEnable = true;
               _serial.DtrEnable = true;
               _serial.Open();

               _logger.WriteInfo("Serial port " + cmbPort.Text + " is opened");
           }
           catch(Exception ex)
           {
               _logger.WriteError("Failed to open serial port - " + ex.Message);
           }
            
        }

        void _serial_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            _logger.WriteInfo("Serial port new data received");
            TakePicture();
        }

        void _serial_PinChanged(object sender, SerialPinChangedEventArgs e)
        {
            _logger.WriteInfo("Serial port pin state changed");
            TakePicture();
          }

        private void StartCapture()
        {
            try
            {
                _media.Start(pbVideo, cmbDevices.Text);
                EnableControls(false);
                btnCapture.Text = "Stop capture";
                _logger.WriteInfo("Capture started, device - " + cmbDevices.Text);
            }
            catch (Exception ex)
            {
                _logger.WriteError("Failed to start capture - " + ex.Message);
            }
        }


        private void StopCapture()
        {
            try
            {                
                _media.Stop();
                EnableControls(true);
                btnCapture.Text = "Start capture";
                _logger.WriteInfo("Capture stopped");
            }
            catch(Exception ex)
            {
                _logger.WriteError("Failed to stop capture - " + ex.Message);
            }
        }


       

        private void btnCapture_Click(object sender, EventArgs e)
        {
            if(btnCapture.Text == "Start capture")
            {             
                StartCapture();             
            }
            else
            {
                StopCapture();              
            }
        }

        private void btnBrowse_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog dlg = new FolderBrowserDialog();
            if (dlg.ShowDialog() == DialogResult.OK)
                tbOutputFolder.Text = dlg.SelectedPath;
        }

        private void cmbPort_SelectedIndexChanged(object sender, EventArgs e)
        {
            InitSerial();
        }


        private void SaveSettings()
        {
            Settings.Default.OutputFolder = tbOutputFolder.Text;
            Settings.Default.ComPort = cmbPort.Text;
            Settings.Default.Save();
        }

        private void LoadSettings()
        {
            Settings.Default.Reload();
            tbOutputFolder.Text = Settings.Default.OutputFolder;
            cmbPort.Text = Settings.Default.ComPort;
        }

        private void btnPicture_Click(object sender, EventArgs e)
        {
            TakePicture();
        }  

        private void TakePicture()
        {
            _logger.WriteInfo("Trying to make a screenshoot");
            try
            {
                btnPicture.Enabled = false;
                if (Directory.Exists(tbOutputFolder.Text) == false)
                    throw new Exception("Folder " + tbOutputFolder.Text + " doesn't exist");

                string fileName = tbOutputFolder.Text + @"\" + DateTime.Now.ToString("yyyy-MM-dd-hh-mm-ss") + ".png";
                _media.TakePicture(fileName);
                _logger.WriteInfo("Screenshoot is made and saved to " + fileName);
            }
            catch(Exception ex)
            {
                _logger.WriteError("Failed to make a screenshoot - " + ex.Message);
            }
            finally
            {
                btnPicture.Enabled = true;
            }
        }
    }
}
