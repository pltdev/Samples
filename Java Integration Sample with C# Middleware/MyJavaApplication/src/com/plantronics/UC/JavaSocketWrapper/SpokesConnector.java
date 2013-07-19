/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.plantronics.UC.JavaSocketWrapper;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.StringTokenizer;

/**
 *
 * @author lcollins
 */
public class SpokesConnector implements Runnable
{
    Socket socket;
    PrintWriter out;
    BufferedReader in;
    Thread readThread;
    ISpokesEvents spokesEventHandler = null;
    public boolean quit = false;
             
    public void SetSpokesEventHandler(ISpokesEvents aSpokesEventHandler)
    {
        spokesEventHandler = aSpokesEventHandler;
    }
    
    public void ConnectToSpokes()
    {            
        // start a read thread to read events from Spokes
        readThread = new Thread(this);
        readThread.start();
    }
    
    private void connectSocket()
    {
        while (!quit && !isConnected())
        {
            CreateSocketConnection();
            if (!isConnected())
            {
                System.out.println("INFO: Awaiting SpokesJavaMiddleware to be run so we can connect...");
                try
                {
                    Thread.sleep(2000);
                }
                catch (InterruptedException e)
                {
                    System.out.println("Sleep interrupted");
                }
            }
        }                  
    }
    
    private void CreateSocketConnection() 
    {
        //Create socket connection
        try
        {
            socket = new Socket("localhost", 8887);
            out = new PrintWriter(socket.getOutputStream(), 
                true);
            in = new BufferedReader(new InputStreamReader(
                socket.getInputStream()));

        } 
        catch (UnknownHostException e) 
        {
            System.out.println("Unknown host: localhost");
            System.exit(1);
        } 
        catch (IOException e) 
        {
            System.out.println("No I/O");
        }
    }
    
    // your app can call this to send commands to Spokes
    public void sendCommand(String commandText)
    {
        if (out != null)
        {
            out.println(commandText);  
        }
    }
    
    private void disconnectSocket()
    {
        //Close socket connection
        try
        {
            if (in!=null) in.close();
            if (out!=null) out.close();
            if (socket!=null) socket.close();
        } 
        catch (IOException e) 
        {
            System.out.println("Exception closing socket");
        }  
    }
    
    // sit and wait for events from Spokes
    public void run()
    {
        System.out.println("About to connectSocket...");
        connectSocket();
        
        String line;
        while(!quit && socket.isConnected())
        {
            try
            {
                if (socket.isConnected())
                {
                    line = in.readLine();
                    if (line!=null)
                    {
                        if (line.length()>0)
                        {
                            if (line.compareToIgnoreCase("quit")==0)
                            {
                                // quit from thread
                                System.out.println("Spokes said: quit");
                                break;
                            }
                            else if (line.compareToIgnoreCase("command ok")==0)
                            {
                                // we received a command acknowledgement from Spokes
                                System.out.println("Spokes said: command ok");
                            }
                            else if (line.compareToIgnoreCase("Hello From Spokes")==0)
                            {
                                System.out.println("Spokes said hello to us.");
                            }
                            else
                            {            
                                //Append data to text area
                                System.out.println("Event: "+line);

                                parseEvent(line); // act on event

                                //Send event acknowledgement back to Spokes
                                out.println("event ok");
                                out.flush();
                            }
                        }
                    }
                }
                else                   
                {
                    System.out.println("Socket not connected, quitting");
                    break;
                }
            }
            catch (IOException e) 
            {
                System.out.println("Read failed - was SpokesJavaMiddleware shut down?");
                disconnectSocket();
                System.out.println("Disconnected Socket. Halting application...");
                System.exit(1);
            }
        }
        disconnectSocket();
    }
    
    public boolean isConnected()
    {
        return (socket!=null && socket.isConnected());
    }
    
