using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Net.Sockets;
using System.Net;
using Plantronics.UC.SpokesWrapper;

namespace Plantronics.UC.SpokesSocketMiddleware
{
    public class SpokesSocketInterface
    {
        private byte[] buffer { get; set; }
        private Socket serverSocket, clientSocket;
        private bool quit;
        private Thread socketThread;

        static Spokes spokes;
        private bool connected = false; // connected to Spokes yet?

        public SpokesSocketInterface(Spokes aSpokes)
        {
            spokes = aSpokes;
        }

        public void StartSocket()
        {
            //socketThread = new 

            serverSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            serverSocket.Bind(new IPEndPoint(0, 8887));
            Console.WriteLine("Awaiting Connection");
            serverSocket.Listen(0);

            while (!quit)
            {
                clientSocket = serverSocket.Accept();
                Console.WriteLine(" >> Accept connection from client");

                sendMsg("Hello From Spokes");

                if (!connected) spokes.Connect("Spokes Socket Middleware");

                recMsg();
            }
        }

        public void StopSocket()
        {
            if (clientSocket!=null) clientSocket.Disconnect(false);
            if (serverSocket!=null) serverSocket.Disconnect(false);
        }

        // receive from client forever 
        // a combination of client Spokes command requests
        // and Spokes event acknowledgements
        public void recMsg()
        {
            while (clientSocket.Connected)
            {
                try
                {
                    Thread.Sleep(100);
                    byte[] Buffer = new byte[1024];
                    int rec = clientSocket.Receive(Buffer, 0, Buffer.Length, 0);
                    Array.Resize(ref Buffer, rec);
                    System.Text.Encoding enc = System.Text.Encoding.ASCII;
                    string rcv = enc.GetString(Buffer);
                    string[] lines = rcv.Split(new string[] { Environment.NewLine }, StringSplitOptions.None); // split at line feed...
                    foreach (string line in lines)
                    {
                        if (line.Length > 0)
                        {
                            Console.WriteLine("RECEIVED = " + line);

                            if (line.ToLower().StartsWith("quit"))
                            {
                                Console.WriteLine("Client app wants to quit.");
                                sendMsg("quit"); // tell client to quit its event reading thread!
                            }
                            else if (line.ToLower().StartsWith("event ok")) // got acknowledgement from client
                            {
                                Console.WriteLine("Client app confirmed received event ok.");
                                // no action
                            }
                            else
                            {
                                Parsecommand(line);

                                sendMsg("command ok"); // send acknowledgement to client
                            }
                        }
                    }
                }
                catch (Exception e)
                {
                    Console.WriteLine("Info Exception = " + e.ToString());
                }
            }
        }

        private static void Parsecommand(string rcv)
        {
            // tokenize command...
            string[] tokens = new string[10];
            for (int i = 0; i < 10; i++) { tokens[i] = ""; }
            string[] words = rcv.Split(',');
            int curtok = 0;
            foreach (string word in words)
            {
                tokens[curtok] = word;
                curtok++;
            }

            Console.WriteLine("Parsecommand: "+tokens[0]);

            if (tokens[0].Length>0)
            {
                switch (tokens[0].ToLower())
                {
                    case "ansmob":
                        spokes.AnswerMobileCall();
                        break;
                    case "audioon":
                        spokes.ConnectAudioLinkToDevice(Convert.ToBoolean(tokens[1]));
                        break;
                    case "audiooff":
                        spokes.ConnectAudioLinkToDevice(Convert.ToBoolean(tokens[1]));
                        break;
                    case "dialmob":
                        spokes.DialMobileCall(tokens[1]);
                        break;
                    case "endcall":
                        spokes.EndCall(Convert.ToInt32(tokens[1]));
                        break;
                    case "endmobcall": 
                        break;
                    case "incoming": 
                        // incoming call (ring headset)
                        spokes.IncomingCall(Convert.ToInt32(tokens[1]), tokens[2]);
                        break;
                    case "muteon":
                        spokes.SetMute(Convert.ToBoolean(tokens[1]));
                        break;
                    case "muteoff":
                        spokes.SetMute(Convert.ToBoolean(tokens[1]));
                        break;
                    case "outgoing":
                        // outgoing call
                        spokes.OutgoingCall(Convert.ToInt32(tokens[1]), tokens[2]);
                        break;
                    case "reqserial":
                        int serialtype = Convert.ToInt32(tokens[1]);
                        spokes.RequestSingleSerialNumber(serialtype == 1 ? SerialNumberTypes.Base : SerialNumberTypes.Headset);
                        break;
                    case "setlineactive":
                        spokes.SetLineActive((Multiline_LineType)(Convert.ToInt32(tokens[1]) - 1), 
                            Convert.ToInt32(tokens[2]) == 1 ? true : false);
                        break;
                    case "setlinehold":
                        spokes.SetLineHold((Multiline_LineType)(Convert.ToInt32(tokens[1]) - 1),
                            Convert.ToInt32(tokens[2]) == 1 ? true : false);
                        break;
                    default:
                        Console.WriteLine("WARNING: Command not recognised.");
                        break;
                }
            }


            // parse incoming command
            if (rcv.ToLower().StartsWith("incoming"))
            {
                // ring headset
                spokes.IncomingCall(1, "Lew");
            }
        }

        // send msg to connected client
        public void sendMsg(string s)
        {
            if (clientSocket == null)
            {
                Console.WriteLine("INFO: No client connected.");
                return;
            }
            try
            {
                if (!s.EndsWith("\r\n")) s = s + "\r\n";
                buffer = Encoding.Default.GetBytes(s);
                clientSocket.Send(buffer);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
        }
    }
}
