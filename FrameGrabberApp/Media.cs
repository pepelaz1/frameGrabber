using DirectShowLib;
using DirectShowLib.DMO;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;
using System.Text;
using System.Threading;
using System.Windows.Forms;

namespace FrameGrabberApp
{
    class Media  : Form
    {
        enum PlayState
        {
            Stopped,
            Paused,
            Running,
            Init
        };

        public const int WM_GRAPHNOTIFY = 0x8000 + 1;

        IVideoWindow videoWindow = null;
        IMediaControl mediaControl = null;
        IMediaEventEx mediaEventEx = null;
        IGraphBuilder graphBuilder = null;
        IFilterGraph2 filterGraph2 = null;
        IBasicVideo basicVideo = null;
        ICaptureGraphBuilder2 captureGraphBuilder = null;
        PlayState currentState = PlayState.Stopped;
        PictureBox _pb;
        DsROTEntry rot = null;
        DsDevice[] _devices;
        Logger _logger;
        List<string> _resolutions = new List<string>();


        public void Init(Logger logger)
        {
            int hr = 0;
            _logger = logger;

            this.graphBuilder = (IGraphBuilder)new FilterGraph();


            this.captureGraphBuilder = (ICaptureGraphBuilder2)new CaptureGraphBuilder2();
            this.mediaControl = (IMediaControl)this.graphBuilder;
            this.videoWindow = (IVideoWindow)this.graphBuilder;
            this.mediaEventEx = (IMediaEventEx)this.graphBuilder;
            this.filterGraph2 = (IFilterGraph2)this.graphBuilder;
            this.basicVideo = (IBasicVideo)this.graphBuilder;

            hr = this.captureGraphBuilder.SetFiltergraph(this.graphBuilder);
            DsError.ThrowExceptionForHR(hr);

            _devices = null;
            _devices = DsDevice.GetDevicesOfCat(FilterCategory.VideoInputDevice);

            

        }

        public void Start(PictureBox pb, String deviceName, String resolution)
        {
            if (captureGraphBuilder == null)
                Init(_logger);

            IEnumFilters filterEnum = null;
            graphBuilder.EnumFilters(out filterEnum);
            filterEnum.Reset();

   
            IBaseFilter[] filters = null;
            IntPtr f = IntPtr.Zero;
            while (filterEnum.Next(1, filters, f) == 0)
            {
                graphBuilder.RemoveFilter(filters[0]);
            }


           // rot = new DsROTEntry(this.graphBuilder);
          //  return;

            _pb = pb;
            int hr = 0;
            IBaseFilter sourceFilter = null;
            try
            {
 
                // Use the system device enumerator and class enumerator to find
                // a video capture/preview device, such as a desktop USB video camera.
               // sourceFilter = FindCaptureDevice();
                DsDevice dev = _devices.Where(d => d.Name == deviceName).First();
                // Add the video device


               

                hr = filterGraph2.AddSourceFilterForMoniker(dev.Mon, null, dev.Name, out sourceFilter);
                DsError.ThrowExceptionForHR(hr);


                ConfigureSourceResolution(sourceFilter, resolution);

                // Add Capture filter to our graph.
               // hr = this.graphBuilder.AddFilter(sourceFilter, "Video Capture");
               // DsError.ThrowExceptionForHR(hr);

               // rot = new DsROTEntry(this.graphBuilder);
               // return;

                // IBaseFilter teeFilter = new SmartTee() as IBaseFilter;
                // hr = this.graphBuilder.AddFilter(teeFilter, "Smart Tee Filter");
                //DsError.ThrowExceptionForHR(hr);

                //sourceFilter.FindPin()

                // this.graphBuilder.Connect(sourceFilter.)

                //  ConfigureSource(sourceFilter, resolution);


                IBaseFilter rendererFilter =  new VideoMixingRenderer9() as IBaseFilter;
                IBaseFilter clcFilter = new Colour() as IBaseFilter;
               //  IBaseFilter rendererFilter =  new VideoRenderer() as IBaseFilter;

               
                
                hr = this.graphBuilder.AddFilter(clcFilter, "Color space converter");
                DsError.ThrowExceptionForHR(hr);

                hr = this.graphBuilder.AddFilter(rendererFilter, "Video Mixing Renderer 9");
                DsError.ThrowExceptionForHR(hr);

                // Render the preview pin on the video capture filter
                // Use this instead of this.graphBuilder.RenderFile
                hr = this.captureGraphBuilder.RenderStream(PinCategory.Preview, MediaType.Video, sourceFilter, clcFilter, rendererFilter);
                DsError.ThrowExceptionForHR(hr);

              //   ConfigureSource(sourceFilter, resolution);

                // Now that the filter has been added to the graph and we have
                // rendered its stream, we can release this reference to the filter.
                // Marshal.ReleaseComObject(sourceFilter);

                // Set video window style and position
                SetupVideoWindow();

                // Add our graph to the running object table, which will allow
                // the GraphEdit application to "spy" on our graph
                rot = new DsROTEntry(this.graphBuilder);
               

                // Start previewing video data
                hr = this.mediaControl.Run();
                DsError.ThrowExceptionForHR(hr);

                // Remember current state
                this.currentState = PlayState.Running;
            }
            catch(Exception ex)
            {
                throw ex;
            }
        }

