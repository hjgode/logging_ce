using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Runtime.InteropServices;

namespace Intermec.DevHealth
{
    #region DEVICE_UUID
    //ssRefClient:  {92B88F1F-A6DD-4735-868F-6E64B31CA9FC}
    //DeviceID:     0040056A-0E9D-0801-0BD0-011700003055
    //SS Health UUID: 6a0540009d0e01080bd0011700003055
    //                6a054000-9d0e-0108-0bd0-011700003055
    // Bytes hex:     6A054000-9D0E-0108-0BD0-011700003055
    //this:         006F0050-0063-006B-0065-434E3531333038583133303030373500    //DeviceID

    class SystemHealth
    {
        private static Int32 FILE_DEVICE_HAL = 0x00000101;
        private static Int32 FILE_ANY_ACCESS = 0x0;
        private static Int32 METHOD_BUFFERED = 0x0;
        private static Int32 IOCTL_HAL_GET_DEVICEID = ((FILE_DEVICE_HAL) << 16) | ((FILE_ANY_ACCESS) << 14) | ((21) << 2) | (METHOD_BUFFERED);
        //IOCTL_HAL_GET_UUID          CTL_CODE(FILE_DEVICE_HAL, 13, METHOD_BUFFERED, FILE_ANY_ACCESS);
        private static Int32 IOCTL_HAL_GET_UUID =  ((FILE_DEVICE_HAL) << 16) | ((FILE_ANY_ACCESS) << 14) | ((13) << 2) | (METHOD_BUFFERED);

        [DllImport("coredll.dll")]
        private static extern bool KernelIoControl(Int32 IoControlCode, IntPtr
          InputBuffer, Int32 InputBufferSize, byte[] OutputBuffer, Int32
          OutputBufferSize, ref Int32 BytesReturned);
        
        public static string GetDeviceSS_UUID()
        {
            Guid myGuid;
            byte[] OutputBuffer = new byte[256];
            Int32 OutputBufferSize, BytesReturned;
            OutputBufferSize = OutputBuffer.Length;
            BytesReturned = 0;
            // values
            bool retVal = KernelIoControl(IOCTL_HAL_GET_UUID,
                    IntPtr.Zero,
                    0,
                    OutputBuffer,
                    OutputBufferSize,
                    ref BytesReturned);
            if (retVal == false)
            {
                return null;
            }
            Int32 PresetIDOffset = BitConverter.ToInt32(OutputBuffer, 4);
            //For UUID
            PresetIDOffset = 0; //no offset
            Int32 PlatformIDOffset = BitConverter.ToInt32(OutputBuffer, 0xc);
            Int32 PlatformIDSize = BitConverter.ToInt32(OutputBuffer, 0x10);
            /*
                TEXT("UUID: %08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\r\n"),
                    myUUID.Data1,
                    myUUID.Data2,
                    myUUID.Data3,
                    myUUID.Data4[0],
                    myUUID.Data4[1],
                    myUUID.Data4[2],
                    myUUID.Data4[3],
                    myUUID.Data4[4],
                    myUUID.Data4[5],
                    myUUID.Data4[6],
                    myUUID.Data4[7]);
            */
            byte[] bGuid = new byte[BytesReturned];
            Array.Copy(OutputBuffer, bGuid, BytesReturned);
            myGuid = new Guid(bGuid);
            System.Diagnostics.Debug.WriteLine("myUUID:" + myGuid.ToString());
            //  myUUID:0040056a-0e9d-0801-0bd0-011700003055
            // Convert the Preset ID segments into a string so they can be 
            // displayed easily.
            StringBuilder sb = new StringBuilder();
            sb.Append(String.Format("{0:X8}-{1:X4}-{2:X4}-{3:X4}-",
                 BitConverter.ToInt32(OutputBuffer, PresetIDOffset),
                 BitConverter.ToInt16(OutputBuffer, PresetIDOffset + 4),
                 BitConverter.ToInt16(OutputBuffer, PresetIDOffset + 6),
                 BitConverter.ToInt16(OutputBuffer, PresetIDOffset + 8)));
            //=> gives DeviceID='{0040056A-0E9D-0801-D00B-}

            // Break the UUID the SmartSystems way
            StringBuilder sbBytesHex = new StringBuilder();
            for (int i = 0; i < BytesReturned; i++)
            {
                sbBytesHex.Append(String.Format("{0:X2}", OutputBuffer[i]));
            }
            // UUID='6A0540009D0E01080BD0011700003055'
            System.Diagnostics.Debug.WriteLine("Bytes hex: " + sbBytesHex.ToString());
            // Bytes hex: 6A0540009D0E01080BD0011700003055
            return sbBytesHex.ToString();
        }
        
