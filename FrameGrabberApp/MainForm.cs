// Snapshot Maker sample application
// AForge.NET Framework
// http://www.aforgenet.com/framework/
//
// Copyright © AForge.NET, 2009-2011
// contacts@aforgenet.com
//

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using AForge.Video;
using AForge.Video.DirectShow;
using System.IO.Ports;
using FrameGrabberApp.Properties;
using System.IO;

namespace Snapshot_Maker
{
    public partial class MainForm : Form
    {
        private FilterInfoCollection _videoDevices;
        private VideoCaptureDevice _videoDevice;
        private VideoCapabilities[] _videoCapabilities;
        private VideoCapabilities[] _snapshotCapabilities;

       
        private bool _take = false;
        private SerialPort _serial = new SerialPort();

        public MainForm( )
        {
            InitializeComponent( );
        }

        // Main form is loaded
        private void MainForm_Load( object sender, EventArgs e )
        {
          
            // enumerate video devices
            _videoDevices = new FilterInfoCollection( FilterCategory.VideoInputDevice );

            if ( _videoDevices.Count != 0 )
            {
                // add all devices to combo
                foreach ( FilterInfo device in _videoDevices )
                {
                    cmbDevices.Items.Add( device.Name );
                }
            }
            else
            {
                cmbDevices.Items.Add( "No DirectShow devices found" );
            }

            cmbDevices.SelectedIndex = 0;
           // cmbPort.SelectedIndex = 0;

            EnableControls( true );

            LoadSettings();

            //InitSerial();

            StartCapture();
        }

        // Closing the main form
        private void MainForm_FormClosing( object sender, FormClosingEventArgs e )
        {
            SaveSettings();
            StopCapture( );
            _serial.Close();
            _serial.PinChanged -= _serial_PinChanged;
        }

        // Enable/disable connection related controls
        private void EnableControls( bool enable )
        {
            cmbDevices.Enabled = enable;
            btnPicture.Enabled = ( !enable ) && ( _snapshotCapabilities.Length != 0 );
            //cmbPort.Enabled = enable;
            tbOutputFolder.Enabled = enable;
            btnBrowse.Enabled = enable;
            
        }

        // New video device is selected
        private void devicesCombo_SelectedIndexChanged( object sender, EventArgs e )
        {
            if ( _videoDevices.Count != 0 )
            {
                _videoDevice = new VideoCaptureDevice( _videoDevices[cmbDevices.SelectedIndex].MonikerString );
                EnumeratedSupportedFrameSizes( _videoDevice );
            }
        }

        // Collect supported video and snapshot sizes
        private void EnumeratedSupportedFrameSizes( VideoCaptureDevice videoDevice )
        {
            this.Cursor = Cursors.WaitCursor;

            try
            {
                _videoCapabilities = videoDevice.VideoCapabilities;
                _snapshotCapabilities = videoDevice.SnapshotCapabilities;
            }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }

  

        // Disconnect from video device
        private void StopCapture( )
        {
            if ( videoSourcePlayer.VideoSource != null )
            {
                // stop video device
                videoSourcePlayer.SignalToStop( );
                videoSourcePlayer.WaitForStop( );
                videoSourcePlayer.VideoSource = null;

                if ( _videoDevice.ProvideSnapshots )
                {
                    _videoDevice.SnapshotFrame -= videoDevice_SnapshotFrame;
                }

                EnableControls( true );
                btnCapture.Text = "Start capture";
            }
        }

        // Simulate snapshot trigger
        private void triggerButton_Click( object sender, EventArgs e )
        {
            _take = true;

            //if ( ( videoDevice != null ) && ( videoDevice.ProvideSnapshots ) )
            //{
            //    videoDevice.SimulateTrigger( );
            //}
        }

        // New snapshot frame is available
        private void videoDevice_SnapshotFrame( object sender, NewFrameEventArgs eventArgs )
        {
            //Console.WriteLine( eventArgs.Frame.Size );

            //ShowSnapshot( (Bitmap) eventArgs.Frame.Clone( ) );

            String fileName = tbOutputFolder.Text + "\\"+  DateTime.Now.ToString("yyyy-MM-dd-hh-mm-ss") + ".png";
            Bitmap bmp = (Bitmap)eventArgs.Frame.Clone();
            bmp.Save(fileName);
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
           }
           catch(Exception ex)
           {
               MessageBox.Show(ex.Message);
           }
            
        }

        void _serial_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            _take = true;
        }

        void _serial_PinChanged(object sender, SerialPinChangedEventArgs e)
        {
            //throw new NotImplementedException();
            _take = true;
        }

        private void StartCapture()
        {
            if (_videoDevice != null)
            {
                if ((_videoCapabilities != null) && (_videoCapabilities.Length != 0))
                {
                    //videoDevice.VideoResolution = videoCapabilities[videoResolutionsCombo.SelectedIndex];
                    _videoDevice.VideoResolution = _videoCapabilities[0];

                }

                if ((_snapshotCapabilities != null) && (_snapshotCapabilities.Length != 0))
                {
                    _videoDevice.ProvideSnapshots = true;
                    _videoDevice.SnapshotResolution = _snapshotCapabilities[0];
               
                    _videoDevice.NewFrame += videoDevice_NewFrame;
                    _videoDevice.SnapshotFrame += videoDevice_SnapshotFrame;
                }

                EnableControls(false);

                videoSourcePlayer.VideoSource = _videoDevice;
                videoSourcePlayer.AutoSizeControl = false;
                videoSourcePlayer.Start();
         

                btnCapture.Text = "Stop capture";
            }
        }

        void videoDevice_NewFrame(object sender, NewFrameEventArgs eventArgs)
        {
           if (_take == true)
           {
               try
               {
                   if (Directory.Exists(tbOutputFolder.Text) == true)
                   {
                       string fileName = tbOutputFolder.Text + @"\\" + DateTime.Now.ToString("yyyy-MM-dd-hh-mm-ss") + ".png";
                       Bitmap bmp = (Bitmap)eventArgs.Frame.Clone();
                       bmp.Save(fileName);
                       _take = false;
                   }
               }
               catch(Exception)
               {

               }
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
    }
}