        public void Stop()
        {
            if (this.mediaControl != null)
                this.mediaControl.StopWhenReady();

            this.currentState = PlayState.Stopped;

            // Stop receiving events
            if (this.mediaEventEx != null)
                this.mediaEventEx.SetNotifyWindow(IntPtr.Zero, WM_GRAPHNOTIFY, IntPtr.Zero);

        
            if (this.videoWindow != null)
            {
                this.videoWindow.put_Visible(OABool.False);
                this.videoWindow.put_Owner(IntPtr.Zero);
            }

            // Remove filter graph from the running object table
            if (rot != null)
            {
                rot.Dispose();
                rot = null;
            }

            // Release DirectShow interfaces
            Marshal.ReleaseComObject(this.mediaControl); this.mediaControl = null;
            Marshal.ReleaseComObject(this.mediaEventEx); this.mediaEventEx = null;
            Marshal.ReleaseComObject(this.videoWindow); this.videoWindow = null;
            Marshal.ReleaseComObject(this.filterGraph2); this.filterGraph2 = null;
            Marshal.ReleaseComObject(this.basicVideo); this.basicVideo = null;
            Marshal.ReleaseComObject(this.graphBuilder); this.graphBuilder = null;
            Marshal.ReleaseComObject(this.captureGraphBuilder); this.captureGraphBuilder = null;
        }
        
        private void ConfigureSourceResolution(IBaseFilter sourceFilter, String resolution)
        {
            int hr = 0;
            object o;
            AMMediaType amt = null;

            string []parts = resolution.Split("x".ToCharArray(), StringSplitOptions.RemoveEmptyEntries);
            hr = captureGraphBuilder.FindInterface(PinCategory.Preview, MediaType.Video, sourceFilter, typeof(IAMStreamConfig).GUID, out o);
            if (hr < 0)
                 hr = captureGraphBuilder.FindInterface(PinCategory.Capture, MediaType.Video, sourceFilter, typeof(IAMStreamConfig).GUID, out o);
          

            IAMStreamConfig videoStreamConfig = o as IAMStreamConfig;

            try
            {
                if (videoStreamConfig == null)
                {
                    throw new Exception("Failed to get IAMStreamConfig");
                }

                hr = videoStreamConfig.GetFormat(out amt);
                DsError.ThrowExceptionForHR(hr);

                if (amt.formatType == FormatType.VideoInfo)
                {
                    VideoInfoHeader v = new VideoInfoHeader();
                    Marshal.PtrToStructure(amt.formatPtr, v);
                    v.BmiHeader.Width = int.Parse(parts[0]);
                    v.BmiHeader.Height = int.Parse(parts[1]);
                    Marshal.StructureToPtr(v, amt.formatPtr, false);
                }
                else if (amt.formatType == FormatType.VideoInfo2)
                {
                    VideoInfoHeader2 v = new VideoInfoHeader2();
                    Marshal.PtrToStructure(amt.formatPtr, v);
                    v.BmiHeader.Width = int.Parse(parts[0]);
                    v.BmiHeader.Height = int.Parse(parts[1]);
                    Marshal.StructureToPtr(v, amt.formatPtr, false);
                }
                else
                {
                    throw new Exception("Unknown input video format");
                }

                // Set the new format
                hr = videoStreamConfig.SetFormat(amt);
                DsError.ThrowExceptionForHR(hr);

                DsUtils.FreeAMMediaType(amt);
                amt = null;

                //// Fix upsidedown video
                //if (videoControl != null)
                //{
                //    VideoControlFlags pCapsFlags;

                //    IPin pPin = DsFindPin.ByCategory(capFilter, PinCategory.Capture, 0);
                //    hr = videoControl.GetCaps(pPin, out pCapsFlags);
                //    DsError.ThrowExceptionForHR(hr);

                //    if ((pCapsFlags & VideoControlFlags.FlipVertical) > 0)
                //    {
                //        hr = videoControl.GetMode(pPin, out pCapsFlags);
                //        DsError.ThrowExceptionForHR(hr);

                //        hr = videoControl.SetMode(pPin, 0);
                //    }
                //}
            }
            finally
            {
                Marshal.ReleaseComObject(videoStreamConfig);
            }
        }