        public static string GetDeviceID()
        {
            byte[] OutputBuffer = new byte[256];
            Int32 OutputBufferSize, BytesReturned;
            OutputBufferSize = OutputBuffer.Length;
            BytesReturned = 0;

            // Call KernelIoControl passing the previously defined
            // IOCTL_HAL_GET_DEVICEID parameter
            // We don’t need to pass any input buffers to this call
            // so InputBuffer and InputBufferSize are set to their null
            // values
            bool retVal = KernelIoControl(IOCTL_HAL_GET_DEVICEID,
                    IntPtr.Zero,
                    0,
                    OutputBuffer,
                    OutputBufferSize,
                    ref BytesReturned);

            // If the request failed, exit the method now
            if (retVal == false)
            {
                return null;
            }

            // Examine the OutputBuffer byte array to find the start of the 
            // Preset ID and Platform ID, as well as the size of the
            // PlatformID. 
            // PresetIDOffset – The number of bytes the preset ID is offset
            //                  from the beginning of the structure
            // PlatformIDOffset - The number of bytes the platform ID is
            //                    offset from the beginning of the structure
            // PlatformIDSize - The number of bytes used to store the
            //                  platform ID
            // Use BitConverter.ToInt32() to convert from byte[] to int
            Int32 PresetIDOffset = BitConverter.ToInt32(OutputBuffer, 4);
            Int32 PlatformIDOffset = BitConverter.ToInt32(OutputBuffer, 0xc);
            Int32 PlatformIDSize = BitConverter.ToInt32(OutputBuffer, 0x10);

            // Convert the Preset ID segments into a string so they can be 
            // displayed easily.
            StringBuilder sb = new StringBuilder();
            sb.Append(String.Format("{0:X8}-{1:X4}-{2:X4}-{3:X4}-",
                 BitConverter.ToInt32(OutputBuffer, PresetIDOffset),
                 BitConverter.ToInt16(OutputBuffer, PresetIDOffset + 4),
                 BitConverter.ToInt16(OutputBuffer, PresetIDOffset + 6),
                 BitConverter.ToInt16(OutputBuffer, PresetIDOffset + 8)));
            //=> gives DeviceID='006F0050-0063-006B-0065-434E3531333038583133303030373500'
            // Break the Platform ID down into 2-digit hexadecimal numbers
            // and append them to the Preset ID. This will result in a 
            // string-formatted Device ID
            for (int i = PlatformIDOffset;
                 i < PlatformIDOffset + PlatformIDSize;
                 i++)
            {
                sb.Append(String.Format("{0:X2}", OutputBuffer[i]));
            }

            // return the Device ID string
            return sb.ToString();
        }
        #endregion
        #region DEVICE_SERIAL
        [DllImport("itc50.dll", SetLastError = true)]
        static extern int ITCGetSerialNumber(byte[] number, int buffSize);

        [DllImport("itc50.dll", SetLastError = true)]
        static extern int ITCGetSerialNumber(System.Text.StringBuilder number, int buffSize);

        public static string getDeviceSerial()
        {
            string sSN = "n/a";
            try
            {
                int iSize=32;

                StringBuilder sbb = new StringBuilder(32);
                int iRes = ITCGetSerialNumber(sbb, iSize);

                if (iRes == 0)
                {
                    sSN = sbb.ToString();
                    sSN.Trim();
                    System.Diagnostics.Debug.WriteLine("sSN: '" + sSN + "'");
                }
                else
                {
                    System.Diagnostics.Debug.WriteLine("ITCGetSerialNumber failed lastError= " + Marshal.GetLastWin32Error().ToString());
                }
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine("Exception in ITCGetSerialNumber: " + ex.Message);
            }
            return sSN;
        }

        #endregion
        #region DATETIME_STRING
        public static string getDateTime()
        {
            DateTime dt = DateTime.Now;
            string sDT = dt.Year.ToString("0000")+ dt.Month.ToString("00")+ dt.Day.ToString("00") +
                dt.Hour.ToString("00")+ dt.Minute.ToString("00")+ dt.Second.ToString("00");
            return sDT;
        }
        #endregion
    }
}
