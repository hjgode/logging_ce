using System;
using System.Linq;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace dumpUUIDssCF
{
    class Program
    {
        static string sLogFile = "\\dumpUUIDss.log.txt";
        static bool bFirstCall = true;

        static void Main(string[] args)
        {
            addLog("++++++++ dumUUIDssCF v0.3 ++++++++");
            string sFile = "\\ssUUID.txt";
            if (args.Length > 0)
            {
                addLog("started with args:");
                foreach (string a in args)
                    addLog("'" + a + "'");

                string s = args[0];
                string sP = System.IO.Path.GetDirectoryName(s);
                if(!sP.EndsWith("\\"))
                    sP+="\\";
                if (System.IO.Directory.Exists(sP))
                    sFile = s;
                else
                    addLog("cmd argument '" + s + "'does not name valid path");
            }

            System.Diagnostics.Debug.WriteLine("DeviceID='" + Intermec.DevHealth.SystemHealth.GetDeviceID()+"'");
            System.Diagnostics.Debug.WriteLine("SS UUID='" + Intermec.DevHealth.SystemHealth.GetDeviceSS_UUID()+"'");

            writeInfo(sFile);

        }

        static void writeInfo(string sFile)
        {
            addLog("writing to '" + sFile + "'");
            try
            {
                using (TextWriter tw = new System.IO.StreamWriter(sFile, true))
                {
                    tw.Write(Intermec.DevHealth.SystemHealth.GetDeviceSS_UUID() + "\t");
                    tw.Write(Intermec.DevHealth.SystemHealth.getDeviceSerial() + "\t");
                    tw.Write(Intermec.DevHealth.SystemHealth.getDateTime());

                    tw.WriteLine();
                    tw.Flush();
                }
            }
            catch (Exception ex)
            {
                addLog("Exception in write file: " + ex.Message);
            }
        }

        static void addLog(string s)
        {
            System.Diagnostics.Debug.WriteLine("addLog: '"+s+"'");
            if (bFirstCall) //delete log file
            {
                try
                {
                    if (System.IO.File.Exists(sLogFile))
                        System.IO.File.Delete(sLogFile);
                }
                catch (Exception) { }
                bFirstCall = false;
            }
            try
            {
                using (TextWriter tw = new System.IO.StreamWriter(sLogFile, true))
                {
                    tw.WriteLine(s);
                    tw.Flush();
                }
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine("Exception in write log file: " + ex.Message);
            }
        }
    }
}