        public void SetupVideoWindow()
        {
            int hr = 0;

            hr = this.mediaEventEx.SetNotifyWindow(this.Handle, WM_GRAPHNOTIFY, IntPtr.Zero);
            DsError.ThrowExceptionForHR(hr);

            // Set the video window to be a child of the main window
            hr = this.videoWindow.put_Owner(_pb.Handle);
            DsError.ThrowExceptionForHR(hr);

            hr = this.videoWindow.put_WindowStyle(WindowStyle.Child | WindowStyle.ClipChildren);
            DsError.ThrowExceptionForHR(hr);

            // Use helper function to position video window in client rect 
            // of main application window
            ResizeVideoWindow();

            // Make the video window visible, now that it is properly positioned
            hr = this.videoWindow.put_Visible(OABool.True);
            DsError.ThrowExceptionForHR(hr);

           
        }

        public void ResizeVideoWindow()
        {
            // Resize the video preview window to match owner window size
            if (this.videoWindow != null)
            {
                this.videoWindow.SetWindowPosition(0, 0, _pb.ClientSize.Width, _pb.ClientSize.Height);
              
            }
        }

        public List<string> EnumCaptureDevices()
        {
            List<string> result = new List<string>();         

            foreach(var device in _devices)
                result.Add(device.Name);
                   
            return result;
        }
      
        public Bitmap TakePicture()
        {
            int hr = 0;
            int size = 0;
            IntPtr p = IntPtr.Zero;

            try
            {
                _logger.WriteInfo("Attempt to pause graph before making a screenshoot");
                mediaControl.Pause();
                FilterState fs = 0;
                while( mediaControl.GetState(100, out fs) != 0)
                {
                    if ( fs != FilterState.Paused)
                    {
                        _logger.WriteInfo("Graph is not it paused state. Continue waiting...");
                         continue;
                    }
                    _logger.WriteInfo("Graph is paused now");
                    break;
                }

          
                for(int i= 0; i < 10; i++)
                {
                    size = 0;
                    _logger.WriteInfo("Trying to get current image");
                    basicVideo.GetCurrentImage(ref size, p);
                    p = Marshal.AllocHGlobal(size);
                    hr = basicVideo.GetCurrentImage(ref size, p);
                    if (hr == 0)
                    {
                        _logger.WriteInfo("Image successfully taken");
                        break;
                    }

                    _logger.WriteError("Failed to get current image - " + DsError.GetErrorText(hr));
                    Marshal.FreeHGlobal(p);
                    Thread.Sleep(100);
                }
            }
            catch (Exception ex)
            {
                throw ex;
            }
            finally
            {
                mediaControl.Run();
            }
           

            BitmapInfoHeader head;
            head = (BitmapInfoHeader)Marshal.PtrToStructure(p, typeof(BitmapInfoHeader));
   
            int width = head.Width;
            int height = head.Height;
            int stride = width * (head.BitCount / 8);

            _logger.WriteInfo(@"BitmapInfo: width=" + width.ToString()
                + ", height=" + height.ToString()
                + ", bitcount="+head.BitCount.ToString()
                + ", compression="+head.Compression.ToString()
                + ", imagesize=" + head.ImageSize.ToString()
                );


            PixelFormat pixelFormat = PixelFormat.Format24bppRgb;           
            switch (head.BitCount)
            {
                case 16: pixelFormat = PixelFormat.Format16bppRgb565; break;
                case 24: pixelFormat = PixelFormat.Format24bppRgb; break;
                case 32: pixelFormat = PixelFormat.Format32bppRgb; break;
                case 48: pixelFormat = PixelFormat.Format48bppRgb; break;
                default: throw new Exception("Unknown BitCount");
            }
           

            Bitmap b = new Bitmap(width, height, stride, pixelFormat, p);
            b.RotateFlip(RotateFlipType.RotateNoneFlipY);
        

            Marshal.FreeHGlobal(p);

            return b;
             
        }

