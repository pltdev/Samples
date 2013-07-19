using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Plantronics.Innovation.Util
{
    // A class to process incoming quats on a background thread
    // and return the computed angles and calibrated flag to
    // main application
    public class QuaternionProcessor
    {
        private Thread m_worker;
        private static HeadtrackingUpdateHandler m_headsetDataHandler;

        // List of incoming odp payloads
        static Queue<string> m_odppayloads = new Queue<string>();

        // locking flag/handle, thanks: http://www.albahari.com/threading/part4.aspx
        static readonly object _locker = new object();
        static bool _go = false;

        // flags for signalling to calibrate etc
        private static double[] calquat;
        private static bool calon = false;
        private static bool initialcalibrate = false; // flag to perform auto-calibrate once gyro calibrate is complete

        public QuaternionProcessor(HeadtrackingUpdateHandler headsetDataHandler)
        {
            calon = false;
            calquat = new double[4];
            for (int i = 0; i < 4; i++)
            {
                calquat[i] = 0;
            }

            m_headsetDataHandler = headsetDataHandler;
            m_worker = new Thread(ProcessorMainFunction);
            m_worker.Start();
        }
        private static void ProcessorMainFunction()
        {
            try
            {
                while (true)
                {
                    lock (_locker)
                        while (!_go)
                            Monitor.Wait(_locker);

                    string odppayload = "";

                    lock (_locker)
                    {
                        int count = m_odppayloads.Count();
                        if (count > 0)
                        {
                            // Dequeue the "freshest" odp report! (the one at the end/bottom
                            // of the queue)
                            odppayload = m_odppayloads.ElementAt(m_odppayloads.Count() - 1);
                            m_odppayloads.Clear(); //kill all the older ones
                        }

                        _go = false;
                    }

                    HeadsetTrackingData headsetdata = ExtractHeadsetData(odppayload);

                    if (!headsetdata.badpacket)
                        m_headsetDataHandler.HeadsetTrackingUpdate(headsetdata);                    
                }
            }
            catch (Exception)
            {
                // thread exit
            }
        }

        private static HeadsetTrackingData ExtractHeadsetData(string odpreport)
        {
            HeadsetTrackingData retval = new HeadsetTrackingData();

            // extract the headtracking payload:
            int lastcomma = odpreport.LastIndexOf(",");
            string odppayload = odpreport.Substring(lastcomma + 1, odpreport.Length - lastcomma - 2);

            // convert the headtracking payload into values:
            long[] quatn;
            double[] quat;
            quatn = new long[16];
            quat = new double[4];
            int gyrocalib = 0;

            bool badpacket = false;

            try
            {
                quatn[0] = ExtractQuatFromHex(odppayload.Substring(6, 4), 0); //, Quat0_hex_label);  // quat 0 is 4 hex chars from char 6 (0-based index)
                quatn[1] = ExtractQuatFromHex(odppayload.Substring(14, 4), 1); //, Quat1_hex_label);  // quat 1 is 4 hex chars from char 14 (0-based index)
                quatn[2] = ExtractQuatFromHex(odppayload.Substring(22, 4), 2); //, Quat2_hex_label);  // quat 2 is 4 hex chars from char 22 (0-based index)
                quatn[3] = ExtractQuatFromHex(odppayload.Substring(30, 4), 3); //, Quat3_hex_label);  // quat 3 is 4 hex chars from char 30 (0-based index)
                gyrocalib = ExtractIntFromHex(odppayload.Substring(36, 2));  // gyrocalibration info is 2 hex chars from char 36 (0-based index)
            }
            catch (Exception)
            {
                badpacket = true;
                retval.badpacket = true;
            }

            if (!badpacket)
            {
                //process for use
                for (int i = 0; i < 4; i++)
                {
                    if (quatn[i] > 32767)
                    {
                        quatn[i] -= 65536;
                    }
                    quat[i] = ((double)quatn[i]) / 16384.0f;

                }

                retval.gyrocalib = gyrocalib;

                // auto calibrate by headset:
                if (!initialcalibrate && gyrocalib == 3)
                {
                    calon = true;
                    initialcalibrate = true;
                }

                // handle calibrate request (auto or manual):
                if (calon)
                {
                    calquat = quat;
                    calon = false;
                }

                //correct for calibration
                double[] newquat = new double[4];

                //create inverse of cal vector
                newquat = quatinv(calquat);

                //reapply current state to cal
                newquat = quatmult(newquat, quat);


                //bank-roll
                retval.phi_roll = -180.0 / 3.14159 * Math.Asin(-2.0 * newquat[1] * newquat[3] + 2.0 * newquat[0] * newquat[2]);
                //heading
                retval.psi_heading = -180.0 / 3.14159 * Math.Atan2((newquat[2] * newquat[1] + newquat[0] * newquat[3]), (newquat[0] * newquat[0] + newquat[1] * newquat[1] - (double)0.5));
                //elevation-pitch
                retval.theta_pitch = -180.0 / 3.14159 * Math.Atan2((newquat[2] * newquat[3] + newquat[0] * newquat[1]), (newquat[0] * newquat[0] + newquat[3] * newquat[3] - (double)0.5));
            }

            return retval;
        }

        private static int ExtractIntFromHex(string hexchrs)
        {
            return Convert.ToByte(hexchrs, 16);
        }

        private static long ExtractQuatFromHex(string hexchrs, int labelnum)
        {
            string digit1hexchrs = hexchrs.Substring(0, 2);
            string digit2hexchrs = hexchrs.Substring(2, 2);
            byte digit1 = Convert.ToByte(digit1hexchrs, 16);
            byte digit2 = Convert.ToByte(digit2hexchrs, 16);

            ulong temp;
            temp = (((ulong)digit1) << 8) + ((ulong)digit2);
            return checked((long)temp);
        }

        public static double[] quatmult(double[] p, double[] q)
        {
            //quaternion multiplication
            double[] newquat = new double[4];

            double[,] quatmat = new double[,] { {p[0], -p[1], -p[2],-p[3]}, 
                                                {p[1], p[0], -p[3], p[2]  }, 
                                                {p[2], p[3], p[0], -p[1]  }, 
                                                {p[3], -p[2], p[1], p[0]  }, 
                                                };
            for (int i = 0; i < 4; i++)
                for (int j = 0; j < 4; j++)
                    newquat[i] += quatmat[i, j] * q[j];

            return newquat;
        }

        public static double[] quatinv(double[] quat)
        {
            double[] newquat = new double[4];
            newquat[0] = quat[0];
            newquat[1] = -quat[1];
            newquat[2] = -quat[2];
            newquat[3] = -quat[3];
            return newquat;
        }


        internal void Stop()
        {
            m_worker.Abort();
        }

        internal void ProcessODPReport(string odpreport)
        {
            lock (_locker)
            {
                m_odppayloads.Enqueue(odpreport);
                _go = true;
                Monitor.Pulse(_locker);
            }
        }

        internal void Calibrate(bool doinitialcalibrate = false)
        {
            calon = true; // lets calibrate now!
            if (doinitialcalibrate) initialcalibrate = true; // handle case where user manually calibrates before auto-calibrate is complete
        }
    }

    // struct to pass values back to app of processed headset tracking packet...
    public class HeadsetTrackingData
    {
        public double theta_pitch;
        public double psi_heading;
        public double phi_roll;
        public int gyrocalib;
        public bool badpacket;
    }

    // interface to allow main app to receive head tracking data...
    public interface HeadtrackingUpdateHandler
    {
        void HeadsetTrackingUpdate(HeadsetTrackingData headsetData);
    }
}
