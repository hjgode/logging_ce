using System;
using System.Linq;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using OpenNETCF.WindowsCE;



namespace DeviceDetect
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            WatchForDevices();

        }

        DeviceStatusMonitor monitor;
        private int iCnt = 0;
        private void WatchForDevices()
        {
            monitor = new DeviceStatusMonitor(DeviceClass.Any, false);
            monitor.StartStatusMonitoring();
            monitor.DeviceNotification+=new DeviceNotificationEventHandler(monitor_DeviceNotification);
            addLog("WatchForDevices started...");
        }

        void monitor_DeviceNotification(object sender, DeviceNotificationArgs e)
        {
            string message = string.Format("{4}.Device '{0}' has been {1} ({2}/{3}.", e.DeviceName, e.DeviceAttached ? "inserted" : "removed", e.DeviceClass, e.DeviceInterfaceGUID.ToString(), iCnt.ToString("000") );
            //MessageBox.Show(message, "Disk Status");
            addLog( message);
            iCnt++;
        }

        delegate void SetTextCallback(string text);
        private void addLog(string text)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (this.txtLog.InvokeRequired)
            {
                SetTextCallback d = new SetTextCallback(addLog);
                this.Invoke(d, new object[] { text });
            }
            else
            {
                if (txtLog.Text.Length > 2000)
                    txtLog.Text = "";
                DateTime dt = DateTime.Now;
                txtLog.Text += dt.ToShortTimeString() + ":\r\n";
                txtLog.Text += text + "\r\n";
                txtLog.SelectionLength = 0;
                txtLog.SelectionStart = txtLog.Text.Length - 1;
                txtLog.ScrollToCaret();
            }
        }

        private void Form1_Closing(object sender, CancelEventArgs e)
        {
            if (monitor != null)
            {
                monitor.StopStatusMonitoring();
                monitor.Dispose();
                monitor = null;
            }
        }
    }
}