        protected override void WndProc(ref Message m)
        {
            switch (m.Msg)
            {
                case WM_GRAPHNOTIFY:
                    {
                        HandleGraphEvent();
                        break;
                    }
            }

            // Pass this message to the video window for notification of system changes
            if (this.videoWindow != null)
                this.videoWindow.NotifyOwnerMessage(m.HWnd, m.Msg, m.WParam, m.LParam);

            base.WndProc(ref m);
        }


        public void HandleGraphEvent()
        {
            int hr = 0;
            EventCode evCode;
            IntPtr evParam1, evParam2;

            if (this.mediaEventEx == null)
                return;

            while (this.mediaEventEx.GetEvent(out evCode, out evParam1, out evParam2, 0) == 0)
            {
                // Free event parameters to prevent memory leaks associated with
                // event parameter data.  While this application is not interested
                // in the received events, applications should always process them.
                hr = this.mediaEventEx.FreeEventParams(evCode, evParam1, evParam2);
                DsError.ThrowExceptionForHR(hr);

                _logger.WriteInfo("Received event from graph manager object - " + evCode.ToString());
                // Insert event processing code here, if desired
            }
        }
   
        public List<string> GetAvailResolutions(string deviceName)
        {
            int hr = 0;
            DsDevice dev = _devices.Where(d => d.Name == deviceName).First();
            if (captureGraphBuilder == null)
                Init(_logger);

       
            _resolutions.Clear();
            IBaseFilter capFilter = null;
            hr = filterGraph2.AddSourceFilterForMoniker(dev.Mon, null, dev.Name, out capFilter);
            DsError.ThrowExceptionForHR(hr);

            // Find the stream config interface
            object o;
            hr = captureGraphBuilder.FindInterface(PinCategory.Capture, MediaType.Video, capFilter, typeof(IAMStreamConfig).GUID, out o);

            IAMStreamConfig config = o as IAMStreamConfig;
            int cnt = 0;
            int size = 0;
            hr = config.GetNumberOfCapabilities(out cnt, out size);
            DsError.ThrowExceptionForHR(hr);
            
            IntPtr buff = Marshal.AllocCoTaskMem(size);
            for (int i = 0; i < cnt; i++ )
            {
                AMMediaType amt = null;
                hr = config.GetStreamCaps(i, out amt, buff);
                DsError.ThrowExceptionForHR(hr);

                if (amt.formatType == FormatType.VideoInfo)
                {
                    VideoInfoHeader v = new VideoInfoHeader();
                    Marshal.PtrToStructure(amt.formatPtr, v);
                    string s = v.BmiHeader.Width.ToString() + "x" + Math.Abs(v.BmiHeader.Height).ToString();
                    if (_resolutions.Contains(s) == false)
                        _resolutions.Add(s);
                }
                else if (amt.formatType == FormatType.VideoInfo2)
                {
                    VideoInfoHeader2 v = new VideoInfoHeader2();
                    Marshal.PtrToStructure(amt.formatPtr, v);
                    string s = v.BmiHeader.Width.ToString() + "x" + Math.Abs(v.BmiHeader.Height).ToString();
                    if (_resolutions.Contains(s) == false)
                        _resolutions.Add(s);
                }
                DsUtils.FreeAMMediaType(amt);
            }

            Marshal.FreeCoTaskMem(buff);
            Marshal.ReleaseComObject(capFilter);
            Marshal.ReleaseComObject(config);

            return _resolutions;
        }

    }
}