    private void parseEvent(String line)
    {
        if (spokesEventHandler==null)
        {
            System.out.println("ERROR: unable to process event, spokesEventHandler is NULL");
            System.out.println("INFO: you need to set one in your app code using SetSpokesEventHandler method");
            return;
        }
        
        String tokens[] = new String[10]; //max tokens 10
        for (int i=0;i<10;i++) { tokens[i]=""; }
        
        String[] items = line.split(",");
        
        for (int k=0;k<items.length;k++)
        {
            tokens[k] = items[k];
        }
        
        System.out.println("parseEvent: "+tokens[0]);

        // extra debug if needed...
//        for (int j=0;j<10;j++)
//        {
//            System.out.println("tokens["+j+"]="+tokens[j]);
//        }
        
        // temp variables for extracting event parameters...
        boolean worn, initial, docked;
        boolean HasCallerId, HasDocking, HasMultiline, HasProximity,
                HasWearingSensor, IsWireless;
        String callerid, serialnum;
        boolean incoming, muted, lineactive;
        MobileCallState mobcallstate;
        SerialNumberTypes serialtype;
        int callid;
        String callsource, prodname, prodid;
        OnCallCallState callstate;
        boolean pcactive, pcheld, mobileactive, mobileheld, deskphoneactive,
                deskphoneheld;
        
        // which event did we receive?
        switch (tokens[0].toLowerCase())
        {
            case "takenoff":
                worn = Boolean.parseBoolean(tokens[1]);
                initial = Boolean.parseBoolean(tokens[2]);
                spokesEventHandler.Spokes_TakenOff(new WearingStateArgs(worn, initial));
                break;
            case "puton":
                worn = Boolean.parseBoolean(tokens[1]);
                initial = Boolean.parseBoolean(tokens[2]);
                spokesEventHandler.Spokes_PutOn(new WearingStateArgs(worn, initial));
                break;
            case "near":
                spokesEventHandler.Spokes_Near(new EventArgs());
                break;
            case "far":
                spokesEventHandler.Spokes_Far(new EventArgs());
                break;
            case "capabilitieschanged":
                HasCallerId = Boolean.parseBoolean(tokens[1]);
                HasDocking = Boolean.parseBoolean(tokens[2]);
                HasMultiline = Boolean.parseBoolean(tokens[3]);
                HasProximity = Boolean.parseBoolean(tokens[4]);
                HasWearingSensor = Boolean.parseBoolean(tokens[5]);
                IsWireless = Boolean.parseBoolean(tokens[6]);
                spokesEventHandler.Spokes_CapabilitiesChanged(new CapabilitiesEventArgs(
                        HasCallerId, HasDocking, HasMultiline, HasProximity,
                        HasWearingSensor, IsWireless));
                break;
            case "docked":
                docked = Boolean.parseBoolean(tokens[1]);
                initial = Boolean.parseBoolean(tokens[2]);
                spokesEventHandler.Spokes_Docked(new DockedStateArgs(docked, initial));
                break;
            case "undocked":
                docked = Boolean.parseBoolean(tokens[1]);
                initial = Boolean.parseBoolean(tokens[2]);
                spokesEventHandler.Spokes_UnDocked(new DockedStateArgs(docked, initial));
                break;
            case "mobilecallerid":
                callerid = tokens[1];
                spokesEventHandler.Spokes_MobileCallerId(new MobileCallerIdArgs(callerid));
                break;
            case "onmobilecall":
                incoming = Boolean.parseBoolean(tokens[1]);
                mobcallstate = MobileCallState.values()[Integer.parseInt(tokens[2])];
                spokesEventHandler.Spokes_OnMobileCall(new OnMobileCallArgs(incoming, mobcallstate));
                break;
            case "serialnumber":
                serialnum = tokens[1];
                serialtype = SerialNumberTypes.values()[Integer.parseInt(tokens[2])];
                spokesEventHandler.Spokes_SerialNumber(new SerialNumberArgs(serialnum, serialtype));
                break;
            case "callanswered":
                callid = Integer.parseInt(tokens[1]);
                callsource = tokens[2];
                spokesEventHandler.Spokes_CallAnswered(new CallAnsweredArgs(callid, callsource));
                break;
            case "callended":
                callid = Integer.parseInt(tokens[1]);
                callsource = tokens[2];
                spokesEventHandler.Spokes_CallEnded(new CallEndedArgs(callid, callsource));
                break;
            case "oncall":
                incoming = Boolean.parseBoolean(tokens[1]);
                callstate = OnCallCallState.values()[Integer.parseInt(tokens[2])];
                callsource = tokens[3];
                spokesEventHandler.Spokes_OnCall(new OnCallArgs(callsource, incoming, callstate));
                break;
            case "mutechanged":
                muted = Boolean.parseBoolean(tokens[1]);
                spokesEventHandler.Spokes_MuteChanged(new MuteChangedArgs(muted));
                break;
            case "lineactivechanged":
                lineactive = Boolean.parseBoolean(tokens[1]);
                spokesEventHandler.Spokes_LineActiveChanged(new LineActiveChangedArgs(lineactive));
                break;
            case "attached":
                prodname = tokens[1];
                prodid = tokens[2];
                spokesEventHandler.Spokes_Attached(new AttachedArgs(prodname, prodid));
                break;
            case "multilinestatechanged":
                pcactive = Boolean.parseBoolean(tokens[1]);
                pcheld = Boolean.parseBoolean(tokens[2]);
                mobileactive = Boolean.parseBoolean(tokens[3]);
                mobileheld = Boolean.parseBoolean(tokens[4]);
                deskphoneactive = Boolean.parseBoolean(tokens[5]);
                deskphoneheld = Boolean.parseBoolean(tokens[6]);
                spokesEventHandler.Spokes_MultiLineStateChanged(new MultiLineStateArgs(
                        new MultiLineStateFlags(pcactive, pcheld, mobileactive, mobileheld, deskphoneactive, deskphoneheld)));
                break;
            case "inrange":
                spokesEventHandler.Spokes_InRange(new EventArgs());
                break;
            case "outofrange":
                spokesEventHandler.Spokes_OutOfRange(new EventArgs());
                break;
            case "notonmobilecall":
                spokesEventHandler.Spokes_NotOnMobileCall(new EventArgs());
                break;
            case "callswitched":
                spokesEventHandler.Spokes_CallSwitched(new EventArgs());
                break;
            case "notoncall":
                spokesEventHandler.Spokes_NotOnCall(new EventArgs());
                break;
            case "detached":
                spokesEventHandler.Spokes_Detached(new EventArgs());
                break;
            default:
                System.out.println("WARNING: Unknown event from Spokes: "+tokens[0]);
                break;
        }        
    }

    private void Delay(int millis) 
    {
        try
        {
            Thread.sleep(millis);
        }
        catch (InterruptedException e)
        {
            System.out.println("Sleep interrupted");
        }
    }
}
