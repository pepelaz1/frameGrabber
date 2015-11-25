using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;


namespace FrameGrabberApp
{
    public class Logger
    {
        private string _fileName;

        
        public void Init(string fileName)
        {
            _fileName = fileName;
        }

        public void WriteInfo(string msg)
        {
            WriteLine("INFO: " + msg);
        }

        public void WriteWarning(string msg)
        {
            WriteLine("WARNING: " + msg);
        }

        public void WriteError(string msg)
        {
            WriteLine("ERROR: " + msg);
        }


        private void WriteLine(string msg)
        {
            msg = DateTime.Now.ToString("dd.MM.yyyy hh:mm:ss") + "|" + msg + Environment.NewLine;
            File.AppendAllText(_fileName, msg);
        }
    }
}
