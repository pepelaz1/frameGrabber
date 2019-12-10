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
using System.Linq;
using System.Drawing.Imaging;
using System.Threading;

namespace FrameGrabberApp
{
    public partial class MainForm : Form
    {
        private SerialPort _serial = new SerialPort();
        private Media _media = new Media();
        private Logger _logger = new Logger();
        private List<PictureBox> _pboxes = new List<PictureBox>();
        private List<Label> _pnames = new List<Label>();
        private List<Label> _presults = new List<Label>();

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
                Text = "Frame Grabber Applicaion v" + Assembly.GetExecutingAssembly().GetName().Version.Major.ToString();

                 _media.Init(_logger);
                foreach (string device in _media.EnumCaptureDevices())
                     cmbDevices.Items.Add(device);

                cmbDevices.SelectedIndex = 0;
                cmbPort.SelectedIndex = 0;

                EnableControls(true);

                LoadSettings();

                InitSerial();

                InitThumbs();
                
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

        protected override bool ProcessCmdKey(ref Message message, Keys keys)
        {
            switch (keys)
            {
                case Keys.Space:
                    TakePicture();
                    return true;
            }

            return false;
        }
        private void EnableControls( bool enable )
        {
            cmbDevices.Enabled = enable;
            cmbResolution.Enabled = enable;
            btnPicture.Enabled = !enable;
            //cmbPort.Enabled = enable;
            tbOutputFolder.Enabled = enable;
            btnBrowse.Enabled = enable;
            
        }


        private void InitThumbs()
        {
            _pboxes.Add(pictureBox1);
            _pboxes.Add(pictureBox2);
            _pboxes.Add(pictureBox3);
            _pboxes.Add(pictureBox4);
            _pboxes.Add(pictureBox5);

            _pnames.Add(pname1);
            _pnames.Add(pname2);
            _pnames.Add(pname3);
            _pnames.Add(pname4);
            _pnames.Add(pname5);

            _presults.Add(presult1);
            _presults.Add(presult2);
            _presults.Add(presult3);
            _presults.Add(presult4);
            _presults.Add(presult5);

            RebuildThumbs();
        }
      
        private void SaveThumb(Bitmap bmp, string fileName, bool result)
        {
            try
            {
                _logger.WriteInfo("Save thumbnail - " + fileName);
                string path = Path.GetDirectoryName(Assembly.GetEntryAssembly().Location) + @"\thumbs";
                if (Directory.Exists(path) == false)
                    Directory.CreateDirectory(path);

                var directory = new DirectoryInfo(path);

             

                // Delete oldest file
                if (directory.GetFiles().Count() > 99)
                     directory.GetFiles().OrderBy(f => f.LastWriteTime).First().Delete();
    
 
                fileName = result ? "1_" + fileName : "0_" + fileName;

                bmp.Save(path + @"\" + fileName, ImageFormat.Png);               
                RebuildThumbs();
            }
            catch(Exception ex)
            {
                _logger.WriteError("Failed to save thumbnail - " + ex.Message);
            }
        }

        private void RebuildThumbs()
        {
            try 
            {
                 _logger.WriteInfo("Rebuilding thumbnails");
                string path = Path.GetDirectoryName(Assembly.GetEntryAssembly().Location) + @"\thumbs";
                if (Directory.Exists(path) == false)
                    Directory.CreateDirectory(path);

                for (int i = 0; i < 5; i++)
                {
                    _pboxes[i].Image = null;   
                    _pnames[i].Text = "";
                    _presults[i].Text = "";
                }

                int n = 0;
                var directory = new DirectoryInfo(path);
                foreach (var f in  directory.GetFiles()
                          .OrderByDescending(f => f.LastWriteTime))
                {
                    _presults[n].Text = "\u2713";
                    using (var bmp = new Bitmap(f.FullName))
                    {
                        _pboxes[n].Image = new Bitmap(bmp);
                        _pnames[n].Text = f.Name.Remove(0, 2);

                        if (f.Name.Substring(0, 1) == "1")
                        {
                            _presults[n].Text = "\u2713";
                            _presults[n].ForeColor = Color.Green;
                        }
                        else
                        {
                            _presults[n].Text = "\u2718";
                            _presults[n].ForeColor = Color.Red;
                        }               
                    }
                    n++;
                }     
            }
            catch(Exception ex) 
            {
                 _logger.WriteError("Failed to rebuid thumbnails - " + ex.Message);
            }   
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
            //if (DateTime.Now - _dt > new TimeSpan(0, 0, 2))
            //{
            //    _logger.WriteInfo("Serial port new data received");
            //    TakePicture();

            //    _dt = DateTime.Now;
            //}
        }

        DateTime _dt = DateTime.Now;
        void _serial_PinChanged(object sender, SerialPinChangedEventArgs e)
        {
            if (DateTime.Now - _dt > new TimeSpan(0, 0, 1))
            {
                _logger.WriteInfo("Serial port pin state changed");
                TakePicture();

                _dt = DateTime.Now;
            }
          }

        private void StartCapture()
        {
            try
            {
                _media.Start(pbVideo, cmbDevices.Text, cmbResolution.Text);
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
            Settings.Default.Device = cmbDevices.Text;
            Settings.Default.Resolution = cmbResolution.Text;
            Settings.Default.Save();
        }

        private void LoadSettings()
        {
            Settings.Default.Reload();
            tbOutputFolder.Text = Settings.Default.OutputFolder;
            cmbPort.Text = Settings.Default.ComPort;       
            cmbDevices.Text = Settings.Default.Device;
            cmbResolution.Text = Settings.Default.Resolution;
        }

        private void btnPicture_Click(object sender, EventArgs e)
        {
            TakePicture();
        }

        ManualResetEvent _ev = new ManualResetEvent(false);

        private void TakePicture()
        {
            if (_ev.WaitOne(0) == true)
                return;

            _ev.Set();
            if (btnCapture.Text == "Start capture")
                return;
       
            string fileName = "";
            bool result = false;
            Bitmap bmp = null;
            _logger.WriteInfo("Trying to make a screenshoot");
            try
            {
                btnPicture.Enabled = false;
           
                fileName = DateTime.Now.ToString("yyyy-MM-dd-hh-mm-ss") + ".png";       
                bmp =  _media.TakePicture();

                string fullName = tbOutputFolder.Text + @"\" + fileName;
                if (Directory.Exists(tbOutputFolder.Text) == false)
                    throw new Exception("Folder " + tbOutputFolder.Text + " doesn't exist");

                bmp.Save(fullName, ImageFormat.Png);
                result = true;
                _logger.WriteInfo("Screenshoot is made and saved to " + fileName);
            }
            catch(Exception ex)
            {
                _logger.WriteError("Failed to make a screenshoot - " + ex.Message);
                result = false;
            }
            finally
            {
                if (bmp != null)
                     SaveThumb(bmp, fileName, result);
                btnPicture.Enabled = true;
                _ev.Reset();
            }
        }

        private void cmbDevices_SelectedIndexChanged(object sender, EventArgs e)
        {
            cmbResolution.Items.Clear();
            foreach(var s in _media.GetAvailResolutions(cmbDevices.Text))
                cmbResolution.Items.Add(s);
            if (cmbResolution.Items.Count > 0)
                cmbResolution.SelectedIndex = 0;
        }

        private void cmbResolution_SelectedIndexChanged(object sender, EventArgs e)
        {

        }
    }
}
